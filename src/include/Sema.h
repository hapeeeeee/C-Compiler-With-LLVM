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
    // std::shared_ptr<ASTNode> SemaBinaryExprNode(VariableDecl);
};

#endif // _SEMA_H_