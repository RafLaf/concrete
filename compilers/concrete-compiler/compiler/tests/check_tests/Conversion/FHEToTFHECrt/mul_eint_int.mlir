// RUN: concretecompiler --optimize-tfhe=false --action=dump-tfhe %s --large-integer-crt-decomposition=2,3,5,7,11 --large-integer-circuit-bootstrap=2,9 --large-integer-packing-keyswitch=694,1024,4,9 --v0-parameter=2,10,693,4,9,7,2 2>&1| FileCheck %s

// CHECK-LABEL:  func.func @mul_eint_int(%arg0: tensor<5x!TFHE.glwe<{_,_,_}{7}>>) -> tensor<5x!TFHE.glwe<{_,_,_}{7}>> {
// CHECK-NEXT:    %c2_i8 = arith.constant 2 : i8
// CHECK-NEXT:    %0 = arith.extsi %c2_i8 : i8 to i64
// CHECK-NEXT:    %1 = bufferization.alloc_tensor() : tensor<5x!TFHE.glwe<{_,_,_}{7}>>
// CHECK-NEXT:    %c0 = arith.constant 0 : index
// CHECK-NEXT:    %c1 = arith.constant 1 : index
// CHECK-NEXT:    %c5 = arith.constant 5 : index
// CHECK-NEXT:    %2 = scf.for %arg1 = %c0 to %c5 step %c1 iter_args(%arg2 = %1) -> (tensor<5x!TFHE.glwe<{_,_,_}{7}>>) {
// CHECK-NEXT:      %3 = tensor.extract %arg0[%arg1] : tensor<5x!TFHE.glwe<{_,_,_}{7}>>
// CHECK-NEXT:      %4 = "TFHE.mul_glwe_int"(%3, %0) : (!TFHE.glwe<{_,_,_}{7}>, i64) -> !TFHE.glwe<{_,_,_}{7}>
// CHECK-NEXT:      %5 = tensor.insert %4 into %arg2[%arg1] : tensor<5x!TFHE.glwe<{_,_,_}{7}>>
// CHECK-NEXT:      scf.yield %5 : tensor<5x!TFHE.glwe<{_,_,_}{7}>>
// CHECK-NEXT:    }
// CHECK-NEXT:    return %2 : tensor<5x!TFHE.glwe<{_,_,_}{7}>>
func.func @mul_eint_int(%arg0: !FHE.eint<7>) -> !FHE.eint<7> {
  %0 = arith.constant 2 : i8
  %1 = "FHE.mul_eint_int"(%arg0, %0): (!FHE.eint<7>, i8) -> (!FHE.eint<7>)
  return %1: !FHE.eint<7>
}