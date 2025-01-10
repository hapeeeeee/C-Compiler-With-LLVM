#include "Lexer.h"
#include <gtest/gtest.h>

class LexerTest : public ::testing::Test {
  public:
    Lexer *lexer;

  public:
    void SetUp() override {
        static llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> buf =
            llvm::MemoryBuffer::getFile("/home/zax/project/CC_LLVM/unittest/testset/lexer_01.cpp");

        if (!buf) {
            llvm::errs() << "can't open file.\n";
            return;
        }

        llvm::SourceMgr mgr;
        Diagnostics diager(mgr);
        mgr.AddNewSourceBuffer(std::move(*buf), llvm::SMLoc());

        lexer = new Lexer(mgr, diager);
    }

    void TearDown() override {
        delete lexer;
    }
};

/// @brief test NextToken for Lexer
/// @details int aa, b = 4; aa = 1;
TEST_F(LexerTest, NextToken) {
    std::vector<Token> expectedVec, curVec;
    expectedVec.push_back(Token{TokenType::KW_int, 1, 1});
    expectedVec.push_back(Token{TokenType::Identifier, 1, 5});
    expectedVec.push_back(Token{TokenType::Comma, 1, 7});
    expectedVec.push_back(Token{TokenType::Identifier, 1, 9});
    expectedVec.push_back(Token{TokenType::Equal, 1, 11});
    expectedVec.push_back(Token{TokenType::Number, 1, 13});
    expectedVec.push_back(Token{TokenType::Semi, 1, 14});
    expectedVec.push_back(Token{TokenType::Identifier, 2, 1});
    expectedVec.push_back(Token{TokenType::Equal, 2, 4});
    expectedVec.push_back(Token{TokenType::Number, 2, 6});
    expectedVec.push_back(Token{TokenType::Semi, 2, 7});

    Token tok;
    while (true) {
        lexer->NextToken(tok);
        if (tok.tokenTy == TokenType::Eof) {
            break;
        }
        curVec.push_back(tok);
    }
    ASSERT_EQ(expectedVec.size(), curVec.size());

    for (int i = 0; i < expectedVec.size(); i++) {
        auto &exceptdTok = expectedVec[0];
        auto &currTok    = curVec[0];
        EXPECT_EQ(exceptdTok.tokenTy, currTok.tokenTy);
        EXPECT_EQ(exceptdTok.row, currTok.row);
        EXPECT_EQ(exceptdTok.col, currTok.col);
    }
}
