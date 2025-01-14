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
  br label %for.init

cond9:                                            ; preds = %then
  %a12 = load i32, ptr %a, align 4
  %5 = icmp sge i32 %a12, 0
  %6 = sext i1 %5 to i32
  %7 = icmp ne i32 %6, 0
  br i1 %7, label %then10, label %last11

then10:                                           ; preds = %cond9
  %b13 = load i32, ptr %b, align 4
  %8 = add nsw i32 %b13, 3
  store i32 %8, ptr %b, align 4
  %b14 = load i32, ptr %b, align 4
  %b15 = load i32, ptr %b, align 4
  %9 = add nsw i32 %b15, 4
  store i32 %9, ptr %b, align 4
  %b16 = load i32, ptr %b, align 4
  br label %last11

last11:                                           ; preds = %then10, %cond9
  br label %last

for.init:                                         ; preds = %last
  %i = alloca i32, align 4
  store i32 0, ptr %i, align 4
  %i22 = load i32, ptr %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.then, %for.init
  %i23 = load i32, ptr %i, align 4
  %10 = icmp slt i32 %i23, 10
  %11 = sext i1 %10 to i32
  %12 = icmp ne i32 %11, 0
  br i1 %12, label %for.body, label %for.last

for.then:                                         ; preds = %for.body
  %i27 = load i32, ptr %i, align 4
  %13 = add nsw i32 %i27, 1
  store i32 %13, ptr %i, align 4
  %i28 = load i32, ptr %i, align 4
  br label %for.cond

for.body:                                         ; preds = %for.cond
  %b24 = load i32, ptr %b, align 4
  %i25 = load i32, ptr %i, align 4
  %14 = add nsw i32 %b24, %i25
  store i32 %14, ptr %b, align 4
  %b26 = load i32, ptr %b, align 4
  br label %for.then

for.last:                                         ; preds = %for.cond
  %b29 = load i32, ptr %b, align 4
  %15 = add nsw i32 1, %b29
  %16 = mul nsw i32 %15, 3
  %f30 = load i32, ptr %f, align 4
  %d31 = load i32, ptr %d, align 4
  %17 = sdiv i32 %f30, %d31
  %18 = sub nsw i32 %16, %17
  %e32 = load i32, ptr %e, align 4
  %19 = add nsw i32 %18, %e32
  %20 = call i32 (ptr, ...) @printf(ptr @0, i32 %19)
  ret i32 0
}
