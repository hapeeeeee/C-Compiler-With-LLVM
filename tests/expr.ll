; ModuleID = 'Literal Expr'
source_filename = "Literal Expr"

@0 = private unnamed_addr constant [13 x i8] c"lastVal: %d\0A\00", align 1

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %aa = alloca i32, align 4
  store i32 1, ptr %aa, align 4
  %aa1 = load i32, ptr %aa, align 4
  %b = alloca i32, align 4
  store i32 1, ptr %b, align 4
  %b2 = load i32, ptr %b, align 4
  br i1 false, label %trueBB, label %nextBB

trueBB:                                           ; preds = %entry
  br label %mergeBB

nextBB:                                           ; preds = %entry
  %b8 = load i32, ptr %b, align 4
  %0 = icmp ne i32 %b8, 0
  br i1 %0, label %nextBB6, label %falseBB5

mergeBB:                                          ; preds = %mergeBB4, %trueBB
  %1 = phi i32 [ 1, %trueBB ], [ %7, %mergeBB4 ]
  store i32 %1, ptr %aa, align 4
  %aa10 = load i32, ptr %aa, align 4
  %aa11 = load i32, ptr %aa, align 4
  %b12 = load i32, ptr %b, align 4
  %2 = add nsw i32 %aa11, %b12
  %3 = call i32 (ptr, ...) @printf(ptr @0, i32 %2)
  ret i32 0

falseBB:                                          ; preds = %mergeBB7
  br label %mergeBB4

nextBB3:                                          ; preds = %mergeBB7
  %4 = icmp ne i32 %9, 0
  br label %mergeBB4

mergeBB4:                                         ; preds = %falseBB, %nextBB3
  %5 = phi i32 [ 0, %falseBB ], [ 0, %nextBB3 ]
  %6 = icmp ne i32 %5, 0
  %7 = zext i1 %6 to i32
  br label %mergeBB

falseBB5:                                         ; preds = %nextBB
  br label %mergeBB7

nextBB6:                                          ; preds = %nextBB
  %aa9 = load i32, ptr %aa, align 4
  %8 = icmp ne i32 %b8, 0
  br label %mergeBB7

mergeBB7:                                         ; preds = %falseBB5, %nextBB6
  %9 = phi i32 [ 0, %falseBB5 ], [ %aa9, %nextBB6 ]
  %10 = icmp ne i32 %9, 0
  br i1 %10, label %nextBB3, label %falseBB
}
