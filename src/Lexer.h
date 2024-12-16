#ifndef _LEXER_H_
#define _LEXER_H_

#pragma once

#include "llvm/ADT/StringRef.h"

enum class TokenType {
    Unknown = 0,
    Number,      ///< 2
    Minus,       ///< -
    Plus,        ///< +
    Star,        ///< *
    Slash,       ///< /
    LeftParent,  ///< {
    RightParent, ///< }
    Semi,        ///< ;
    Eof          ///< end of file
};

/// @brief Represents a token with its position, type, and value
/// @details The Token class stores information about a specific token including
/// its position (row and column), the type of the token (TokenType), and an
/// associated value. This class is used for lexical analysis in a compiler.
class Token {
  public:
    int row, col;
    TokenType tokenTy;
    int value;

    Token() {
        row = col = -1;
        tokenTy   = TokenType::Unknown;
        value     = __INT_MAX__;
    }
};

class Lexer {
  public:
    Lexer(llvm::StringRef sourceCode);

  public:
    void NextToken(Token &tok);

  private:
    const char *workPtr;
    const char *workRowHeadPtr;
    const char *eofPtr;

    int workRow;

  private:
    bool IsWhiteSpace(char ch);
    bool IsDigit(char ch);
};

#endif // _LEXER_H_
