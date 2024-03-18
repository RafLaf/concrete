// Part of the Concrete Compiler Project, under the BSD3 License with Zama
// Exceptions. See
// https://github.com/zama-ai/concrete/blob/main/LICENSE.txt
// for license information.

#ifndef CONCRETELANG_DIALECT_SDFG_SDFGCONVERTIBLEOPINTERFACEIMPL_H
#define CONCRETELANG_DIALECT_SDFG_SDFGCONVERTIBLEOPINTERFACEIMPL_H

namespace mlir {
class DialectRegistry;

namespace concretelang {
namespace SDFG {
void registerSDFGConvertibleOpInterfaceExternalModels(
    DialectRegistry &registry);
} // namespace SDFG
} // namespace concretelang
} // namespace mlir

#endif
