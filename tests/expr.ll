; ModuleID = 'Literal Expr'
source_filename = "Literal Expr"

@0 = private unnamed_addr constant [13 x i8] c"lastVal: %d\0A\00", align 1

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  store i32 1, ptr %b, align 4
  %b1 = load i32, ptr %b, align 4
  %c = alloca i32, align 4
  %d = alloca i32, align 4
  store i32 2, ptr %d, align 4
  %d2 = load i32, ptr %d, align 4
  %e = alloca i32, align 4
  store i32 3, ptr %e, align 4
  %e3 = load i32, ptr %e, align 4
  %f = alloca i32, align 4
  store i32 4, ptr %f, align 4
  %f4 = load i32, ptr %f, align 4
  store i32 2, ptr %e, align 4
  %e5 = load i32, ptr %e, align 4
  store i32 %e5, ptr %b, align 4
  %b6 = load i32, ptr %b, align 4
  %b7 = load i32, ptr %b, align 4
  %0 = mul nsw i32 %b7, 3
  %1 = add nsw i32 1, %0
  %f8 = load i32, ptr %f, align 4
  %d9 = load i32, ptr %d, align 4
  %2 = sdiv i32 %f8, %d9
  %3 = sub nsw i32 %1, %2
  %e10 = load i32, ptr %e, align 4
  %4 = add nsw i32 %3, %e10
  %5 = call i32 (ptr, ...) @printf(ptr @0, i32 %4)
  ret i32 0
}
