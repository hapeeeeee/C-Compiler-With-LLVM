#pragma once
#ifndef _PRINTVISITOR_H_
#define _PRINTVISITOR_H_
#include "Ast.h"

class PrintVisitor : public Visitor {
  public:
    PrintVisitor(std::shared_ptr<Program> program);
    llvm::Value *VisitProgram(Program *program) override;
    llvm::Value *VisitDeclStmts(DeclStmts *declStmts) override;
    llvm::Value *VisitVariableDecl(VariableDecl *VariableDecl) override;
    llvm::Value *VisitIfStmt(IfStmt *ifStmt) override;
    llvm::Value *VisitBinaryExpr(BinaryExpr *binaryExpr) override;
    llvm::Value *VisitNumberExpr(NumberExpr *numberExpr) override;
    llvm::Value *VisitVariableAssessExpr(VariableAssessExpr *variableAssessExpr) override;
    llvm::Value *VisitAssignExpr(AssignExpr *assignExpr) override;
};

#endif // _PRINTVISITOR_H_
