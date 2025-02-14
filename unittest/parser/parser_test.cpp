#include "Lexer.h"
#include "Parser.h"
#include "PrintVisitor.h"
#include <gtest/gtest.h>

bool TestParserWithContent(llvm::StringRef content, llvm::StringRef expect) {
    llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> buf = llvm::MemoryBuffer::getMemBuffer(content, "stdin");
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

    std::string s;
    llvm::raw_string_ostream ss(s);
    PrintVisitor printVisitor(program, &ss);
    if (expect != s) {
        llvm::outs() << "expect: " << expect << ", but got: " << s << "\n";
    }
    EXPECT_EQ(expect, s);
    return true;
}

TEST(ParserTest, logor) {
    bool res = TestParserWithContent("{1||2;}", "{1||2;}");
    ASSERT_EQ(res, true);
}

TEST(ParserTest, logand) {
    bool res = TestParserWithContent("{1&&2;}", "{1&&2;}");
    ASSERT_EQ(res, true);
}

TEST(ParserTest, bitor) {
    bool res = TestParserWithContent("{1|2;}", "{1|2;}");
    ASSERT_EQ(res, true);
}

TEST(ParserTest, bitxor) {
    bool res = TestParserWithContent("{1^2;}", "{1^2;}");
    ASSERT_EQ(res, true);
}

TEST(ParserTest, bitand) {
    bool res = TestParserWithContent("{1&2;}", "{1&2;}");
    ASSERT_EQ(res, true);
}

TEST(ParserTest, lshift) {
    bool res = TestParserWithContent("{1<<2;}", "{1<<2;}");
    ASSERT_EQ(res, true);
}

TEST(ParserTest, rshift) {
    bool res = TestParserWithContent("{1>>2;}", "{1>>2;}");
    ASSERT_EQ(res, true);
}

TEST(ParserTest, equal) {
    bool res = TestParserWithContent("{1==2;}", "{1==2;}");
    ASSERT_EQ(res, true);
}

TEST(ParserTest, notequal) {
    bool res = TestParserWithContent("{1!=2;}", "{1!=2;}");
    ASSERT_EQ(res, true);
}

TEST(ParserTest, add) {
    bool res = TestParserWithContent("{1+2;}", "{1+2;}");
    ASSERT_EQ(res, true);
}
TEST(ParserTest, sub) {
    bool res = TestParserWithContent("{1-2;}", "{1-2;}");
    ASSERT_EQ(res, true);
}
TEST(ParserTest, mul) {
    bool res = TestParserWithContent("{1*2;}", "{1*2;}");
    ASSERT_EQ(res, true);
}
TEST(ParserTest, div) {
    bool res = TestParserWithContent("{1/2;}", "{1/2;}");
    ASSERT_EQ(res, true);
}
TEST(ParserTest, mod) {
    bool res = TestParserWithContent("{1%2;}", "{1%2;}");
    ASSERT_EQ(res, true);
}

TEST(ParserTest1, multi_arith) {
    bool res = TestParserWithContent("{1+2/3-4*(6-9);}", "{1+2/3-4*6-9;}");
    ASSERT_EQ(res, true);
}

TEST(ParserTest2, assign) {
    bool res = TestParserWithContent("{int a;a=2;}", "{int a;a=2;}");
    ASSERT_EQ(res, true);
}

TEST(ParserTest, multi_assign) {
    bool res = TestParserWithContent("{int a=0,b=3;a=2;}", "{int a=0;int b=3;a=2;}");
    ASSERT_EQ(res, true);
}

TEST(ParserTest, half_assign) {
    bool res = TestParserWithContent("{int a=0,b;a=5;b=2;}", "{int a=0;int b;a=5;b=2;}");
    ASSERT_EQ(res, true);
}

TEST(ParserTest, if_stmt) {
    bool res =
        TestParserWithContent("{int a=0,b=1;if(a>=0){b=100;}else{b=50;}}", "{int a=0;int b=1;if(a>=0){b=100;}else{b=50;};}");
    ASSERT_EQ(res, true);
}

TEST(ParserTest, for_stmt) {
    bool res = TestParserWithContent("{int a=0,b=1;for(int i=0;i<=100;i=i+1){a=a+b;}}",
                                     "{int a=0;int b=1;for(int i=0;i<=100;i=i+1){a=a+b;};}");
    ASSERT_EQ(res, true);
}

TEST(ParserTest, break_stmt) {
    bool res = TestParserWithContent("{int a=0,b=1;for(int i=0;i<=100;i=i+1){if(a>20){break;}a=a+b;}}",
                                     "{int a=0;int b=1;for(int i=0;i<=100;i=i+1){if(a>20){break;};a=a+b;};}");
    ASSERT_EQ(res, true);
}

TEST(ParserTest, continue_stmt) {
    bool res = TestParserWithContent("{int a=0,b=1;for(int i=0;i<=100;i=i+1){if(a==20){continue;}a=a+b;}}",
                                     "{int a=0;int b=1;for(int i=0;i<=100;i=i+1){if(a==20){continue;};a=a+b;};}");
    ASSERT_EQ(res, true);
}

TEST(ParserTest, block_stmt) {
    bool res = TestParserWithContent("{int a;a+3;}", "{int a;a+3;}");
    ASSERT_EQ(res, true);
}

TEST(ParserTest, single_assign) {
    bool res = TestParserWithContent("{int a;a=3;}", "{int a;a=3;}");
    ASSERT_EQ(res, true);
}

TEST(ParserTest, assign_op) {
    bool res = TestParserWithContent("{int a;a+=3;a-=3;a*=3;a/=3;a%=3;a|=3;a^=3;a&=3;a<<=3;a>>=3;}",
                                     "{int a;a+=3;a-=3;a*=3;a/=3;a%=3;a|=3;a^=3;a&=3;a<<=3;a>>=3;}");
    ASSERT_EQ(res, true);
}

TEST(ParserTest, unary_op) {
    bool res = TestParserWithContent("{int a=0;int *p=&a;+a;-a;!a;~a;++a;--a;sizeof a;&a;*p;}",
                                     "{int a=0;int *p=&a;+a;-a;!a;~a;++a;--a;sizeof a;&a;*p;}");
    ASSERT_EQ(res, true);
}

TEST(ParserTest, tree_op) {
    bool res = TestParserWithContent("{int a=3;a>=3?a=5:-5;}", "{int a=3;a>=3?a=5:-5;}");
    ASSERT_EQ(res, true);
}

TEST(ParserTest, post_op) {
    bool res = TestParserWithContent("{int a=3;int *p=&a;p++;p--;*p++;*p--;++*p;--*p;}",
                                     "{int a=3;int *p=&a;p++;p--;*p++;*p--;++*p;--*p;}");
    ASSERT_EQ(res, true);
}

TEST(ParserTest, multi_point_op) {
    bool res = TestParserWithContent("{int a=3;int *p=&a;int **q=&p;--*p;}", "{int a=3;int *p=&a;int **q=&p;--*p;}");
    ASSERT_EQ(res, true);
}

TEST(ParserTest, sizeof_op) {
    bool res = TestParserWithContent("{int a=3;sizeof a;sizeof (a);sizeof(int);}", "{int a=3;sizeof a;sizeof a;sizeof (int );}");
    ASSERT_EQ(res, true);
}

TEST(ParserTest, assign_comma_op) {
    bool res = TestParserWithContent("{int a=3,b;a=3,b=4;}", "{int a=3;int b;a=3,b=4;}");
    ASSERT_EQ(res, true);
}

TEST(ParserTest, array_one) {
    bool res = TestParserWithContent("{int a[3];}", "{[3]int a;}");
    ASSERT_EQ(res, true);
}

TEST(ParserTest, array_two) {
    bool res = TestParserWithContent("{int a[3],b[5];}", "{[3]int a;[5]int b;}");
    ASSERT_EQ(res, true);
}

TEST(ParserTest, array_three) {
    bool res = TestParserWithContent("{int a[3][5][8];}", "{[3][5][8]int a;}");
    ASSERT_EQ(res, true);
}

TEST(ParserTest, array_four) {
    bool res = TestParserWithContent("{int** a[3][5][8];}", "{[3][5][8]int **a;}");
    ASSERT_EQ(res, true);
}

TEST(ParserTest, array_five) {
    bool res = TestParserWithContent("{int a[3],b[5][8];int *p[5];}", "{[3]int a;[5][8]int b;[5]int *p;}");
    ASSERT_EQ(res, true);
}

TEST(ParserTest, sizeof_array1) {
    bool res = TestParserWithContent("{sizeof (int [5]);}", "{sizeof ([5]int );}");
    ASSERT_EQ(res, true);
}

TEST(ParserTest, sizeof_array2) {
    bool res = TestParserWithContent("{sizeof (int* [5]);}", "{sizeof ([5]int *);}");
    ASSERT_EQ(res, true);
}

TEST(ParserTest, sizeof_array3) {
    bool res = TestParserWithContent("{sizeof (int* [5][3]);}", "{sizeof ([5][3]int *);}");
    ASSERT_EQ(res, true);
}

// TEST(ParserTest, post_arr_1) {
//     bool res = TestParserWithContent("{int a[3]; a[0] = 4;}", "{[3]int a;a[0]=4;}");
//     ASSERT_EQ(res, true);
// }

// TEST(ParserTest, arr_init1) {
//     bool res = TestParserWithContent("{int a[3]={1,2}; a[0] = 4;}", "{[3]int a=1,2;a[0]=4;}");
//     ASSERT_EQ(res, true);
// }