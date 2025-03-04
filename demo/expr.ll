; ModuleID = './expr.txt'
source_filename = "./expr.txt"

%TreeNode = type { i32, i32, ptr, ptr }

define ptr @rotateLeft(ptr %pivot, ptr %parent) {
entry:
  %h = alloca ptr, align 8
  %pivot1 = alloca ptr, align 8
  store ptr %pivot, ptr %pivot1, align 8
  %parent2 = alloca ptr, align 8
  store ptr %parent, ptr %parent2, align 8
  %pivot3 = load ptr, ptr %pivot1, align 8
  %0 = getelementptr inbounds %TreeNode, ptr %pivot3, i32 0, i32 2
  %1 = load ptr, ptr %0, align 8
  store ptr %1, ptr %h, align 8
  %2 = load ptr, ptr %h, align 8
  %pivot4 = load ptr, ptr %pivot1, align 8
  %3 = getelementptr inbounds %TreeNode, ptr %pivot4, i32 0, i32 2
  %4 = load ptr, ptr %3, align 8
  %h5 = load ptr, ptr %h, align 8
  %5 = getelementptr inbounds %TreeNode, ptr %h5, i32 0, i32 3
  %6 = load ptr, ptr %5, align 8
  store ptr %6, ptr %3, align 8
  %h6 = load ptr, ptr %h, align 8
  %7 = getelementptr inbounds %TreeNode, ptr %h6, i32 0, i32 3
  %8 = load ptr, ptr %7, align 8
  %pivot7 = load ptr, ptr %pivot1, align 8
  store ptr %pivot7, ptr %7, align 8
  %pivot8 = load ptr, ptr %pivot1, align 8
  call void @updateHeight(ptr %pivot8)
  %h9 = load ptr, ptr %h, align 8
  call void @updateHeight(ptr %h9)
  br label %cond

cond:                                             ; preds = %entry
  %parent10 = load ptr, ptr %parent2, align 8
  %9 = ptrtoint ptr %parent10 to i32
  %10 = icmp ne i32 %9, 0
  br i1 %10, label %then, label %last20

then:                                             ; preds = %cond
  br label %cond11

cond11:                                           ; preds = %then
  %parent12 = load ptr, ptr %parent2, align 8
  %11 = getelementptr inbounds %TreeNode, ptr %parent12, i32 0, i32 3
  %12 = load ptr, ptr %11, align 8
  %pivot13 = load ptr, ptr %pivot1, align 8
  %13 = icmp eq ptr %12, %pivot13
  %14 = sext i1 %13 to i32
  %15 = icmp ne i32 %14, 0
  br i1 %15, label %then14, label %else

then14:                                           ; preds = %cond11
  %parent15 = load ptr, ptr %parent2, align 8
  %16 = getelementptr inbounds %TreeNode, ptr %parent15, i32 0, i32 3
  %17 = load ptr, ptr %16, align 8
  %h16 = load ptr, ptr %h, align 8
  store ptr %h16, ptr %16, align 8
  br label %last

else:                                             ; preds = %cond11
  %parent17 = load ptr, ptr %parent2, align 8
  %18 = getelementptr inbounds %TreeNode, ptr %parent17, i32 0, i32 2
  %19 = load ptr, ptr %18, align 8
  %h18 = load ptr, ptr %h, align 8
  store ptr %h18, ptr %18, align 8
  br label %last

last:                                             ; preds = %else, %then14
  %parent19 = load ptr, ptr %parent2, align 8
  call void @updateHeight(ptr %parent19)
  br label %last20

last20:                                           ; preds = %last, %cond
  %h21 = load ptr, ptr %h, align 8
  ret ptr %h21
}

define ptr @rotateRight(ptr %pivot, ptr %parent) {
entry:
  %h = alloca ptr, align 8
  %pivot1 = alloca ptr, align 8
  store ptr %pivot, ptr %pivot1, align 8
  %parent2 = alloca ptr, align 8
  store ptr %parent, ptr %parent2, align 8
  %pivot3 = load ptr, ptr %pivot1, align 8
  %0 = getelementptr inbounds %TreeNode, ptr %pivot3, i32 0, i32 3
  %1 = load ptr, ptr %0, align 8
  store ptr %1, ptr %h, align 8
  %2 = load ptr, ptr %h, align 8
  %pivot4 = load ptr, ptr %pivot1, align 8
  %3 = getelementptr inbounds %TreeNode, ptr %pivot4, i32 0, i32 3
  %4 = load ptr, ptr %3, align 8
  %h5 = load ptr, ptr %h, align 8
  %5 = getelementptr inbounds %TreeNode, ptr %h5, i32 0, i32 2
  %6 = load ptr, ptr %5, align 8
  store ptr %6, ptr %3, align 8
  %h6 = load ptr, ptr %h, align 8
  %7 = getelementptr inbounds %TreeNode, ptr %h6, i32 0, i32 2
  %8 = load ptr, ptr %7, align 8
  %pivot7 = load ptr, ptr %pivot1, align 8
  store ptr %pivot7, ptr %7, align 8
  %pivot8 = load ptr, ptr %pivot1, align 8
  call void @updateHeight(ptr %pivot8)
  %h9 = load ptr, ptr %h, align 8
  call void @updateHeight(ptr %h9)
  br label %cond

cond:                                             ; preds = %entry
  %parent10 = load ptr, ptr %parent2, align 8
  %9 = ptrtoint ptr %parent10 to i32
  %10 = icmp ne i32 %9, 0
  br i1 %10, label %then, label %last20

then:                                             ; preds = %cond
  br label %cond11

cond11:                                           ; preds = %then
  %parent12 = load ptr, ptr %parent2, align 8
  %11 = getelementptr inbounds %TreeNode, ptr %parent12, i32 0, i32 3
  %12 = load ptr, ptr %11, align 8
  %pivot13 = load ptr, ptr %pivot1, align 8
  %13 = icmp eq ptr %12, %pivot13
  %14 = sext i1 %13 to i32
  %15 = icmp ne i32 %14, 0
  br i1 %15, label %then14, label %else

then14:                                           ; preds = %cond11
  %parent15 = load ptr, ptr %parent2, align 8
  %16 = getelementptr inbounds %TreeNode, ptr %parent15, i32 0, i32 3
  %17 = load ptr, ptr %16, align 8
  %h16 = load ptr, ptr %h, align 8
  store ptr %h16, ptr %16, align 8
  br label %last

else:                                             ; preds = %cond11
  %parent17 = load ptr, ptr %parent2, align 8
  %18 = getelementptr inbounds %TreeNode, ptr %parent17, i32 0, i32 2
  %19 = load ptr, ptr %18, align 8
  %h18 = load ptr, ptr %h, align 8
  store ptr %h18, ptr %18, align 8
  br label %last

last:                                             ; preds = %else, %then14
  %parent19 = load ptr, ptr %parent2, align 8
  call void @updateHeight(ptr %parent19)
  br label %last20

last20:                                           ; preds = %last, %cond
  %h21 = load ptr, ptr %h, align 8
  ret ptr %h21
}

define i32 @getBalance(ptr %h) {
entry:
  %h1 = alloca ptr, align 8
  store ptr %h, ptr %h1, align 8
  %h2 = load ptr, ptr %h1, align 8
  %0 = getelementptr inbounds %TreeNode, ptr %h2, i32 0, i32 3
  %1 = load ptr, ptr %0, align 8
  %2 = call i32 @height(ptr %1)
  %h3 = load ptr, ptr %h1, align 8
  %3 = getelementptr inbounds %TreeNode, ptr %h3, i32 0, i32 2
  %4 = load ptr, ptr %3, align 8
  %5 = call i32 @height(ptr %4)
  %6 = sub nsw i32 %2, %5
  ret i32 %6
}

define i32 @height(ptr %h) {
entry:
  %h1 = alloca ptr, align 8
  store ptr %h, ptr %h1, align 8
  %h2 = load ptr, ptr %h1, align 8
  %0 = ptrtoint ptr %h2 to i32
  %1 = icmp ne i32 %0, 0
  br i1 %1, label %then, label %els

then:                                             ; preds = %entry
  %h3 = load ptr, ptr %h1, align 8
  %2 = getelementptr inbounds %TreeNode, ptr %h3, i32 0, i32 1
  %3 = load i32, ptr %2, align 4
  %4 = add nsw i32 %3, 1
  br label %merge

els:                                              ; preds = %entry
  br label %merge

merge:                                            ; preds = %els, %then
  %5 = phi i32 [ %4, %then ], [ 0, %els ]
  ret i32 %5
}

define void @recursiveAdd(ptr %h, ptr %p, ptr %parent, ptr %root) {
entry:
  %h1 = alloca ptr, align 8
  store ptr %h, ptr %h1, align 8
  %p2 = alloca ptr, align 8
  store ptr %p, ptr %p2, align 8
  %parent3 = alloca ptr, align 8
  store ptr %parent, ptr %parent3, align 8
  %root4 = alloca ptr, align 8
  store ptr %root, ptr %root4, align 8
  br label %cond

cond:                                             ; preds = %entry
  %p5 = load ptr, ptr %p2, align 8
  %0 = getelementptr inbounds %TreeNode, ptr %p5, i32 0, i32 0
  %1 = load i32, ptr %0, align 4
  %h6 = load ptr, ptr %h1, align 8
  %2 = getelementptr inbounds %TreeNode, ptr %h6, i32 0, i32 0
  %3 = load i32, ptr %2, align 4
  %4 = icmp sge i32 %1, %3
  %5 = sext i1 %4 to i32
  %6 = icmp ne i32 %5, 0
  br i1 %6, label %then, label %else16

then:                                             ; preds = %cond
  br label %cond7

cond7:                                            ; preds = %then
  %h8 = load ptr, ptr %h1, align 8
  %7 = getelementptr inbounds %TreeNode, ptr %h8, i32 0, i32 2
  %8 = load ptr, ptr %7, align 8
  %9 = ptrtoint ptr %8 to i32
  %10 = icmp ne i32 %9, 0
  br i1 %10, label %then9, label %else

then9:                                            ; preds = %cond7
  %h10 = load ptr, ptr %h1, align 8
  %11 = getelementptr inbounds %TreeNode, ptr %h10, i32 0, i32 2
  %12 = load ptr, ptr %11, align 8
  %p11 = load ptr, ptr %p2, align 8
  %h12 = load ptr, ptr %h1, align 8
  %root13 = load ptr, ptr %root4, align 8
  call void @recursiveAdd(ptr %12, ptr %p11, ptr %h12, ptr %root13)
  br label %last

else:                                             ; preds = %cond7
  %h14 = load ptr, ptr %h1, align 8
  %13 = getelementptr inbounds %TreeNode, ptr %h14, i32 0, i32 2
  %14 = load ptr, ptr %13, align 8
  %p15 = load ptr, ptr %p2, align 8
  store ptr %p15, ptr %13, align 8
  br label %last

last:                                             ; preds = %else, %then9
  br label %last28

else16:                                           ; preds = %cond
  br label %cond17

cond17:                                           ; preds = %else16
  %h18 = load ptr, ptr %h1, align 8
  %15 = getelementptr inbounds %TreeNode, ptr %h18, i32 0, i32 3
  %16 = load ptr, ptr %15, align 8
  %17 = ptrtoint ptr %16 to i32
  %18 = icmp ne i32 %17, 0
  br i1 %18, label %then19, label %else24

then19:                                           ; preds = %cond17
  %h20 = load ptr, ptr %h1, align 8
  %19 = getelementptr inbounds %TreeNode, ptr %h20, i32 0, i32 3
  %20 = load ptr, ptr %19, align 8
  %p21 = load ptr, ptr %p2, align 8
  %h22 = load ptr, ptr %h1, align 8
  %root23 = load ptr, ptr %root4, align 8
  call void @recursiveAdd(ptr %20, ptr %p21, ptr %h22, ptr %root23)
  br label %last27

else24:                                           ; preds = %cond17
  %h25 = load ptr, ptr %h1, align 8
  %21 = getelementptr inbounds %TreeNode, ptr %h25, i32 0, i32 3
  %22 = load ptr, ptr %21, align 8
  %p26 = load ptr, ptr %p2, align 8
  store ptr %p26, ptr %21, align 8
  br label %last27

last27:                                           ; preds = %else24, %then19
  br label %last28

last28:                                           ; preds = %last27, %last
  %h29 = load ptr, ptr %h1, align 8
  %parent30 = load ptr, ptr %parent3, align 8
  %root31 = load ptr, ptr %root4, align 8
  call void @Rotate(ptr %h29, ptr %parent3, ptr %root31)
  ret void
}

define void @addNode(ptr %root, ptr %p) {
entry:
  %root1 = alloca ptr, align 8
  store ptr %root, ptr %root1, align 8
  %p2 = alloca ptr, align 8
  store ptr %p, ptr %p2, align 8
  %root3 = load ptr, ptr %root1, align 8
  %p4 = load ptr, ptr %p2, align 8
  %root5 = load ptr, ptr %root1, align 8
  call void @recursiveAdd(ptr %root3, ptr %p4, ptr null, ptr %root1)
  ret void
}

define void @Rotate(ptr %h, ptr %parent, ptr %root) {
entry:
  %newparent30 = alloca ptr, align 8
  %newparent = alloca ptr, align 8
  %balance = alloca i32, align 4
  %h1 = alloca ptr, align 8
  store ptr %h, ptr %h1, align 8
  %parent2 = alloca ptr, align 8
  store ptr %parent, ptr %parent2, align 8
  %root3 = alloca ptr, align 8
  store ptr %root, ptr %root3, align 8
  %h4 = load ptr, ptr %h1, align 8
  call void @updateHeight(ptr %h4)
  %h5 = load ptr, ptr %h1, align 8
  %0 = call i32 @getBalance(ptr %h5)
  store i32 %0, ptr %balance, align 4
  %1 = load i32, ptr %balance, align 4
  br label %cond

cond:                                             ; preds = %entry
  %balance6 = load i32, ptr %balance, align 4
  %2 = call i32 @abs(i32 %balance6)
  %3 = icmp eq i32 %2, 2
  %4 = sext i1 %3 to i32
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %then, label %last41

then:                                             ; preds = %cond
  br label %cond7

cond7:                                            ; preds = %then
  %balance8 = load i32, ptr %balance, align 4
  %6 = icmp sgt i32 %balance8, 0
  %7 = sext i1 %6 to i32
  %8 = icmp ne i32 %7, 0
  br i1 %8, label %then9, label %else

then9:                                            ; preds = %cond7
  br label %cond10

cond10:                                           ; preds = %then9
  %h11 = load ptr, ptr %h1, align 8
  %9 = getelementptr inbounds %TreeNode, ptr %h11, i32 0, i32 3
  %10 = load ptr, ptr %9, align 8
  %11 = call i32 @getBalance(ptr %10)
  %12 = icmp slt i32 %11, 0
  %13 = sext i1 %12 to i32
  %14 = icmp ne i32 %13, 0
  br i1 %14, label %then12, label %last

then12:                                           ; preds = %cond10
  %h13 = load ptr, ptr %h1, align 8
  %15 = getelementptr inbounds %TreeNode, ptr %h13, i32 0, i32 3
  %16 = load ptr, ptr %15, align 8
  %h14 = load ptr, ptr %h1, align 8
  %17 = call ptr @rotateLeft(ptr %16, ptr %h14)
  br label %last

last:                                             ; preds = %then12, %cond10
  %h15 = load ptr, ptr %h1, align 8
  %parent16 = load ptr, ptr %parent2, align 8
  %18 = load ptr, ptr %parent16, align 8
  %19 = call ptr @rotateRight(ptr %h15, ptr %18)
  store ptr %19, ptr %newparent, align 8
  %20 = load ptr, ptr %newparent, align 8
  br label %cond17

cond17:                                           ; preds = %last
  %h18 = load ptr, ptr %h1, align 8
  %root19 = load ptr, ptr %root3, align 8
  %21 = load ptr, ptr %root19, align 8
  %22 = icmp eq ptr %h18, %21
  %23 = sext i1 %22 to i32
  %24 = icmp ne i32 %23, 0
  br i1 %24, label %then20, label %last23

then20:                                           ; preds = %cond17
  %root21 = load ptr, ptr %root3, align 8
  %25 = load ptr, ptr %root21, align 8
  %newparent22 = load ptr, ptr %newparent, align 8
  store ptr %newparent22, ptr %root21, align 8
  br label %last23

last23:                                           ; preds = %then20, %cond17
  br label %last40

else:                                             ; preds = %cond7
  br label %cond24

cond24:                                           ; preds = %else
  %h25 = load ptr, ptr %h1, align 8
  %26 = getelementptr inbounds %TreeNode, ptr %h25, i32 0, i32 2
  %27 = load ptr, ptr %26, align 8
  %28 = call i32 @getBalance(ptr %27)
  %29 = icmp sgt i32 %28, 0
  %30 = sext i1 %29 to i32
  %31 = icmp ne i32 %30, 0
  br i1 %31, label %then26, label %last29

then26:                                           ; preds = %cond24
  %h27 = load ptr, ptr %h1, align 8
  %32 = getelementptr inbounds %TreeNode, ptr %h27, i32 0, i32 2
  %33 = load ptr, ptr %32, align 8
  %h28 = load ptr, ptr %h1, align 8
  %34 = call ptr @rotateRight(ptr %33, ptr %h28)
  br label %last29

last29:                                           ; preds = %then26, %cond24
  %h31 = load ptr, ptr %h1, align 8
  %parent32 = load ptr, ptr %parent2, align 8
  %35 = load ptr, ptr %parent32, align 8
  %36 = call ptr @rotateLeft(ptr %h31, ptr %35)
  store ptr %36, ptr %newparent30, align 8
  %37 = load ptr, ptr %newparent30, align 8
  br label %cond33

cond33:                                           ; preds = %last29
  %h34 = load ptr, ptr %h1, align 8
  %root35 = load ptr, ptr %root3, align 8
  %38 = load ptr, ptr %root35, align 8
  %39 = icmp eq ptr %h34, %38
  %40 = sext i1 %39 to i32
  %41 = icmp ne i32 %40, 0
  br i1 %41, label %then36, label %last39

then36:                                           ; preds = %cond33
  %root37 = load ptr, ptr %root3, align 8
  %42 = load ptr, ptr %root37, align 8
  %newparent38 = load ptr, ptr %newparent, align 8
  store ptr %newparent38, ptr %root37, align 8
  br label %last39

last39:                                           ; preds = %then36, %cond33
  br label %last40

last40:                                           ; preds = %last39, %last23
  br label %last41

last41:                                           ; preds = %last40, %cond
  ret void
}

define void @updateHeight(ptr %h) {
entry:
  %h1 = alloca ptr, align 8
  store ptr %h, ptr %h1, align 8
  br label %cond

cond:                                             ; preds = %entry
  %h2 = load ptr, ptr %h1, align 8
  %0 = getelementptr inbounds %TreeNode, ptr %h2, i32 0, i32 2
  %1 = load ptr, ptr %0, align 8
  %2 = call i32 @height(ptr %1)
  %h3 = load ptr, ptr %h1, align 8
  %3 = getelementptr inbounds %TreeNode, ptr %h3, i32 0, i32 3
  %4 = load ptr, ptr %3, align 8
  %5 = call i32 @height(ptr %4)
  %6 = icmp sgt i32 %2, %5
  %7 = sext i1 %6 to i32
  %8 = icmp ne i32 %7, 0
  br i1 %8, label %then, label %else

then:                                             ; preds = %cond
  %h4 = load ptr, ptr %h1, align 8
  %9 = getelementptr inbounds %TreeNode, ptr %h4, i32 0, i32 1
  %10 = load i32, ptr %9, align 4
  %h5 = load ptr, ptr %h1, align 8
  %11 = getelementptr inbounds %TreeNode, ptr %h5, i32 0, i32 2
  %12 = load ptr, ptr %11, align 8
  %13 = call i32 @height(ptr %12)
  store i32 %13, ptr %9, align 4
  br label %last

else:                                             ; preds = %cond
  %h6 = load ptr, ptr %h1, align 8
  %14 = getelementptr inbounds %TreeNode, ptr %h6, i32 0, i32 1
  %15 = load i32, ptr %14, align 4
  %h7 = load ptr, ptr %h1, align 8
  %16 = getelementptr inbounds %TreeNode, ptr %h7, i32 0, i32 3
  %17 = load ptr, ptr %16, align 8
  %18 = call i32 @height(ptr %17)
  store i32 %18, ptr %14, align 4
  br label %last

last:                                             ; preds = %else, %then
  ret void
}

define i32 @abs(i32 %number) {
entry:
  %number1 = alloca i32, align 4
  store i32 %number, ptr %number1, align 4
  %number2 = load i32, ptr %number1, align 4
  %0 = icmp slt i32 %number2, 0
  %1 = sext i1 %0 to i32
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %then, label %els

then:                                             ; preds = %entry
  %number3 = load i32, ptr %number1, align 4
  %3 = sub i32 0, %number3
  br label %merge

els:                                              ; preds = %entry
  %number4 = load i32, ptr %number1, align 4
  br label %merge

merge:                                            ; preds = %els, %then
  %4 = phi i32 [ %3, %then ], [ %number4, %els ]
  ret i32 %4
}

define i32 @main() {
entry:
  %newNode10 = alloca %TreeNode, align 8
  %newNode9 = alloca %TreeNode, align 8
  %newNode8 = alloca %TreeNode, align 8
  %newNode7 = alloca %TreeNode, align 8
  %newNode6 = alloca %TreeNode, align 8
  %newNode5 = alloca %TreeNode, align 8
  %newNode4 = alloca %TreeNode, align 8
  %newNode3 = alloca %TreeNode, align 8
  %newNode2 = alloca %TreeNode, align 8
  %newNode1 = alloca %TreeNode, align 8
  %root = alloca %TreeNode, align 8
  %0 = getelementptr inbounds %TreeNode, ptr %root, i32 0, i32 0
  store i32 1001, ptr %0, align 4
  %1 = getelementptr inbounds %TreeNode, ptr %root, i32 0, i32 1
  store i32 0, ptr %1, align 4
  %2 = getelementptr inbounds %TreeNode, ptr %root, i32 0, i32 2
  store ptr null, ptr %2, align 8
  %3 = getelementptr inbounds %TreeNode, ptr %root, i32 0, i32 3
  store ptr null, ptr %3, align 8
  %4 = load %TreeNode, ptr %root, align 8
  %5 = getelementptr inbounds %TreeNode, ptr %newNode1, i32 0, i32 0
  store i32 1002, ptr %5, align 4
  %6 = getelementptr inbounds %TreeNode, ptr %newNode1, i32 0, i32 1
  store i32 0, ptr %6, align 4
  %7 = getelementptr inbounds %TreeNode, ptr %newNode1, i32 0, i32 2
  store ptr null, ptr %7, align 8
  %8 = getelementptr inbounds %TreeNode, ptr %newNode1, i32 0, i32 3
  store ptr null, ptr %8, align 8
  %9 = load %TreeNode, ptr %newNode1, align 8
  %10 = getelementptr inbounds %TreeNode, ptr %newNode2, i32 0, i32 0
  store i32 1003, ptr %10, align 4
  %11 = getelementptr inbounds %TreeNode, ptr %newNode2, i32 0, i32 1
  store i32 0, ptr %11, align 4
  %12 = getelementptr inbounds %TreeNode, ptr %newNode2, i32 0, i32 2
  store ptr null, ptr %12, align 8
  %13 = getelementptr inbounds %TreeNode, ptr %newNode2, i32 0, i32 3
  store ptr null, ptr %13, align 8
  %14 = load %TreeNode, ptr %newNode2, align 8
  %15 = getelementptr inbounds %TreeNode, ptr %newNode3, i32 0, i32 0
  store i32 888, ptr %15, align 4
  %16 = getelementptr inbounds %TreeNode, ptr %newNode3, i32 0, i32 1
  store i32 0, ptr %16, align 4
  %17 = getelementptr inbounds %TreeNode, ptr %newNode3, i32 0, i32 2
  store ptr null, ptr %17, align 8
  %18 = getelementptr inbounds %TreeNode, ptr %newNode3, i32 0, i32 3
  store ptr null, ptr %18, align 8
  %19 = load %TreeNode, ptr %newNode3, align 8
  %20 = getelementptr inbounds %TreeNode, ptr %newNode4, i32 0, i32 0
  store i32 10010, ptr %20, align 4
  %21 = getelementptr inbounds %TreeNode, ptr %newNode4, i32 0, i32 1
  store i32 0, ptr %21, align 4
  %22 = getelementptr inbounds %TreeNode, ptr %newNode4, i32 0, i32 2
  store ptr null, ptr %22, align 8
  %23 = getelementptr inbounds %TreeNode, ptr %newNode4, i32 0, i32 3
  store ptr null, ptr %23, align 8
  %24 = load %TreeNode, ptr %newNode4, align 8
  %25 = getelementptr inbounds %TreeNode, ptr %newNode5, i32 0, i32 0
  store i32 1006, ptr %25, align 4
  %26 = getelementptr inbounds %TreeNode, ptr %newNode5, i32 0, i32 1
  store i32 0, ptr %26, align 4
  %27 = getelementptr inbounds %TreeNode, ptr %newNode5, i32 0, i32 2
  store ptr null, ptr %27, align 8
  %28 = getelementptr inbounds %TreeNode, ptr %newNode5, i32 0, i32 3
  store ptr null, ptr %28, align 8
  %29 = load %TreeNode, ptr %newNode5, align 8
  %30 = getelementptr inbounds %TreeNode, ptr %newNode6, i32 0, i32 0
  store i32 1006, ptr %30, align 4
  %31 = getelementptr inbounds %TreeNode, ptr %newNode6, i32 0, i32 1
  store i32 0, ptr %31, align 4
  %32 = getelementptr inbounds %TreeNode, ptr %newNode6, i32 0, i32 2
  store ptr null, ptr %32, align 8
  %33 = getelementptr inbounds %TreeNode, ptr %newNode6, i32 0, i32 3
  store ptr null, ptr %33, align 8
  %34 = load %TreeNode, ptr %newNode6, align 8
  %35 = getelementptr inbounds %TreeNode, ptr %newNode7, i32 0, i32 0
  store i32 1006, ptr %35, align 4
  %36 = getelementptr inbounds %TreeNode, ptr %newNode7, i32 0, i32 1
  store i32 0, ptr %36, align 4
  %37 = getelementptr inbounds %TreeNode, ptr %newNode7, i32 0, i32 2
  store ptr null, ptr %37, align 8
  %38 = getelementptr inbounds %TreeNode, ptr %newNode7, i32 0, i32 3
  store ptr null, ptr %38, align 8
  %39 = load %TreeNode, ptr %newNode7, align 8
  %40 = getelementptr inbounds %TreeNode, ptr %newNode8, i32 0, i32 0
  store i32 1006, ptr %40, align 4
  %41 = getelementptr inbounds %TreeNode, ptr %newNode8, i32 0, i32 1
  store i32 0, ptr %41, align 4
  %42 = getelementptr inbounds %TreeNode, ptr %newNode8, i32 0, i32 2
  store ptr null, ptr %42, align 8
  %43 = getelementptr inbounds %TreeNode, ptr %newNode8, i32 0, i32 3
  store ptr null, ptr %43, align 8
  %44 = load %TreeNode, ptr %newNode8, align 8
  %45 = getelementptr inbounds %TreeNode, ptr %newNode9, i32 0, i32 0
  store i32 1006, ptr %45, align 4
  %46 = getelementptr inbounds %TreeNode, ptr %newNode9, i32 0, i32 1
  store i32 0, ptr %46, align 4
  %47 = getelementptr inbounds %TreeNode, ptr %newNode9, i32 0, i32 2
  store ptr null, ptr %47, align 8
  %48 = getelementptr inbounds %TreeNode, ptr %newNode9, i32 0, i32 3
  store ptr null, ptr %48, align 8
  %49 = load %TreeNode, ptr %newNode9, align 8
  %50 = getelementptr inbounds %TreeNode, ptr %newNode10, i32 0, i32 0
  store i32 1006, ptr %50, align 4
  %51 = getelementptr inbounds %TreeNode, ptr %newNode10, i32 0, i32 1
  store i32 0, ptr %51, align 4
  %52 = getelementptr inbounds %TreeNode, ptr %newNode10, i32 0, i32 2
  store ptr null, ptr %52, align 8
  %53 = getelementptr inbounds %TreeNode, ptr %newNode10, i32 0, i32 3
  store ptr null, ptr %53, align 8
  %54 = load %TreeNode, ptr %newNode10, align 8
  %root1 = load %TreeNode, ptr %root, align 8
  %newNode12 = load %TreeNode, ptr %newNode1, align 8
  call void @addNode(ptr %root, ptr %newNode1)
  %root3 = load %TreeNode, ptr %root, align 8
  %newNode24 = load %TreeNode, ptr %newNode2, align 8
  call void @addNode(ptr %root, ptr %newNode2)
  %root5 = load %TreeNode, ptr %root, align 8
  %newNode36 = load %TreeNode, ptr %newNode3, align 8
  call void @addNode(ptr %root, ptr %newNode3)
  %root7 = load %TreeNode, ptr %root, align 8
  %newNode48 = load %TreeNode, ptr %newNode4, align 8
  call void @addNode(ptr %root, ptr %newNode4)
  %root9 = load %TreeNode, ptr %root, align 8
  %newNode510 = load %TreeNode, ptr %newNode5, align 8
  call void @addNode(ptr %root, ptr %newNode5)
  %root11 = load %TreeNode, ptr %root, align 8
  %newNode612 = load %TreeNode, ptr %newNode6, align 8
  call void @addNode(ptr %root, ptr %newNode6)
  %root13 = load %TreeNode, ptr %root, align 8
  %newNode714 = load %TreeNode, ptr %newNode7, align 8
  call void @addNode(ptr %root, ptr %newNode7)
  %root15 = load %TreeNode, ptr %root, align 8
  %newNode816 = load %TreeNode, ptr %newNode8, align 8
  call void @addNode(ptr %root, ptr %newNode8)
  %root17 = load %TreeNode, ptr %root, align 8
  %newNode918 = load %TreeNode, ptr %newNode9, align 8
  call void @addNode(ptr %root, ptr %newNode9)
  %root19 = load %TreeNode, ptr %root, align 8
  %newNode1020 = load %TreeNode, ptr %newNode10, align 8
  call void @addNode(ptr %root, ptr %newNode10)
  %newNode421 = load %TreeNode, ptr %newNode4, align 8
  %55 = getelementptr inbounds %TreeNode, ptr %newNode4, i32 0, i32 1
  %56 = load i32, ptr %55, align 4
  ret i32 %56
}
