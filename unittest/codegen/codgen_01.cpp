#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include <gtest/gtest.h>

#include "CodeGen.h"
#include "Lexer.h"
#include "Parser.h"

#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetSelect.h"
#include <functional>
#include <stdarg.h>

bool TestProgramUseJit(llvm::StringRef content, int expectValue) {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    LLVMLinkInMCJIT();
    llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> buf = llvm : : MemoryBuffer::getMemBuffer(content, "stdin");
    if (!buf) {
        llvm::errs() << "open file failed!!!\n";
        return false;
    }
    llvm::SourceMgr mgr;
    Diagnostics diagEngine(mgr);
    mgr.AddNewSourceBuffer(std::move(*buf), llvm::SMLoc());
    Lexer lex(mgr, diagEngine);
    Sema sema(diagEngine);
    Parser parser(lex, sema);
    auto program = parser.ParserProgram();
    CodeGen codegen(program);
    auto &module = codegen.GetModule();
    {
        EXPECT_FALSE(llvm::verifyModule(*module));
        llvm::EngineBuilder builder(std::move(module));
        std::string error;
        auto ptr = std::make_unique<llvm::SectionMemoryManager>();
        auto ref = ptr.get();
        std::unique_ptr<llvm::ExecutionEngine> ee(builder.setErrorStr(&error)
                                                      .setEngineKind(llvm::EngineKind::JIT)
                                                      .setOptLevel(llvm::CodeGenOptLevel::None)
                                                      .setSymbolResolver(std::move(ptr))
                                                      .create());

        ref->finalizeMemory(&error);
        void *addr = (void *)ee->getFunctionAddress("main");
        int res    = ((int (*)())addr)();
        if (res != expectValue) {
            llvm::errs() << "expected: " << expectValue << ", but gat " << res << "\n";
        }
        EXPECT_EQ(res, expectValue);
    }
    return true;
}

TEST(CodeGenTest, assign) {
    bool res = TestProgramUseJit("{int a; int b=4;a=3;b=5; a= b;}", 5);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, add_assign) {
    bool res = TestProgramUseJit("{int a;int b=4;a=3;b=5; a += b;}", 8);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, sub_assign) {
    bool res = TestProgramUseJit("(int a;int b=4;a=3;b = 5; a -= b;}", -2);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, mut_assign) {
    bool res = TestProgramUseJit("{int a;int b=4;a= 3;b = 5; a *= b;}", 15);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, div_assign) {
    bool res = TestProgramUseJit("{int a; int b= 4; a= 3; b = 5; a /= b;}", 0);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, mod_assign) {
    bool res = TestProgramUseJit("{int a;int b=4;a=3;b=5; a %= b;}", 3);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, bit_or_assign) {
    bool res = TestProgramUseJit("{int a; int b=4; a = 3; b = 5; a |= b;}", 7);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, bit_and_assign) {
    bool res = TestProgramUseJit("{int a; int b=4;a=3;b=5; a &= b;}", 1);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, bit_xor_assign) {
    bool res = TestProgramUseJit("{int a; int b=4; a = 3; b = 5; a ^= b;}", 6);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, left_shift_assign) {
    bool res = TestProgramUseJit("{int a; int b=4; a=3; b = 5; a <<= b;}", 96);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, right_shift_assign) {
    bool res = TestProgramUseJit("{int a; int b=4; a= 3; b = 5; a >>= b;}", 0);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, three_op1) {
    bool res = TestProgramUseJit("{inta=1,b=2,ans;ans =(a==1?(b== 2 ?3 : 5): 0);}", 3);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, three_op2) {
    bool res = TestProgramUseJit("{int a=10,b= 20,c; c =(a < b)? a : b;}", 10);
    ASSERT_EQ(res, true);
}
TEST(CodeGenTest, sizeof_int) {
    bool res = TestProgramUseJit("{int a= 10; sizeof(int);}", 4);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, sizeof_pointer) {
    bool res = TestProgramUseJit("{int a =10; sizeof(int*);}", 8);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, sizeof_unary) {
    bool res = TestProgramUseJit("{int a =sizeof(a)+ sizeof a;}", 8);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, unary_positive) {
    bool res = TestProgramUseJit("{inta=10;+a;}", 10);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, unary_negative) {
    bool res = TestProgramUseJit("{int a=10;-a;}", -10);
    ASSERT_EQ(res, true);
}