; ModuleID = 'Literal Expr'
source_filename = "Literal Expr"

@0 = private unnamed_addr constant [13 x i8] c"lastVal: %d\0A\00", align 1

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %a = alloca i32, align 4
  store i32 0, ptr %a, align 4
  %a1 = load i32, ptr %a, align 4
  %b = alloca i32, align 4
  store i32 2, ptr %b, align 4
  %b2 = load i32, ptr %b, align 4
  br label %cond

cond:                                             ; preds = %entry
  %b3 = load i32, ptr %b, align 4
  %0 = icmp ne i32 %b3, 0
  br i1 %0, label %then, label %else

then:                                             ; preds = %cond
  br label %cond4

else:                                             ; preds = %cond
  store i32 20, ptr %b, align 4
  %b11 = load i32, ptr %b, align 4
  br label %last

last:                                             ; preds = %else, %last7
  %c = alloca i32, align 4
  %d = alloca i32, align 4
  store i32 2, ptr %d, align 4
  %d12 = load i32, ptr %d, align 4
  %e = alloca i32, align 4
  store i32 3, ptr %e, align 4
  %e13 = load i32, ptr %e, align 4
  %f = alloca i32, align 4
  store i32 4, ptr %f, align 4
  %f14 = load i32, ptr %f, align 4
  store i32 2, ptr %e, align 4
  %e15 = load i32, ptr %e, align 4
  %b16 = load i32, ptr %b, align 4
  %1 = add nsw i32 1, %b16
  %2 = mul nsw i32 %1, 3
  %f17 = load i32, ptr %f, align 4
  %d18 = load i32, ptr %d, align 4
  %3 = sdiv i32 %f17, %d18
  %4 = sub nsw i32 %2, %3
  %e19 = load i32, ptr %e, align 4
  %5 = add nsw i32 %4, %e19
  %6 = call i32 (ptr, ...) @printf(ptr @0, i32 %5)
  ret i32 0

cond4:                                            ; preds = %then
  %a8 = load i32, ptr %a, align 4
  %7 = icmp ne i32 %a8, 0
  br i1 %7, label %then5, label %else6

then5:                                            ; preds = %cond4
  store i32 1, ptr %b, align 4
  %b9 = load i32, ptr %b, align 4
  br label %last7

else6:                                            ; preds = %cond4
  store i32 10, ptr %b, align 4
  %b10 = load i32, ptr %b, align 4
  br label %last7

last7:                                            ; preds = %else6, %then5
  br label %last
}
