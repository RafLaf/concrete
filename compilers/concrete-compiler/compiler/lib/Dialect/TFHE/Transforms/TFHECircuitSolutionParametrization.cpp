// Part of the Concrete Compiler Project, under the BSD3 License with Zama
// Exceptions. See
// https://github.com/zama-ai/concrete-compiler-internal/blob/main/LICENSE.txt
// for license information.

#include "concrete-optimizer.hpp"
#include "concretelang/Dialect/TFHE/IR/TFHEParameters.h"
#include "concretelang/Dialect/TypeInference/IR/TypeInferenceOps.h"

#include <concretelang/Analysis/TypeInferenceAnalysis.h>
#include <concretelang/Dialect/Optimizer/IR/OptimizerOps.h>
#include <concretelang/Dialect/TFHE/IR/TFHEOps.h>
#include <concretelang/Dialect/TFHE/IR/TFHETypes.h>
#include <concretelang/Dialect/TFHE/Transforms/Transforms.h>
#include <concretelang/Dialect/Tracing/IR/TracingOps.h>
#include <concretelang/Transforms/TypeInferenceRewriter.h>

#include <mlir/Analysis/DataFlow/ConstantPropagationAnalysis.h>
#include <mlir/Analysis/DataFlow/DeadCodeAnalysis.h>
#include <mlir/Analysis/DataFlow/SparseAnalysis.h>
#include <mlir/Dialect/Arith/IR/Arith.h>
#include <mlir/Dialect/Bufferization/IR/Bufferization.h>
#include <mlir/Dialect/SCF/IR/SCF.h>
#include <mlir/Dialect/Tensor/IR/Tensor.h>
#include <mlir/Transforms/GreedyPatternRewriteDriver.h>

namespace mlir {
namespace concretelang {
namespace {
// Return the element type of `t` if `t` is a tensor or memref type
// or `t` itself
template <class T> static std::optional<T> tryGetScalarType(mlir::Type t) {
  if (T ctt = t.dyn_cast<T>())
    return ctt;

  if (mlir::RankedTensorType rtt = t.dyn_cast<mlir::RankedTensorType>())
    return tryGetScalarType<T>(rtt.getElementType());
  else if (mlir::MemRefType mrt = t.dyn_cast<mlir::MemRefType>())
    return tryGetScalarType<T>(mrt.getElementType());

  return std::nullopt;
}

// Wraps a `::concrete_optimizer::dag::CircuitSolution` and provides
// helper functions for lookups and code generation
class CircuitSolutionWrapper {
public:
  CircuitSolutionWrapper(
      const ::concrete_optimizer::dag::CircuitSolution &solution)
      : solution(solution) {}

  enum class SolutionKeyKind {
    OPERAND,
    RESULT,
    KSK_IN,
    KSK_OUT,
    CKSK_IN,
    CKSK_OUT,
    BSK_IN,
    BSK_OUT
  };

  // Returns the `GLWESecretKey` type for a secrete key
  TFHE::GLWESecretKey
  toGLWESecretKey(const ::concrete_optimizer::dag::SecretLweKey &key) const {
    return TFHE::GLWESecretKey::newParameterized(
        key.glwe_dimension * key.polynomial_size, 1, key.identifier);
  }

  // Looks up the keys associated to an operation with a given `oid`
  const ::concrete_optimizer::dag::InstructionKeys &
  lookupInstructionKeys(int64_t oid) const {
    assert(oid <= (int64_t)solution.instructions_keys.size() &&
           "Invalid optimizer ID");

    return solution.instructions_keys[oid];
  }

  // Returns a `GLWEKeyswitchKeyAttr` for a given keyswitch key
  // (either of type `KeySwitchKey` or `ConversionKeySwitchKey`)
  template <typename KeyT>
  TFHE::GLWEKeyswitchKeyAttr getKeyswitchKeyAttr(mlir::MLIRContext *ctx,
                                                 const KeyT &ksk) const {
    return TFHE::GLWEKeyswitchKeyAttr::get(
        ctx, toGLWESecretKey(ksk.input_key), toGLWESecretKey(ksk.output_key),
        ksk.ks_decomposition_parameter.level,
        ksk.ks_decomposition_parameter.log2_base, -1);
  }

  // Returns a `GLWEKeyswitchKeyAttr` for the keyswitch key of an
  // operation tagged with a given `oid`
  TFHE::GLWEKeyswitchKeyAttr getKeyswitchKeyAttr(mlir::MLIRContext *ctx,
                                                 int64_t oid) const {
    const ::concrete_optimizer::dag::KeySwitchKey &ksk =
        lookupKeyswitchKey(oid);

    return getKeyswitchKeyAttr(ctx, ksk);
  }

  // Returns a `GLWEBootstrapKeyAttr` for the bootstrap key of an
  // operation tagged with a given `oid`
  TFHE::GLWEBootstrapKeyAttr getBootstrapKeyAttr(mlir::MLIRContext *ctx,
                                                 int64_t oid) const {
    const ::concrete_optimizer::dag::BootstrapKey &bsk =
        lookupBootstrapKey(oid);

    return TFHE::GLWEBootstrapKeyAttr::get(
        ctx, toGLWESecretKey(bsk.input_key), toGLWESecretKey(bsk.output_key),
        bsk.output_key.polynomial_size, bsk.output_key.glwe_dimension,
        bsk.br_decomposition_parameter.level,
        bsk.br_decomposition_parameter.log2_base, -1);
  }

  // Looks up the keyswitch key for an operation tagged with a given
  // `oid`
  const ::concrete_optimizer::dag::KeySwitchKey &
  lookupKeyswitchKey(int64_t oid) const {
    uint64_t keyID = lookupInstructionKeys(oid).tlu_keyswitch_key;
    return solution.circuit_keys.keyswitch_keys[keyID];
  }

  // Looks up the bootstrap key for an operation tagged with a given
  // `oid`
  const ::concrete_optimizer::dag::BootstrapKey &
  lookupBootstrapKey(int64_t oid) const {
    uint64_t keyID = lookupInstructionKeys(oid).tlu_bootstrap_key;
    return solution.circuit_keys.bootstrap_keys[keyID];
  }

  // Looks up the conversion keyswitch key for an operation tagged
  // with a given `oid`
  const ::concrete_optimizer::dag::ConversionKeySwitchKey &
  lookupConversionKeyswitchKey(uint64_t oid) const {
    uint64_t keyID = lookupInstructionKeys(oid).extra_conversion_keys[0];
    return solution.circuit_keys.conversion_keyswitch_keys[keyID];
  }

  // Looks up the conversion keyswitch key for the conversion of the
  // key with the ID `fromKeyID` to the key with the ID `toKeyID`. The
  // key must exist, otherwise an assertion is triggered.
  const ::concrete_optimizer::dag::ConversionKeySwitchKey &
  lookupConversionKeyswitchKey(uint64_t fromKeyID, uint64_t toKeyID) const {
    auto convKSKIt = std::find_if(
        solution.circuit_keys.conversion_keyswitch_keys.cbegin(),
        solution.circuit_keys.conversion_keyswitch_keys.cend(),
        [&](const ::concrete_optimizer::dag::ConversionKeySwitchKey &arg) {
          return arg.input_key.identifier == fromKeyID &&
                 arg.output_key.identifier == toKeyID;
        });

    assert(convKSKIt !=
               solution.circuit_keys.conversion_keyswitch_keys.cend() &&
           "Required conversion key must be available");

    return *convKSKIt;
  }

  // Looks up the secret key of type `kind` for an instruction tagged
  // with the optimizer id `oid`
  const ::concrete_optimizer::dag::SecretLweKey &
  lookupSecretKey(int64_t oid, SolutionKeyKind kind) const {
    uint64_t keyID;

    switch (kind) {
    case SolutionKeyKind::OPERAND:
      keyID = lookupInstructionKeys(oid).input_key;
      return solution.circuit_keys.secret_keys[keyID];
    case SolutionKeyKind::RESULT:
      keyID = lookupInstructionKeys(oid).output_key;
      return solution.circuit_keys.secret_keys[keyID];
    case SolutionKeyKind::KSK_IN:
      return lookupKeyswitchKey(oid).input_key;
    case SolutionKeyKind::KSK_OUT:
      return lookupKeyswitchKey(oid).output_key;
    case SolutionKeyKind::CKSK_IN:
      return lookupConversionKeyswitchKey(oid).input_key;
    case SolutionKeyKind::CKSK_OUT:
      return lookupConversionKeyswitchKey(oid).output_key;
    case SolutionKeyKind::BSK_IN:
      return lookupBootstrapKey(oid).input_key;
    case SolutionKeyKind::BSK_OUT:
      return lookupBootstrapKey(oid).output_key;
    }

    llvm_unreachable("Unknown key kind");
  }

  TFHE::GLWECipherTextType
  getTFHETypeForKey(mlir::MLIRContext *ctx,
                    const ::concrete_optimizer::dag::SecretLweKey &key) const {
    return TFHE::GLWECipherTextType::get(ctx, toGLWESecretKey(key));
  }

protected:
  const ::concrete_optimizer::dag::CircuitSolution &solution;
};

// Type resolver for the type inference for values with unparametrized
// `tfhe.glwe` types
class TFHEParametrizationTypeResolver : public TypeResolver {
public:
  TFHEParametrizationTypeResolver(
      std::optional<CircuitSolutionWrapper> solution)
      : solution(solution) {}

  LocalInferenceState
  resolve(mlir::Operation *op,
          const LocalInferenceState &inferredTypes) override {
    LocalInferenceState state = inferredTypes;

    mlir::TypeSwitch<mlir::Operation *>(op)
        .Case<mlir::func::FuncOp>([&](auto op) {
          TypeConstraintSet<> cs;

          if (solution.has_value()) {
            cs.addConstraint<ApplySolverSolutionToFunctionArgsConstraint>(
                *this, solution.value());
          }

          cs.addConstraint<NoTypeConstraint>();

          cs.converge(op, *this, state, inferredTypes);
        })

        .Case<TFHE::ZeroGLWEOp, TFHE::ZeroTensorGLWEOp,
              mlir::bufferization::AllocTensorOp, TFHE::KeySwitchGLWEOp,
              TFHE::BootstrapGLWEOp, TFHE::BatchedKeySwitchGLWEOp,
              TFHE::BatchedBootstrapGLWEOp, TFHE::EncodeExpandLutForBootstrapOp,
              TFHE::EncodeLutForCrtWopPBSOp, TFHE::EncodePlaintextWithCrtOp,
              TFHE::WopPBSGLWEOp, mlir::func::ReturnOp,
              Tracing::TraceCiphertextOp, mlir::tensor::EmptyOp>([&](auto op) {
          converge<NoTypeConstraint>(op, state, inferredTypes);
        })

        .Case<TFHE::AddGLWEOp, TFHE::ABatchedAddGLWEOp>([&](auto op) {
          converge<SameOperandTypeConstraint<0, 1>,
                   SameOperandAndResultTypeConstraint<0, 0>>(op, state,
                                                             inferredTypes);
        })
        .Case<TFHE::BatchedNegGLWEOp, TFHE::NegGLWEOp, TFHE::AddGLWEIntOp,
              TFHE::BatchedMulGLWEIntOp, TFHE::BatchedMulGLWEIntCstOp,
              TFHE::MulGLWEIntOp, TFHE::ABatchedAddGLWEIntOp,
              TFHE::ABatchedAddGLWEIntCstOp>([&](auto op) {
          converge<SameOperandAndResultTypeConstraint<0, 0>>(op, state,
                                                             inferredTypes);
        })
        .Case<TFHE::SubGLWEIntOp>([&](auto op) {
          converge<SameOperandAndResultTypeConstraint<1, 0>>(op, state,
                                                             inferredTypes);
        })
        .Case<TFHE::BatchedMulGLWECstIntOp, mlir::tensor::ExpandShapeOp>(
            [&](auto op) {
              converge<SameOperandAndResultElementTypeConstraint<0, 0>>(
                  op, state, inferredTypes);
            })

        .Case<mlir::tensor::FromElementsOp>([&](auto op) {
          TypeConstraintSet<> cs;

          if (solution.has_value()) {
            cs.addConstraint<ApplySolverSolutionConstraint>(*this,
                                                            solution.value());
          }

          // TODO: This can be quite slow for `tensor.from_elements`
          // with lots of operands; implement
          // SameOperandTypeConstraint taking into account all
          // operands at once.
          for (size_t i = 1; i < op.getNumOperands(); i++) {
            cs.addConstraint<
                DynamicSameTypeConstraint<DynamicOperandValueYield>>(0, i);
          }

          cs.addConstraint<SameOperandAndResultElementTypeConstraint<0, 0>>();
          cs.converge(op, *this, state, inferredTypes);
        })

        .Case<mlir::tensor::InsertOp, mlir::tensor::InsertSliceOp>(
            [&](auto op) {
              converge<SameOperandElementTypeConstraint<0, 1>,
                       SameOperandAndResultTypeConstraint<1, 0>>(op, state,
                                                                 inferredTypes);
            })
        .Case<mlir::tensor::ExtractOp, mlir::tensor::ExtractSliceOp,
              mlir::tensor::CollapseShapeOp>([&](auto op) {
          converge<SameOperandAndResultElementTypeConstraint<0, 0>>(
              op, state, inferredTypes);
        })
        .Case<mlir::scf::ForOp>([&](mlir::scf::ForOp op) {
          TypeConstraintSet<> cs;

          if (solution.has_value()) {
            cs.addConstraint<ApplySolverSolutionConstraint>(*this,
                                                            solution.value());
          }

          for (size_t i = 0; i < op.getNumIterOperands(); i++) {
            mlir::Value initArg = op.getInitArgs()[i];
            mlir::Value regionIterArg = op.getRegionIterArg(i);
            mlir::Value result = op.getResult(i);
            mlir::Value terminatorOperand =
                op.getBody()->getTerminator()->getOperand(i);

            // Ensure that init args, return values, region iter args and
            // operands of terminator all have the same type
            cs.addConstraint<DynamicSameTypeConstraint<DynamicFunctorYield>>(
                [=]() { return initArg; }, [=]() { return regionIterArg; });

            cs.addConstraint<DynamicSameTypeConstraint<DynamicFunctorYield>>(
                [=]() { return initArg; }, [=]() { return result; });

            cs.addConstraint<DynamicSameTypeConstraint<DynamicFunctorYield>>(
                [=]() { return result; }, [=]() { return terminatorOperand; });
          }

          cs.converge(op, *this, state, inferredTypes);
        })

        .Case<mlir::scf::YieldOp>([&](auto op) {
          TypeConstraintSet<> cs;

          if (solution.has_value()) {
            cs.addConstraint<ApplySolverSolutionConstraint>(*this,
                                                            solution.value());
          }

          for (size_t i = 0; i < op.getNumOperands(); i++) {
            cs.addConstraint<DynamicSameTypeConstraint<DynamicFunctorYield>>(
                [=]() { return op->getParentOp()->getResult(i); },
                [=]() { return op->getOperand(i); });
          }

          cs.converge(op, *this, state, inferredTypes);
        })
        .Default([&](auto _op) { assert(false && "unsupported op type"); });

    return state;
  }

  bool isUnresolvedType(mlir::Type t) const override {
    return isUnparametrizedGLWEType(t);
  }

protected:
  // Type constraint that applies the type assigned to the operation
  // by a TFHE solver via the `TFHE.OId` attribute
  class ApplySolverSolutionConstraint : public TypeConstraint {
  public:
    ApplySolverSolutionConstraint(const TypeResolver &typeResolver,
                                  const CircuitSolutionWrapper &solution)
        : solution(solution), typeResolver(typeResolver) {}

    void apply(mlir::Operation *op, TypeResolver &resolver,
               LocalInferenceState &currState,
               const LocalInferenceState &prevState) override {
      mlir::IntegerAttr oid = op->getAttrOfType<mlir::IntegerAttr>("TFHE.OId");

      if (!oid)
        return;

      mlir::TypeSwitch<mlir::Operation *>(op)
          .Case<TFHE::KeySwitchGLWEOp, TFHE::BatchedKeySwitchGLWEOp>(
              [&](auto op) {
                applyKeyswitch(op, resolver, currState, prevState,
                               oid.getInt());
              })
          .Case<TFHE::BootstrapGLWEOp, TFHE::BatchedBootstrapGLWEOp>(
              [&](auto op) {
                applyBootstrap(op, resolver, currState, prevState,
                               oid.getInt());
              })
          .Default([&](auto op) {
            applyGeneric(op, resolver, currState, prevState, oid.getInt());
          });
    }

  protected:
    // For any value in `values`, set the scalar or element type to
    // `t` if the value is of an unresolved type or of a tensor type
    // with an unresolved element type
    void setUnresolvedTo(mlir::ValueRange values, mlir::Type t,
                         TypeResolver &resolver,
                         LocalInferenceState &currState) {
      for (mlir::Value v : values) {
        if (typeResolver.isUnresolvedType(v.getType())) {
          currState.set(v,
                        TypeInferenceUtils::applyElementType(t, v.getType()));
        }
      }
    }

    // Apply the rule to a keyswitch or batched keyswitch operation
    void applyKeyswitch(mlir::Operation *op, TypeResolver &resolver,
                        LocalInferenceState &currState,
                        const LocalInferenceState &prevState, int64_t oid) {
      // Operands
      TFHE::GLWECipherTextType scalarOperandType = solution.getTFHETypeForKey(
          op->getContext(),
          solution.lookupSecretKey(
              oid, CircuitSolutionWrapper::SolutionKeyKind::KSK_IN));
      setUnresolvedTo(op->getOperands(), scalarOperandType, resolver,
                      currState);

      // Results
      TFHE::GLWECipherTextType scalarResultType = solution.getTFHETypeForKey(
          op->getContext(),
          solution.lookupSecretKey(
              oid, CircuitSolutionWrapper::SolutionKeyKind::KSK_OUT));
      setUnresolvedTo(op->getResults(), scalarResultType, resolver, currState);
    }

    // Apply the rule to a bootstrap or batched bootstrap operation
    void applyBootstrap(mlir::Operation *op, TypeResolver &resolver,
                        LocalInferenceState &currState,
                        const LocalInferenceState &prevState, int64_t oid) {
      // Operands
      TFHE::GLWECipherTextType scalarOperandType = solution.getTFHETypeForKey(
          op->getContext(),
          solution.lookupSecretKey(
              oid, CircuitSolutionWrapper::SolutionKeyKind::BSK_IN));
      setUnresolvedTo(op->getOperands(), scalarOperandType, resolver,
                      currState);

      // Results
      TFHE::GLWECipherTextType scalarResultType = solution.getTFHETypeForKey(
          op->getContext(),
          solution.lookupSecretKey(
              oid, CircuitSolutionWrapper::SolutionKeyKind::BSK_OUT));
      setUnresolvedTo(op->getResults(), scalarResultType, resolver, currState);
    }

    // Apply the rule to any operation that is neither a keyswitch,
    // nor a bootstrap operation
    void applyGeneric(mlir::Operation *op, TypeResolver &resolver,
                      LocalInferenceState &currState,
                      const LocalInferenceState &prevState, int64_t oid) {
      // Operands
      TFHE::GLWECipherTextType scalarOperandType = solution.getTFHETypeForKey(
          op->getContext(),
          solution.lookupSecretKey(
              oid, CircuitSolutionWrapper::SolutionKeyKind::OPERAND));
      setUnresolvedTo(op->getOperands(), scalarOperandType, resolver,
                      currState);

      // Results
      TFHE::GLWECipherTextType scalarResultType = solution.getTFHETypeForKey(
          op->getContext(),
          solution.lookupSecretKey(
              oid, CircuitSolutionWrapper::SolutionKeyKind::RESULT));
      setUnresolvedTo(op->getResults(), scalarResultType, resolver, currState);
    }

  protected:
    const CircuitSolutionWrapper &solution;
    const TypeResolver &typeResolver;
  };

  // Type constraint that applies the type assigned to the arguments
  // of a function by a TFHE solver via the `TFHE.OId` attributes of
  // the function arguments
  class ApplySolverSolutionToFunctionArgsConstraint : public TypeConstraint {
  public:
    ApplySolverSolutionToFunctionArgsConstraint(
        const TypeResolver &typeResolver,
        const CircuitSolutionWrapper &solution)
        : solution(solution), typeResolver(typeResolver) {}

    void apply(mlir::Operation *op, TypeResolver &resolver,
               LocalInferenceState &currState,
               const LocalInferenceState &prevState) override {
      mlir::func::FuncOp func = llvm::cast<mlir::func::FuncOp>(op);

      for (size_t i = 0; i < func.getNumArguments(); i++) {
        mlir::BlockArgument arg = func.getArgument(i);
        if (mlir::IntegerAttr oidAttr =
                func.getArgAttrOfType<mlir::IntegerAttr>(i, "TFHE.OId")) {
          TFHE::GLWECipherTextType scalarOperandType =
              solution.getTFHETypeForKey(
                  func->getContext(),
                  solution.lookupSecretKey(
                      oidAttr.getInt(),
                      CircuitSolutionWrapper::SolutionKeyKind::RESULT));

          currState.set(arg, TypeInferenceUtils::applyElementType(
                                 scalarOperandType, arg.getType()));
        }
      }
    }

  protected:
    const CircuitSolutionWrapper &solution;
    const TypeResolver &typeResolver;
  };

  // Instantiates the constraint types `ConstraintTs`, adds a solver
  // solution constraint as the first constraint and converges on all
  // constraints
  template <typename... ConstraintTs>
  void converge(mlir::Operation *op, LocalInferenceState &state,
                const LocalInferenceState &inferredTypes) {
    TypeConstraintSet<> cs;

    if (solution.has_value())
      cs.addConstraint<ApplySolverSolutionConstraint>(*this, solution.value());

    cs.addConstraints<ConstraintTs...>();
    cs.converge(op, *this, state, inferredTypes);
  }

  // Return `true` iff `t` is GLWE type that is not parameterized,
  // otherwise `false`
  static bool isUnparametrizedGLWEType(mlir::Type t) {
    std::optional<TFHE::GLWECipherTextType> ctt =
        tryGetScalarType<TFHE::GLWECipherTextType>(t);

    return ctt.has_value() && ctt.value().getKey().isNone();
  }

  std::optional<CircuitSolutionWrapper> solution;
};

// TFHE-specific rewriter that handles conflicts of contradicting TFHE
// types through the introduction of `tfhe.keyswitch` /
// `tfhe.batched_keyswitch` operations and that removes `TFHE.OId`
// attributes after the rewrite.
class TFHECircuitSolutionRewriter : public TypeInferenceRewriter {
public:
  TFHECircuitSolutionRewriter(
      const mlir::DataFlowSolver &solver,
      TFHEParametrizationTypeResolver &typeResolver,
      const std::optional<CircuitSolutionWrapper> &solution)
      : TypeInferenceRewriter(solver, typeResolver), typeResolver(typeResolver),
        solution(solution) {}

  virtual mlir::LogicalResult postRewriteHook(mlir::IRRewriter &rewriter,
                                              mlir::Operation *oldOp,
                                              mlir::Operation *newOp) override {
    mlir::IntegerAttr oid = newOp->getAttrOfType<mlir::IntegerAttr>("TFHE.OId");

    if (oid) {
      newOp->removeAttr("TFHE.OId");

      if (solution.has_value()) {
        // Fixup key attributes
        if (TFHE::GLWEKeyswitchKeyAttr attrKeyswitchKey =
                newOp->getAttrOfType<TFHE::GLWEKeyswitchKeyAttr>("key")) {
          newOp->setAttr("key", solution->getKeyswitchKeyAttr(
                                    newOp->getContext(), oid.getInt()));
        } else if (TFHE::GLWEBootstrapKeyAttr attrBootstrapKey =
                       newOp->getAttrOfType<TFHE::GLWEBootstrapKeyAttr>(
                           "key")) {
          newOp->setAttr("key", solution->getBootstrapKeyAttr(
                                    newOp->getContext(), oid.getInt()));
        }
      }
    }

    // Bootstrap operations that have changed keys may need an
    // adjustment of their lookup tables. This is currently limited to
    // bootstrap operations using static LUTs to implement the rounded
    // PBS operation and to bootstrap operations whose LUTs are
    // encoded within function scope using an encode and expand
    // operation.
    if (TFHE::BootstrapGLWEOp newBSOp =
            llvm::dyn_cast<TFHE::BootstrapGLWEOp>(newOp)) {
      TFHE::BootstrapGLWEOp oldBSOp = llvm::cast<TFHE::BootstrapGLWEOp>(oldOp);

      if (checkFixupBootstrapLUTs(rewriter, oldBSOp, newBSOp).failed())
        return mlir::failure();
    }

    return mlir::success();
  }

  // Resolves conflicts between cipher text scalar or ciphertext
  // tensor types by creating keyswitch / batched keyswitch operations
  mlir::Value handleConflict(mlir::IRRewriter &rewriter,
                             mlir::OpOperand &oldOperand,
                             mlir::Type resolvedType,
                             mlir::Value producerValue) override {
    mlir::Operation *oldOp = oldOperand.getOwner();

    std::optional<TFHE::GLWECipherTextType> cttFrom =
        tryGetScalarType<TFHE::GLWECipherTextType>(producerValue.getType());
    std::optional<TFHE::GLWECipherTextType> cttTo =
        tryGetScalarType<TFHE::GLWECipherTextType>(resolvedType);

    // Only handle conflicts wrt. ciphertext types or tensors of ciphertext
    // types
    if (!cttFrom.has_value() || !cttTo.has_value() ||
        resolvedType.isa<mlir::MemRefType>()) {
      return TypeInferenceRewriter::handleConflict(rewriter, oldOperand,
                                                   resolvedType, producerValue);
    }

    // Place keyswitch operation near the producer of the value to
    // avoid nesting it too depply into loops
    if (mlir::Operation *producer = producerValue.getDefiningOp())
      rewriter.setInsertionPointAfter(producer);

    assert(cttFrom->getKey().getParameterized().has_value());
    assert(cttTo->getKey().getParameterized().has_value());

    const ::concrete_optimizer::dag::ConversionKeySwitchKey &cksk =
        solution->lookupConversionKeyswitchKey(
            cttFrom->getKey().getParameterized()->identifier,
            cttTo->getKey().getParameterized()->identifier);

    TFHE::GLWEKeyswitchKeyAttr kskAttr =
        solution->getKeyswitchKeyAttr(rewriter.getContext(), cksk);

    // For tensor types, conversion must be done using a batched
    // keyswitch operation, otherwise a simple keyswitch op is
    // sufficient
    if (mlir::RankedTensorType rtt =
            resolvedType.dyn_cast<mlir::RankedTensorType>()) {
      if (rtt.getShape().size() == 1) {
        // Flat input shapes can be handled directly by a batched
        // keyswitch operation
        return rewriter.create<TFHE::BatchedKeySwitchGLWEOp>(
            oldOp->getLoc(), resolvedType, producerValue, kskAttr);
      } else {
        // Input shapes with more dimensions must first be flattened
        // using a tensor.collapse_shape operation before passing the
        // values to a batched keyswitch operation

        mlir::ReassociationIndices reassocDims(rtt.getShape().size());

        for (size_t i = 0; i < rtt.getShape().size(); i++)
          reassocDims[i] = i;

        llvm::SmallVector<mlir::ReassociationIndices> reassocs = {reassocDims};

        // Flatten inputs
        mlir::Value collapsed = rewriter.create<mlir::tensor::CollapseShapeOp>(
            oldOp->getLoc(), producerValue, reassocs);

        mlir::Type collapsedResolvedType = mlir::RankedTensorType::get(
            {rtt.getNumElements()}, rtt.getElementType());

        TFHE::BatchedKeySwitchGLWEOp ksOp =
            rewriter.create<TFHE::BatchedKeySwitchGLWEOp>(
                oldOp->getLoc(), collapsedResolvedType, collapsed, kskAttr);

        // Restore original shape for the result
        return rewriter.create<mlir::tensor::ExpandShapeOp>(
            oldOp->getLoc(), resolvedType, ksOp.getResult(), reassocs);
      }
    } else {
      // Scalar inputs are directly handled by a simple keyswitch
      // operation
      return rewriter.create<TFHE::KeySwitchGLWEOp>(
          oldOp->getLoc(), resolvedType, producerValue, kskAttr);
    }
  }

protected:
  // Checks if the lookup table for a freshly rewritten bootstrap
  // operation needs to be adjusted and performs the adjustment if
  // this is the case.
  mlir::LogicalResult checkFixupBootstrapLUTs(mlir::IRRewriter &rewriter,
                                              TFHE::BootstrapGLWEOp oldBSOp,
                                              TFHE::BootstrapGLWEOp newBSOp) {
    TFHE::GLWEBootstrapKeyAttr oldBSKeyAttr =
        oldBSOp->getAttrOfType<TFHE::GLWEBootstrapKeyAttr>("key");

    assert(oldBSKeyAttr);

    mlir::Value lut = newBSOp.getLookupTable();
    mlir::RankedTensorType lutType =
        lut.getType().cast<mlir::RankedTensorType>();

    assert(lutType.getShape().size() == 1);

    if (lutType.getShape()[0] == oldBSKeyAttr.getPolySize()) {
      // Parametrization has no effect on LUT
      return mlir::success();
    }

    mlir::Operation *lutOp = lut.getDefiningOp();

    TFHE::GLWEBootstrapKeyAttr newBSKeyAttr =
        newBSOp->getAttrOfType<TFHE::GLWEBootstrapKeyAttr>("key");

    assert(newBSKeyAttr);

    mlir::RankedTensorType newLUTType = mlir::RankedTensorType::get(
        mlir::ArrayRef<int64_t>{newBSKeyAttr.getPolySize()},
        rewriter.getI64Type());

    if (arith::ConstantOp oldCstOp =
            llvm::dyn_cast_or_null<arith::ConstantOp>(lutOp)) {
      // LUT is generated from a constant. Parametrization is only
      // supported if this is a scenario, in which the bootstrap
      // operation is used as a rounded bootstrap with identical
      // entries in the LUT.
      mlir::DenseIntElementsAttr oldCstValsAttr =
          oldCstOp.getValueAttr().dyn_cast<mlir::DenseIntElementsAttr>();

      if (!oldCstValsAttr.isSplat()) {
        oldBSOp->emitError(
            "Bootstrap operation uses a constant LUT, but with different "
            "entries. Only constants with identical elements for the "
            "implementation of a rounded PBS are supported for now");

        return mlir::failure();
      }

      rewriter.setInsertionPointAfter(oldCstOp);
      mlir::arith::ConstantOp newCstOp =
          rewriter.create<mlir::arith::ConstantOp>(
              oldCstOp.getLoc(), newLUTType,
              oldCstValsAttr.resizeSplat(newLUTType));

      newBSOp.setOperand(1, newCstOp);
    } else if (TFHE::EncodeExpandLutForBootstrapOp oldEncodeOp =
                   llvm::dyn_cast_or_null<TFHE::EncodeExpandLutForBootstrapOp>(
                       lutOp)) {
      // For encode and expand operations, simply update the size of
      // the polynomial

      rewriter.setInsertionPointAfter(oldEncodeOp);

      TFHE::EncodeExpandLutForBootstrapOp newEncodeOp =
          rewriter.create<TFHE::EncodeExpandLutForBootstrapOp>(
              oldEncodeOp.getLoc(), newLUTType,
              oldEncodeOp.getInputLookupTable(), newBSKeyAttr.getPolySize(),
              oldEncodeOp.getOutputBits(), oldEncodeOp.getIsSigned());

      newBSOp.setOperand(1, newEncodeOp);
    } else {
      oldBSOp->emitError(
          "Cannot update lookup table after parametrization, only constants "
          "and tables generated through TFHE.encode_expand_lut_for_bootstrap "
          "are supported");

      return mlir::failure();
    }

    return mlir::success();
  }

  TFHEParametrizationTypeResolver &typeResolver;
  const std::optional<CircuitSolutionWrapper> &solution;
};

// Rewrite pattern that materializes the boundary between two
// partitions specified in the solution of the optimizer by an extra
// conversion key for a bootstrap operation.
//
// Replaces the pattern:
//
//   %v = TFHE.bootstrap_glwe(%i0, %i1) : (T0, T1) -> T2
//   ...
//   ... someotherop(..., %v, ...) : (..., T2, ...) -> ...
//
// with:
//
//   %v = TFHE.bootstrap_glwe(%i0, %i1) : (T0, T1) -> T2
//   %v1 = TypeInference.propagate_upward(%v) : T2 -> CT0
//   %v2 = TFHE.keyswitch_glwe(%v1) : CT0 -> CT1
//   %v3 = TypeInference.propagate_downward(%v) : CT1 -> T2
//   ...
//   ... someotherop(..., %v3, ...) : (..., T2, ...) -> ...
//
// The TypeInference operations are necessary to avoid producing
// invalid IR if `T2` is an unparametrized type.
class MaterializePartitionBoundaryPattern
    : public mlir::OpRewritePattern<Optimizer::PartitionFrontierOp> {
public:
  MaterializePartitionBoundaryPattern(mlir::MLIRContext *ctx,
                                      const CircuitSolutionWrapper &solution)
      : mlir::OpRewritePattern<Optimizer::PartitionFrontierOp>(ctx, 0),
        solution(solution) {}

  mlir::LogicalResult
  matchAndRewrite(Optimizer::PartitionFrontierOp pfOp,
                  mlir::PatternRewriter &rewriter) const override {
    const ::concrete_optimizer::dag::ConversionKeySwitchKey &cksk =
        solution.lookupConversionKeyswitchKey(pfOp.getInputKeyID(),
                                              pfOp.getOutputKeyID());

    TFHE::GLWECipherTextType cksInputType =
        solution.getTFHETypeForKey(pfOp->getContext(), cksk.input_key);

    TFHE::GLWECipherTextType cksOutputType =
        solution.getTFHETypeForKey(pfOp->getContext(), cksk.output_key);

    rewriter.setInsertionPointAfter(pfOp);

    TypeInference::PropagateUpwardOp puOp =
        rewriter.create<TypeInference::PropagateUpwardOp>(
            pfOp->getLoc(), cksInputType, pfOp.getInput());

    TFHE::GLWEKeyswitchKeyAttr keyAttr =
        solution.getKeyswitchKeyAttr(rewriter.getContext(), cksk);

    TFHE::KeySwitchGLWEOp ksOp = rewriter.create<TFHE::KeySwitchGLWEOp>(
        pfOp->getLoc(), cksOutputType, puOp.getResult(), keyAttr);

    mlir::Type unparametrizedType = TFHE::GLWECipherTextType::get(
        rewriter.getContext(), TFHE::GLWESecretKey::newNone());

    TypeInference::PropagateDownwardOp pdOp =
        rewriter.create<TypeInference::PropagateDownwardOp>(
            pfOp->getLoc(), unparametrizedType, ksOp.getResult());

    rewriter.replaceAllUsesWith(pfOp.getResult(), pdOp.getResult());

    return mlir::success();
  }

protected:
  const CircuitSolutionWrapper &solution;
};

class TFHECircuitSolutionParametrizationPass
    : public TFHECircuitSolutionParametrizationBase<
          TFHECircuitSolutionParametrizationPass> {
public:
  TFHECircuitSolutionParametrizationPass(
      std::optional<::concrete_optimizer::dag::CircuitSolution> solution)
      : solution(solution){};

  void runOnOperation() override {
    mlir::ModuleOp module = this->getOperation();
    mlir::DataFlowSolver solver;
    std::optional<CircuitSolutionWrapper> solutionWrapper =
        solution.has_value()
            ? std::make_optional<CircuitSolutionWrapper>(solution.value())
            : std::nullopt;

    if (solutionWrapper.has_value()) {
      // Materialize explicit transitions between optimizer partitions
      // by replacing `optimizer.partition_frontier` operations with
      // keyswitch operations in order to keep type inference and the
      // subsequent rewriting simple.
      mlir::RewritePatternSet patterns(module->getContext());
      patterns.add<MaterializePartitionBoundaryPattern>(
          module->getContext(), solutionWrapper.value());

      if (mlir::applyPatternsAndFoldGreedily(module, std::move(patterns))
              .failed())
        this->signalPassFailure();
    }

    TFHEParametrizationTypeResolver typeResolver(solutionWrapper);
    mlir::SymbolTableCollection symbolTables;

    solver.load<mlir::dataflow::DeadCodeAnalysis>();
    solver.load<mlir::dataflow::SparseConstantPropagation>();
    solver.load<ForwardTypeInferenceAnalysis>(typeResolver);
    solver.load<BackwardTypeInferenceAnalysis>(symbolTables, typeResolver);

    if (failed(solver.initializeAndRun(module)))
      return signalPassFailure();

    TFHECircuitSolutionRewriter tir(solver, typeResolver, solutionWrapper);

    if (tir.rewrite(module).failed())
      signalPassFailure();
  }

private:
  std::optional<::concrete_optimizer::dag::CircuitSolution> solution;
};

} // end anonymous namespace

std::unique_ptr<mlir::OperationPass<mlir::ModuleOp>>
createTFHECircuitSolutionParametrizationPass(
    std::optional<::concrete_optimizer::dag::CircuitSolution> solution) {
  return std::make_unique<TFHECircuitSolutionParametrizationPass>(solution);
}

} // namespace concretelang
} // namespace mlir
