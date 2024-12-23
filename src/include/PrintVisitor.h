#pragma once
#ifndef _PRINTVISITOR_H_
#define _PRINTVISITOR_H_
#include "Ast.h"

class PrintVisitor : public Visitor {
  public:
    PrintVisitor(std::shared_ptr<Program> program);
    void VisitProgram(Program *program) override;
    void VisitBinaryExpr(BinaryExpr *binaryExpr) override;
    void VisitFactorExpr(FactorExpr *factorExpr) override;
};

#endif // _PRINTVISITOR_H_