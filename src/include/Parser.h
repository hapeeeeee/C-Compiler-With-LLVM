#pragma once
#ifndef _PARSER_H_
#define _PARSER_H_

#include "Ast.h"
#include "Lexer.h"

/// @brief Syntax analyzer that uses recursive descent to parse input tokens into C language syntax
/// @details The current grammar rules are as follows:
/// +-----------------------------------------------------+
/// |prog       : (decl-stmt | expr-stmt )*               |
/// |decl-stam  : type-decl identifier ("=" expr)*  ";"   |
/// |type-decl  : "int"                                   |
/// |expr-stmt  : expr? ";"                               |
/// |expr       : term(("+" | "-") term)*                 |
/// |term       : factor(("*" | "/") factor)*             |
/// |factor     : identifier | number | "(" expr")"       |
/// |number     : ([0-9])+                                |
/// |identifier : (a-zA-Z_)(a-zA-Z0-9_)*                  |
/// +-----------------------------------------------------+
/// The grammar rules can also be referenced in bnf/bnf.txt
class Parser {
  public:
    Parser(Lexer &lex);
    std::shared_ptr<Program> ParserProgram();

  private:
    Lexer &lexer;
    Token token; ///< The current token

  private:
    std::vector<std::shared_ptr<ASTNode>> ParserDecl();
    std::shared_ptr<ASTNode> ParserExpr();
    std::shared_ptr<ASTNode> ParserTerm();
    std::shared_ptr<ASTNode> ParserFactor();

  private:
    /// @brief Checks if the current token is the expected token without consuming it
    bool IsExcept(TokenType tokTy);

    /// @brief Consumes the current token if it matches the expected token type
    bool Consume(TokenType tokTy);

    /// @brief Advances to the next token in the input stream
    void Advance();
};

#endif // _PARSER_H_