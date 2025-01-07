#pragma once
#ifndef _SEMA_H_
#define _SEMA_H_

#include "Ast.h"
#include "Lexer.h"
#include "Scope.h"

/// @brief Performs semantic analysis for the program.
/// @details The `Sema` class is responsible for performing semantic analysis on Abstract Syntax
/// Tree (AST) nodes. It validates and processes various constructs in the program, such as variable
/// declarations, expressions, and operations. It ensures that the program adheres to semantic rules
/// and prepares the AST for further compilation stages.
class Sema {
  private:
    Scope scope;

  public:
    std::shared_ptr<ASTNode> SemaVariableDeclNode(CType *cType, Token &tok);
    std::shared_ptr<ASTNode> SemaAssignExprNode(std::shared_ptr<ASTNode> left,
                                                std::shared_ptr<ASTNode> right);
    std::shared_ptr<ASTNode> SemaVariableAccessExprNode(Token &tok);
    std::shared_ptr<ASTNode>
    SemaBinaryExprNode(std::shared_ptr<ASTNode> left, OpCode op, std::shared_ptr<ASTNode> right);
    std::shared_ptr<ASTNode> SemaNumberExprNode(CType *cType, Token &tok);
};

#endif // _SEMA_H_