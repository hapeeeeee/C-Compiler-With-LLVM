#pragma once
#ifndef _SEMA_H_
#define _SEMA_H_

#include "Ast.h"
#include "Lexer.h"
#include "Scope.h"

class Sema {
  private:
    Scope scope;

  public:
    std::shared_ptr<ASTNode> SemaVariableDeclNode(CType *cType, Token &tok);
    std::shared_ptr<ASTNode> SemaAssignExprNode(std::shared_ptr<ASTNode> left,
                                                std::shared_ptr<ASTNode> right);
    std::shared_ptr<ASTNode> SemaVariableAccessExprNode(CType *cType, Token &tok);
    std::shared_ptr<ASTNode>
    SemaBinaryExprNode(std::shared_ptr<ASTNode> left, OpCode op, std::shared_ptr<ASTNode> right);
};

#endif // _SEMA_H_