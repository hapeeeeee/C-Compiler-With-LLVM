gcc ./test.c
./a.out
../bin/CC_LLVM ./expr.txt > ./expr.ll
/home/zax/third-project/llvm-20/llvm-install-dir/bin/lli ./expr.ll