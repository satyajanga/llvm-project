; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; RUN: opt -S -passes=instcombine < %s | FileCheck %s

@contant_int_array = private unnamed_addr constant [10 x i64] [i64 0, i64 1, i64 2, i64 3, i64 4, i64 5, i64 6, i64 7, i64 8, i64 9]

; InstCombine should be able to optimize out the alloca and memcpy:
define void @combine_masked_load_store_from_constant_array(ptr %ptr) {
; CHECK-LABEL: @combine_masked_load_store_from_constant_array(
; CHECK-NEXT:    [[TMP1:%.*]] = tail call <vscale x 2 x i1> @llvm.aarch64.sve.whilelt.nxv2i1.i32(i32 0, i32 10)
; CHECK-NEXT:    [[TMP2:%.*]] = call <vscale x 2 x i64> @llvm.masked.load.nxv2i64.p0(ptr nonnull @contant_int_array, i32 8, <vscale x 2 x i1> [[TMP1]], <vscale x 2 x i64> zeroinitializer)
; CHECK-NEXT:    call void @llvm.masked.store.nxv2i64.p0(<vscale x 2 x i64> [[TMP2]], ptr [[PTR:%.*]], i32 1, <vscale x 2 x i1> [[TMP1]])
; CHECK-NEXT:    ret void
;
  %1 = alloca [10 x i64]
  call void @llvm.memcpy.p0.p0.i64(ptr %1, ptr @contant_int_array, i64 80, i1 false)
  %2 = tail call <vscale x 2 x i1> @llvm.aarch64.sve.whilelt.nxv2i1.i32(i32 0, i32 10)
  %3 = call <vscale x 2 x i64> @llvm.masked.load.nxv2i64.p0(ptr nonnull %1, i32 8, <vscale x 2 x i1> %2, <vscale x 2 x i64> zeroinitializer)
  call void @llvm.masked.store.nxv2i64.p0(<vscale x 2 x i64> %3, ptr %ptr, i32 1, <vscale x 2 x i1> %2)
  ret void
}

define void @combine_masked_expandload_compressstore_from_constant_array(ptr %ptr) {
; CHECK-LABEL: @combine_masked_expandload_compressstore_from_constant_array(
; CHECK-NEXT:    [[TMP1:%.*]] = call <10 x i64> @llvm.masked.expandload.v10i64(ptr nonnull @contant_int_array, <10 x i1> splat (i1 true), <10 x i64> zeroinitializer)
; CHECK-NEXT:    call void @llvm.masked.compressstore.v10i64(<10 x i64> [[TMP1]], ptr [[PTR:%.*]], <10 x i1> splat (i1 true))
; CHECK-NEXT:    ret void
;
  %1 = alloca [10 x i64]
  call void @llvm.memcpy.p0.p0.i64(ptr %1, ptr @contant_int_array, i64 80, i1 false)
  %2 = call <10 x i64> @llvm.masked.expandload.v10i64(ptr nonnull %1, <10 x i1> <i1 1, i1 1, i1 1, i1 1, i1 1, i1 1, i1 1, i1 1, i1 1, i1 1>, <10 x i64> zeroinitializer)
  call void @llvm.masked.compressstore.nxv10i64.p0(<10 x i64> %2, ptr %ptr, <10 x i1> <i1 1, i1 1, i1 1, i1 1, i1 1, i1 1, i1 1, i1 1, i1 1, i1 1>)
  ret void
}

declare void @llvm.memcpy.p0.p0.i64(ptr, ptr, i64, i1)
declare <vscale x 2 x i64> @llvm.masked.load.nxv2i64.p0(ptr, i32, <vscale x 2 x i1>, <vscale x 2 x i64>)
declare void @llvm.masked.store.nxv2i64.p0(<vscale x 2 x i64>, ptr, i32, <vscale x 2 x i1>)
declare <vscale x 2 x i1> @llvm.aarch64.sve.whilelt.nxv2i1.i32(i32, i32)
declare <10 x i64> @llvm.masked.expandload.v10i64(ptr, <10 x i1>,  <10 x i64>)
declare void @llvm.masked.compressstore.nxv10i64.p0(<10 x i64>, ptr, <10 x i1>)
