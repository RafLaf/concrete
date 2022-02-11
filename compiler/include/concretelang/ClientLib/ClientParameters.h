// Part of the Concrete Compiler Project, under the BSD3 License with Zama
// Exceptions. See
// https://github.com/zama-ai/concrete-compiler-internal/blob/master/LICENSE.txt
// for license information.

#ifndef CONCRETELANG_CLIENTLIB_CLIENTPARAMETERS_H_
#define CONCRETELANG_CLIENTLIB_CLIENTPARAMETERS_H_
#include <map>
#include <string>
#include <vector>

#include <llvm/Support/JSON.h>

namespace mlir {
namespace concretelang {

const std::string SMALL_KEY = "small";
const std::string BIG_KEY = "big";

typedef size_t DecompositionLevelCount;
typedef size_t DecompositionBaseLog;
typedef size_t PolynomialSize;
typedef size_t Precision;
typedef double Variance;

typedef uint64_t LweDimension;
typedef uint64_t GlweDimension;

typedef std::string LweSecretKeyID;
struct LweSecretKeyParam {
  LweDimension size;

  void hash(size_t &seed);
};
static bool operator==(const LweSecretKeyParam &lhs,
                       const LweSecretKeyParam &rhs) {
  return lhs.size == rhs.size;
}

typedef std::string BootstrapKeyID;
struct BootstrapKeyParam {
  LweSecretKeyID inputSecretKeyID;
  LweSecretKeyID outputSecretKeyID;
  DecompositionLevelCount level;
  DecompositionBaseLog baseLog;
  GlweDimension glweDimension;
  Variance variance;

  void hash(size_t &seed);
};
static inline bool operator==(const BootstrapKeyParam &lhs,
                              const BootstrapKeyParam &rhs) {
  return lhs.inputSecretKeyID == rhs.inputSecretKeyID &&
         lhs.outputSecretKeyID == rhs.outputSecretKeyID &&
         lhs.level == rhs.level && lhs.baseLog == rhs.baseLog &&
         lhs.glweDimension == rhs.glweDimension && lhs.variance == rhs.variance;
}

typedef std::string KeyswitchKeyID;
struct KeyswitchKeyParam {
  LweSecretKeyID inputSecretKeyID;
  LweSecretKeyID outputSecretKeyID;
  DecompositionLevelCount level;
  DecompositionBaseLog baseLog;
  Variance variance;

  void hash(size_t &seed);
};
static inline bool operator==(const KeyswitchKeyParam &lhs,
                              const KeyswitchKeyParam &rhs) {
  return lhs.inputSecretKeyID == rhs.inputSecretKeyID &&
         lhs.outputSecretKeyID == rhs.outputSecretKeyID &&
         lhs.level == rhs.level && lhs.baseLog == rhs.baseLog &&
         lhs.variance == rhs.variance;
}

struct Encoding {
  Precision precision;
};
static inline bool operator==(const Encoding &lhs, const Encoding &rhs) {
  return lhs.precision == rhs.precision;
}

struct EncryptionGate {
  LweSecretKeyID secretKeyID;
  Variance variance;
  Encoding encoding;
};
static inline bool operator==(const EncryptionGate &lhs,
                              const EncryptionGate &rhs) {
  return lhs.secretKeyID == rhs.secretKeyID && lhs.variance == rhs.variance &&
         lhs.encoding == rhs.encoding;
}

struct CircuitGateShape {
  // Width of the scalar value
  size_t width;
  // Dimensions of the tensor, empty if scalar
  std::vector<int64_t> dimensions;
  // Size of the buffer containing the tensor
  size_t size;
};
static inline bool operator==(const CircuitGateShape &lhs,
                              const CircuitGateShape &rhs) {
  return lhs.width == rhs.width && lhs.dimensions == rhs.dimensions &&
         lhs.size == rhs.size;
}

struct CircuitGate {
  llvm::Optional<EncryptionGate> encryption;
  CircuitGateShape shape;
};
static inline bool operator==(const CircuitGate &lhs, const CircuitGate &rhs) {
  return lhs.encryption == rhs.encryption && lhs.shape == rhs.shape;
}

struct ClientParameters {
  std::map<LweSecretKeyID, LweSecretKeyParam> secretKeys;
  std::map<BootstrapKeyID, BootstrapKeyParam> bootstrapKeys;
  std::map<KeyswitchKeyID, KeyswitchKeyParam> keyswitchKeys;
  std::vector<CircuitGate> inputs;
  std::vector<CircuitGate> outputs;
  std::string functionName;
  size_t hash();
};
static inline bool operator==(const ClientParameters &lhs,
                              const ClientParameters &rhs) {
  return lhs.secretKeys == rhs.secretKeys &&
         lhs.bootstrapKeys == rhs.bootstrapKeys &&
         lhs.keyswitchKeys == rhs.keyswitchKeys && lhs.inputs == lhs.inputs &&
         lhs.outputs == lhs.outputs;
}

llvm::json::Value toJSON(const LweSecretKeyParam &);
bool fromJSON(const llvm::json::Value, LweSecretKeyParam &, llvm::json::Path);

llvm::json::Value toJSON(const BootstrapKeyParam &);
bool fromJSON(const llvm::json::Value, BootstrapKeyParam &, llvm::json::Path);

llvm::json::Value toJSON(const KeyswitchKeyParam &);
bool fromJSON(const llvm::json::Value, KeyswitchKeyParam &, llvm::json::Path);

llvm::json::Value toJSON(const Encoding &);
bool fromJSON(const llvm::json::Value, Encoding &, llvm::json::Path);

llvm::json::Value toJSON(const EncryptionGate &);
bool fromJSON(const llvm::json::Value, EncryptionGate &, llvm::json::Path);

llvm::json::Value toJSON(const CircuitGateShape &);
bool fromJSON(const llvm::json::Value, CircuitGateShape &, llvm::json::Path);

llvm::json::Value toJSON(const CircuitGate &);
bool fromJSON(const llvm::json::Value, CircuitGate &, llvm::json::Path);

llvm::json::Value toJSON(const ClientParameters &);
bool fromJSON(const llvm::json::Value, ClientParameters &, llvm::json::Path);
static inline llvm::raw_ostream &operator<<(llvm::raw_ostream &OS,
                                            ClientParameters cp) {
  return OS << llvm::formatv("{0:2}", toJSON(cp));
}

} // namespace concretelang
} // namespace mlir

#endif