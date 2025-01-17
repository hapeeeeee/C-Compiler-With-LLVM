; ModuleID = 'Literal Expr'
source_filename = "Literal Expr"

@0 = private unnamed_addr constant [13 x i8] c"lastVal: %d\0A\00", align 1

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %a1 = alloca i32, align 4
  store i32 1, ptr %a1, align 4
  %a11 = load i32, ptr %a1, align 4
  %a2 = alloca i32, align 4
  store i32 0, ptr %a2, align 4
  %a22 = load i32, ptr %a2, align 4
  %a3 = alloca i32, align 4
  store i32 1, ptr %a3, align 4
  %a33 = load i32, ptr %a3, align 4
  %a4 = alloca i32, align 4
  store i32 1159, ptr %a4, align 4
  %a44 = load i32, ptr %a4, align 4
  %a5 = alloca i32, align 4
  store i32 1, ptr %a5, align 4
  %a55 = load i32, ptr %a5, align 4
  %a6 = alloca i32, align 4
  store i32 1, ptr %a6, align 4
  %a66 = load i32, ptr %a6, align 4
  %a7 = alloca i32, align 4
  store i32 1, ptr %a7, align 4
  %a77 = load i32, ptr %a7, align 4
  %a8 = alloca i32, align 4
  store i32 1984, ptr %a8, align 4
  %a88 = load i32, ptr %a8, align 4
  %b1 = alloca i32, align 4
  %a19 = load i32, ptr %a1, align 4
  %0 = icmp ne i32 %a19, 0
  br i1 %0, label %nextBB, label %falseBB

falseBB:                                          ; preds = %entry
  br label %mergeBB

nextBB:                                           ; preds = %entry
  %1 = icmp ne i32 %a19, 0
  br label %mergeBB

mergeBB:                                          ; preds = %falseBB, %nextBB
  %2 = phi i32 [ 0, %falseBB ], [ 1, %nextBB ]
  store i32 %2, ptr %b1, align 4
  %b110 = load i32, ptr %b1, align 4
  %b2 = alloca i32, align 4
  %a213 = load i32, ptr %a2, align 4
  %3 = icmp ne i32 %a213, 0
  br i1 %3, label %trueBB, label %nextBB11

trueBB:                                           ; preds = %mergeBB
  br label %mergeBB12

nextBB11:                                         ; preds = %mergeBB
  br label %mergeBB12

mergeBB12:                                        ; preds = %nextBB11, %trueBB
  %4 = phi i32 [ 1, %trueBB ], [ 0, %nextBB11 ]
  store i32 %4, ptr %b2, align 4
  %b214 = load i32, ptr %b2, align 4
  %b3 = alloca i32, align 4
  %a315 = load i32, ptr %a3, align 4
  %5 = shl i32 %a315, 2
  store i32 %5, ptr %b3, align 4
  %b316 = load i32, ptr %b3, align 4
  %b4 = alloca i32, align 4
  %a417 = load i32, ptr %a4, align 4
  %6 = lshr i32 %a417, 2
  store i32 %6, ptr %b4, align 4
  %b418 = load i32, ptr %b4, align 4
  %b5 = alloca i32, align 4
  %a519 = load i32, ptr %a5, align 4
  %7 = and i32 %a519, 11
  store i32 %7, ptr %b5, align 4
  %b520 = load i32, ptr %b5, align 4
  %b6 = alloca i32, align 4
  %a621 = load i32, ptr %a6, align 4
  %8 = or i32 %a621, 7
  store i32 %8, ptr %b6, align 4
  %b622 = load i32, ptr %b6, align 4
  %b7 = alloca i32, align 4
  %a723 = load i32, ptr %a7, align 4
  %9 = xor i32 %a723, 78
  store i32 %9, ptr %b7, align 4
  %b724 = load i32, ptr %b7, align 4
  %b8 = alloca i32, align 4
  %a825 = load i32, ptr %a8, align 4
  %10 = srem i32 %a825, 10
  store i32 %10, ptr %b8, align 4
  %b826 = load i32, ptr %b8, align 4
  %b127 = load i32, ptr %b1, align 4
  %b228 = load i32, ptr %b2, align 4
  %11 = add nsw i32 %b127, %b228
  %b329 = load i32, ptr %b3, align 4
  %12 = add nsw i32 %11, %b329
  %b430 = load i32, ptr %b4, align 4
  %13 = add nsw i32 %12, %b430
  %b531 = load i32, ptr %b5, align 4
  %14 = add nsw i32 %13, %b531
  %b632 = load i32, ptr %b6, align 4
  %15 = add nsw i32 %14, %b632
  %b733 = load i32, ptr %b7, align 4
  %16 = add nsw i32 %15, %b733
  %b834 = load i32, ptr %b8, align 4
  %17 = add nsw i32 %16, %b834
  %18 = call i32 (ptr, ...) @printf(ptr @0, i32 %17)
  ret i32 0
}
