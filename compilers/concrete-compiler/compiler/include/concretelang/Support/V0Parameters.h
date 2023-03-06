// Part of the Concrete Compiler Project, under the BSD3 License with Zama
// Exceptions. See
// https://github.com/zama-ai/concrete-compiler-internal/blob/main/LICENSE.txt
// for license information.

#ifndef CONCRETELANG_SUPPORT_V0Parameter_H_
#define CONCRETELANG_SUPPORT_V0Parameter_H_

#include "llvm/ADT/Optional.h"

#include "concrete-optimizer.hpp"
#include "concretelang/Conversion/Utils/GlobalFHEContext.h"
#include "concretelang/Support/CompilationFeedback.h"

namespace mlir {
namespace concretelang {

namespace optimizer {
constexpr double DEFAULT_GLOBAL_P_ERROR = 1.0 / 100000.0;
constexpr double UNSPECIFIED_P_ERROR = NAN; // will use DEFAULT_GLOBAL_P_ERROR
constexpr double UNSPECIFIED_GLOBAL_P_ERROR =
    NAN; // will use DEFAULT_GLOBAL_P_ERROR
constexpr uint DEFAULT_SECURITY = 128;
constexpr uint DEFAULT_FALLBACK_LOG_NORM_WOPPBS = 8;
constexpr bool DEFAULT_DISPLAY = false;
constexpr bool DEFAULT_STRATEGY_V0 = false;
constexpr bool DEFAULT_USE_GPU_CONSTRAINTS = false;
constexpr concrete_optimizer::Encoding DEFAULT_ENCODING =
    concrete_optimizer::Encoding::Auto;
constexpr bool DEFAULT_CACHE_ON_DISK = true;

struct Config {
  double p_error;
  double global_p_error;
  bool display;
  bool strategy_v0;
  std::uint64_t security;
  double fallback_log_norm_woppbs;
  bool use_gpu_constraints;
  concrete_optimizer::Encoding encoding;
  bool cache_on_disk;
};

constexpr Config DEFAULT_CONFIG = {
    UNSPECIFIED_P_ERROR,
    UNSPECIFIED_GLOBAL_P_ERROR,
    DEFAULT_DISPLAY,
    DEFAULT_STRATEGY_V0,
    DEFAULT_SECURITY,
    DEFAULT_FALLBACK_LOG_NORM_WOPPBS,
    DEFAULT_USE_GPU_CONSTRAINTS,
    DEFAULT_ENCODING,
    DEFAULT_CACHE_ON_DISK,
};

using Dag = rust::Box<concrete_optimizer::OperationDag>;
using Solution = concrete_optimizer::v0::Solution;
using DagSolution = concrete_optimizer::dag::DagSolution;

/* Contains any circuit description usable by the concrete-optimizer */
struct Description {
  V0FHEConstraint constraint;
  llvm::Optional<optimizer::Dag> dag;
};

} // namespace optimizer

llvm::Expected<V0Parameter> getParameter(optimizer::Description &descr,
                                         CompilationFeedback &feedback,
                                         optimizer::Config optimizerConfig);
} // namespace concretelang
} // namespace mlir
#endif