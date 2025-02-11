; ModuleID = 'Literal Expr'
source_filename = "Literal Expr"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %a = alloca i32, align 4
  store i32 1, ptr %a, align 4
  %b = alloca i32, align 4
  store i32 2, ptr %b, align 4
  %ans = alloca i32, align 4
  %ans1 = load i32, ptr %ans, align 4
  %a2 = load i32, ptr %a, align 4
  %0 = icmp eq i32 %a2, 1
  %1 = sext i1 %0 to i32
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %threeExpr_trueBB, label %threeExpr_falseBB5

threeExpr_trueBB:                                 ; preds = %entry
  %b3 = load i32, ptr %b, align 4
  %3 = icmp eq i32 %b3, 2
  %4 = sext i1 %3 to i32
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %threeExpr_trueBB4, label %threeExpr_falseBB

threeExpr_trueBB4:                                ; preds = %threeExpr_trueBB
  br label %threeExpr_lastBB

threeExpr_falseBB:                                ; preds = %threeExpr_trueBB
  br label %threeExpr_lastBB

threeExpr_lastBB:                                 ; preds = %threeExpr_falseBB, %threeExpr_trueBB4
  br label %threeExpr_lastBB6

threeExpr_falseBB5:                               ; preds = %entry
  br label %threeExpr_lastBB6

threeExpr_lastBB6:                                ; preds = %threeExpr_falseBB5, %threeExpr_lastBB
  store i32 <badref>, ptr %ans, align 4
  ret i32 <badref>
}
