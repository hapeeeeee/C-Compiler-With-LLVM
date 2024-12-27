#pragma once
#ifndef _PRINTVISITOR_H_
#define _PRINTVISITOR_H_
#include "Ast.h"

class PrintVisitor : public Visitor {
  public:
    PrintVisitor(std::shared_ptr<Program> program);
    llvm::Value *VisitProgram(Program *program) override;
    llvm::Value *VisitBinaryExpr(BinaryExpr *binaryExpr) override;
    llvm::Value *VisitFactorExpr(FactorExpr *factorExpr) override;
};

#endif // _PRINTVISITOR_H_