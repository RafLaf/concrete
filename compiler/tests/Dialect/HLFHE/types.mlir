// RUN: zamacompiler %s  2>&1| FileCheck %s

// CHECK-LABEL: func @memref_arg(%arg0: memref<2x!HLFHE.eint<0>>
func @memref_arg(%arg0: memref<2x!HLFHE.eint<0>>) {
  return
}
