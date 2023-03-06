// Part of the Concrete Compiler Project, under the BSD3 License with Zama
// Exceptions. See
// https://github.com/zama-ai/concrete-compiler-internal/blob/main/LICENSE.txt
// for license information.

#include <concretelang/Dialect/TFHE/IR/TFHETypes.h>

namespace mlir {
namespace concretelang {
namespace TFHE {

void printSigned(mlir::AsmPrinter &p, signed i) {
  if (i == -1)
    p << "_";
  else
    p << i;
}

void GLWECipherTextType::print(mlir::AsmPrinter &p) const {
  p << "<";
  p << "{";
  printSigned(p, getDimension());
  p << ",";
  printSigned(p, getPolynomialSize());
  p << ",";
  printSigned(p, getBits());
  p << "}";

  p << "{";
  printSigned(p, getP());
  p << "}>";
}

mlir::Type GLWECipherTextType::parse(AsmParser &parser) {
  if (parser.parseLess())
    return mlir::Type();

  if (parser.parseLBrace())
    return mlir::Type();

  // First parameters block
  int dimension = -1;
  if (parser.parseOptionalKeyword("_") && parser.parseInteger(dimension))
    return mlir::Type();
  if (parser.parseComma())
    return mlir::Type();
  int polynomialSize = -1;
  if (parser.parseOptionalKeyword("_") && parser.parseInteger(polynomialSize))
    return mlir::Type();
  if (parser.parseComma())
    return mlir::Type();
  int bits = -1;
  if (parser.parseOptionalKeyword("_") && parser.parseInteger(bits))
    return mlir::Type();
  if (parser.parseRBrace())
    return mlir::Type();

  // Next parameters block
  if (parser.parseLBrace())
    return mlir::Type();
  int p = -1;
  if (parser.parseInteger(p))
    return mlir::Type();
  if (parser.parseRBrace())
    return mlir::Type();

  if (parser.parseGreater())
    return mlir::Type();
  Location loc = parser.getEncodedSourceLoc(parser.getNameLoc());
  return getChecked(loc, loc.getContext(), dimension, polynomialSize, bits, p);
}
} // namespace TFHE
} // namespace concretelang
} // namespace mlir