#pragma
#ifndef _PARSER_H_
#define _PARSER_H_

#include "Ast.h"
#include "Lexer.h"

/// @brief Syntax analyzer that uses recursive descent to parse input tokens into C language syntax
/// @details The current grammar rules are as follows:
/// +-----------------------------------------+
/// | prog   : (expr? ";")*                   |
/// | expr   : term(("+" | "-") term)* ;      |
/// | term   : factor(("*" | "/") factor)* ;  |
/// | factor : number | "(" expr ")" ;        |
/// | number : ([0-9])+ ;                     |
/// +-----------------------------------------+
/// The grammar rules can also be referenced in bnf/bnf.txt
class Parser {
  public:
    Parser(Lexer &lex);
    std::shared_ptr<Program> ParserProgram();

  private:
    Lexer &lexer;
    Token token; ///< The current token

  private:
    std::shared_ptr<Expr> ParserExpr();
    std::shared_ptr<Expr> ParserTerm();
    std::shared_ptr<Expr> ParserFactor();

  private:
    /// @brief Checks if the current token is the expected token without consuming it
    bool IsExcept(TokenType tokTy);

    /// @brief Consumes the current token if it matches the expected token type
    bool Consume(TokenType tokTy);

    /// @brief Advances to the next token in the input stream
    void Advance();
};

#endif // _PARSER_H_