#pragma once
#ifndef _LEXER_H_
#define _LEXER_H_

#include "CType.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/raw_ostream.h"

enum class TokenType {
    Unknown = 0,
    Number,      ///< literal number
    Equal,       ///< =
    Minus,       ///< -
    Plus,        ///< +
    Star,        ///< *
    Slash,       ///< /
    LeftParent,  ///< (
    RightParent, ///< )
    Comma,       ///< ,
    Semi,        ///< ;
    Identifier,  ///< variable name
    KW_int,      ///< int
    Eof          ///< end of file
};

/// @brief Represents a token with its position, type, and value
/// @details The Token class stores information about a specific token including its position (row
/// and column), the type of the token (TokenType), and an associated value. This class is used for
/// lexical analysis in a compiler.
class Token {
  public:
    TokenType tokenTy;
    CType *cType;
    int row;
    int col;                 ///< The line and column number of the token in the source code.
    int value;               ///< The value of the token, used when the TokenType is 'number'.
    llvm::StringRef content; ///< The textual content of the token in the source code.

  public:
    Token() {
        row     = -1;
        col     = -1;
        tokenTy = TokenType::Unknown;
        value   = 0;
    }

    void setMember(TokenType tokTy, const char *pos, int len, int val = 0, CType *cTy = nullptr) {
        tokenTy = tokTy;
        content = llvm::StringRef(pos, len);
        value   = val;
        cType   = cTy;
    }

    void Dump() {
        llvm::outs() << "[ " << content << ", row = " << row << ", col = " << col << " ]\n";
    }
};

/// @brief Represents a lexer that tokenizes source code into a sequence of tokens
/// @details The `Lexer` class is responsible for tokenizing the input source code. It processes the
/// source code character by character, recognizing patterns such as keywords, operators, literals,
/// and identifiers, and generates the corresponding tokens. The class also tracks the current
/// position in the source code (row and column) to aid in error reporting and debugging. This class
/// is an essential component of the lexical analysis phase in a compiler, where the source code is
/// divided into meaningful symbols for further parsing and compilation.
class Lexer {
  public:
    Lexer(llvm::StringRef sourceCode);
    void NextToken(Token &tok);
    void Run(Token &tok);

  private:
    const char *workPtr;        ///< Pointer to the current character in the source
                                ///< code being scanned
    const char *workRowHeadPtr; ///< Pointer to the start of the current line in
                                ///< the source code being scanned
    const char *eofPtr;         ///< Pointer to the end-of-file in the source code being scanned
    int workRow;                ///< Line number in the source code currently being scanned

  private:
    void KeyWordHandle(Token &tok);
    bool IsWhiteSpace(char ch);
    bool IsDigit(char ch);
    bool IsLetter(char ch);
};

#endif // _LEXER_H_
