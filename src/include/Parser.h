#pragma once
#ifndef _PARSER_H_
#define _PARSER_H_

#include "Ast.h"
#include "Lexer.h"
#include "Sema.h"

/// @brief Syntax analyzer that uses recursive descent to parse input tokens into C language syntax
/// @details The current grammar rules are as follows:
/// +-----------------------------------------------------------------------------------+
/// | prog            : stmt*
/// | stmt            : decl-stmt | expr-stmt | null-stmt | if-stmt | block-stmt
/// | null-stmt       : ";"
/// | decl-stmt       : "int" identifier ("=" expr)? ("," identifier ("=" expr)?)* ";"
/// | expr-stmt       : expr ";"
/// | if-stmt         : "if" "(" expr ")" "{" stmt  "}" ("else" "{" stmt "}")?
/// | block-stmt      : "{" stmt* "}"
/// | expr            : assign-expr | equal-expr
/// | assign-expr     : identifier ("=" expr)+
/// | equal-expr      : relational-expr ( ("==" | "!=") relational-expr)*
/// | relational-expr : add-expr (( ">" |"<" | "<=" | ">=") add-expr)*
/// | add-expr        : mult-expr ( ("+" | "_") mult-expr)*
/// | mult-expr       : primary-expr ( ("*" | "/") primary-expr)*
/// | primary-expr    : identifier | number | "(" expr ")"
/// | number          : ([0-9])+
/// | identifier      : (a-zA-Z)(a-zA-Z0-9)*
/// +----------------------------------------------------------------------------------+
/// The grammar rules can also be referenced in bnf/bnf.txt
class Parser {
  public:
    Parser(Lexer &lex, Sema &sema);
    std::shared_ptr<Program> ParserProgram();

  private:
    Lexer &lexer;
    Sema &sema;
    Token token; ///< The current token

  private:
    std::shared_ptr<ASTNode> ParserStmt();
    std::shared_ptr<ASTNode> ParserDeclStmt();
    std::shared_ptr<ASTNode> ParserBlockStmt();
    std::shared_ptr<ASTNode> ParserExprStmt();
    std::shared_ptr<ASTNode> ParserIfStmt();
    std::shared_ptr<ASTNode> ParserExpr();
    std::shared_ptr<ASTNode> ParserAssignExpr();
    std::shared_ptr<ASTNode> ParserEqualExpr();
    std::shared_ptr<ASTNode> ParserRelationalExpr();
    std::shared_ptr<ASTNode> ParserAddExpr();
    std::shared_ptr<ASTNode> ParserMultExpr();
    std::shared_ptr<ASTNode> ParserPrimaryExpr();

  private:
    /// @brief Checks if the current token is the expected token without consuming it
    bool IsExcept(TokenType tokTy);

    /// @brief Consumes the current token if it matches the expected token type
    bool Consume(TokenType tokTy);

    /// @brief Advances to the next token in the input stream
    void Advance();

    Diagnostics &GetDiagnostics();
};

#endif // _PARSER_H_