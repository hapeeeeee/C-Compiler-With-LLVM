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
  %0 = icmp eq i32 %b3, 1
  %1 = sext i1 %0 to i32
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %then, label %else

then:                                             ; preds = %cond
  store i32 1, ptr %b, align 4
  %b4 = load i32, ptr %b, align 4
  %b5 = load i32, ptr %b, align 4
  %3 = add nsw i32 %b5, 1
  store i32 %3, ptr %b, align 4
  %b6 = load i32, ptr %b, align 4
  %b7 = load i32, ptr %b, align 4
  %4 = add nsw i32 %b7, 2
  store i32 %4, ptr %b, align 4
  %b8 = load i32, ptr %b, align 4
  br label %cond9

else:                                             ; preds = %cond
  store i32 20, ptr %b, align 4
  %b17 = load i32, ptr %b, align 4
  br label %last

last:                                             ; preds = %else, %last11
  %c = alloca i32, align 4
  %d = alloca i32, align 4
  store i32 2, ptr %d, align 4
  %d18 = load i32, ptr %d, align 4
  %e = alloca i32, align 4
  store i32 3, ptr %e, align 4
  %e19 = load i32, ptr %e, align 4
  %f = alloca i32, align 4
  store i32 4, ptr %f, align 4
  %f20 = load i32, ptr %f, align 4
  store i32 2, ptr %e, align 4
  %e21 = load i32, ptr %e, align 4
  %b22 = load i32, ptr %b, align 4
  %5 = add nsw i32 1, %b22
  %6 = mul nsw i32 %5, 3
  %f23 = load i32, ptr %f, align 4
  %d24 = load i32, ptr %d, align 4
  %7 = sdiv i32 %f23, %d24
  %8 = sub nsw i32 %6, %7
  %e25 = load i32, ptr %e, align 4
  %9 = add nsw i32 %8, %e25
  %10 = call i32 (ptr, ...) @printf(ptr @0, i32 %9)
  ret i32 0

cond9:                                            ; preds = %then
  %a12 = load i32, ptr %a, align 4
  %11 = icmp sge i32 %a12, 0
  %12 = sext i1 %11 to i32
  %13 = icmp ne i32 %12, 0
  br i1 %13, label %then10, label %last11

then10:                                           ; preds = %cond9
  %b13 = load i32, ptr %b, align 4
  %14 = add nsw i32 %b13, 3
  store i32 %14, ptr %b, align 4
  %b14 = load i32, ptr %b, align 4
  %b15 = load i32, ptr %b, align 4
  %15 = add nsw i32 %b15, 4
  store i32 %15, ptr %b, align 4
  %b16 = load i32, ptr %b, align 4
  br label %last11

last11:                                           ; preds = %then10, %cond9
  br label %last
}
