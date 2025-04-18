; NOTE: Assertions have been autogenerated by utils/update_test_checks.py UTC_ARGS: --version 2
; RUN: opt -S -O3 -memssa-check-limit=1 -memdep-block-scan-limit=1 < %s | FileCheck %s

define i32 @f(ptr noalias %p, i32 %c) {
; CHECK-LABEL: define noundef i32 @f
; CHECK-SAME: (ptr noalias readonly captures(none) [[P:%.*]], i32 [[C:%.*]]) local_unnamed_addr {
; CHECK-NEXT:    tail call void @g()
; CHECK-NEXT:    tail call void @g()
; CHECK-NEXT:    tail call void @g()
; CHECK-NEXT:    tail call void @g()
; CHECK-NEXT:    tail call void @g()
; CHECK-NEXT:    tail call void @g()
; CHECK-NEXT:    ret i32 0
;
  %i = load i32, ptr %p
  call void @g()
  call void @g()
  call void @g()
  call void @g()
  call void @g()
  call void @g()
  %i2 = load i32, ptr %p
  %r = sub i32 %i, %i2
  ret i32 %r
}

declare void @g()
