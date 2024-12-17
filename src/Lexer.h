#ifndef _LEXER_H_
#define _LEXER_H_

#pragma once

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/raw_ostream.h"

enum class TokenType {
    Unknown = 0,
    Number,      ///< literal number
    Minus,       ///< -
    Plus,        ///< +
    Star,        ///< *
    Slash,       ///< /
    LeftParent,  ///< (
    RightParent, ///< )
    Semi,        ///< ;
    Eof          ///< end of file
};

/// @brief Represents a token with its position, type, and value
/// @details The Token class stores information about a specific token including
/// its position (row and column), the type of the token (TokenType), and an
/// associated value. This class is used for lexical analysis in a compiler.
class Token {
  public:
    TokenType tokenTy;
    int row;
    int col;   ///< The line and column number of the token in the source code.
    int value; ///< The value of the token, used when the TokenType is 'number'.
    llvm::StringRef
        content; ///< The textual content of the token in the source code.

  public:
    Token() {
        row     = -1;
        col     = -1;
        tokenTy = TokenType::Unknown;
        value   = 0;
    }

    void setMember(TokenType tokTy, const char *pos, int len, int val = 0) {
        tokenTy = tokTy;
        content = llvm::StringRef(pos, len);
        value   = val;
    }

    void Dump() {
        llvm::outs() << "[ row = " << row << ", col = " << col << " ]\n";
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
