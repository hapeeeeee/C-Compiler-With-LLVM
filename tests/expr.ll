; ModuleID = 'Literal Expr'
source_filename = "Literal Expr"

@0 = private unnamed_addr constant [13 x i8] c"lastVal: %d\0A\00", align 1

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %b = alloca i32, align 4
  store i32 0, ptr %b, align 4
  %b1 = load i32, ptr %b, align 4
  br label %for.init

for.init:                                         ; preds = %entry
  %i = alloca i32, align 4
  store i32 0, ptr %i, align 4
  %i2 = load i32, ptr %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.then, %for.init
  %i3 = load i32, ptr %i, align 4
  %0 = icmp slt i32 %i3, 10
  %1 = sext i1 %0 to i32
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %for.body, label %for.last

for.then:                                         ; preds = %for.continue.death, %last
  %i15 = load i32, ptr %i, align 4
  %3 = add nsw i32 %i15, 1
  store i32 %3, ptr %i, align 4
  %i16 = load i32, ptr %i, align 4
  br label %for.cond

for.body:                                         ; preds = %for.cond
  %b4 = load i32, ptr %b, align 4
  %i5 = load i32, ptr %i, align 4
  %4 = add nsw i32 %b4, %i5
  store i32 %4, ptr %b, align 4
  %b6 = load i32, ptr %b, align 4
  br label %cond

for.last:                                         ; preds = %then, %for.cond
  %b17 = load i32, ptr %b, align 4
  %5 = call i32 (ptr, ...) @printf(ptr @0, i32 %b17)
  ret i32 0

cond:                                             ; preds = %for.body
  %i7 = load i32, ptr %i, align 4
  %6 = icmp eq i32 %i7, 5
  %7 = sext i1 %6 to i32
  %8 = icmp ne i32 %7, 0
  br i1 %8, label %then, label %last

then:                                             ; preds = %cond
  br label %for.last

last:                                             ; preds = %for.break.death, %cond
  %b9 = load i32, ptr %b, align 4
  %i10 = load i32, ptr %i, align 4
  %9 = add nsw i32 %b9, %i10
  store i32 %9, ptr %b, align 4
  %b11 = load i32, ptr %b, align 4
  br label %for.then

for.break.death:                                  ; No predecessors!
  %b8 = load i32, ptr %b, align 4
  br label %last

for.continue.death:                               ; No predecessors!
  %b12 = load i32, ptr %b, align 4
  %i13 = load i32, ptr %i, align 4
  %10 = add nsw i32 %b12, %i13
  store i32 %10, ptr %b, align 4
  %b14 = load i32, ptr %b, align 4
  br label %for.then
}
