; ModuleID = 'Literal Expr'
source_filename = "Literal Expr"

@0 = private unnamed_addr constant [13 x i8] c"exprRet: %d\0A\00", align 1
@1 = private unnamed_addr constant [13 x i8] c"exprRet: %d\0A\00", align 1
@2 = private unnamed_addr constant [13 x i8] c"exprRet: %d\0A\00", align 1
@3 = private unnamed_addr constant [13 x i8] c"exprRet: %d\0A\00", align 1

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @0, i32 3)
  %1 = call i32 (ptr, ...) @printf(ptr @1, i32 5)
  %2 = call i32 (ptr, ...) @printf(ptr @2, i32 7)
  %3 = call i32 (ptr, ...) @printf(ptr @3, i32 -369113415)
  ret i32 0
}
