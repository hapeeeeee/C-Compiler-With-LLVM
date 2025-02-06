; ModuleID = 'Literal Expr'
source_filename = "Literal Expr"

@0 = private unnamed_addr constant [13 x i8] c"lastVal: %d\0A\00", align 1

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %a1 = alloca i32, align 4
  %a2 = alloca i32, align 4
  %a3 = alloca i32, align 4
  %a4 = alloca i32, align 4
  %a5 = alloca i32, align 4
  %a6 = alloca i32, align 4
  %a7 = alloca i32, align 4
  %a8 = alloca i32, align 4
  %b1 = alloca i32, align 4
  %b2 = alloca i32, align 4
  %b3 = alloca i32, align 4
  %b4 = alloca i32, align 4
  %b5 = alloca i32, align 4
  %b6 = alloca i32, align 4
  %b7 = alloca i32, align 4
  %b8 = alloca i32, align 4
  %b9 = alloca i32, align 4
  %b81 = load i32, ptr %b8, align 4
  %b72 = load i32, ptr %b7, align 4
  %0 = add nsw i32 %b81, %b72
  %b63 = load i32, ptr %b6, align 4
  %1 = add nsw i32 %0, %b63
  %b54 = load i32, ptr %b5, align 4
  %2 = add nsw i32 %1, %b54
  %b45 = load i32, ptr %b4, align 4
  %3 = add nsw i32 %2, %b45
  %b36 = load i32, ptr %b3, align 4
  %4 = add nsw i32 %3, %b36
  %b27 = load i32, ptr %b2, align 4
  %5 = add nsw i32 %4, %b27
  %b18 = load i32, ptr %b1, align 4
  %6 = add nsw i32 %5, %b18
  %7 = call i32 (ptr, ...) @printf(ptr @0, i32 %6)
  ret i32 0
}
