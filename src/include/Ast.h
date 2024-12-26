#pragma once
#ifndef _AST_H_
#define _AST_H_

#include "llvm/IR/Value.h"
#include <memory>
#include <vector>

class Program;
class Expr;
class BinaryExpr;
class FactorExpr;

/// @brief
class Visitor {
  public:
    virtual ~Visitor() {
    }
    virtual llvm::Value *VisitProgram(Program *program)          = 0;
    virtual llvm::Value *VisitBinaryExpr(BinaryExpr *binaryExpr) = 0;
    virtual llvm::Value *VisitFactorExpr(FactorExpr *factorExpr) = 0;
};

enum class OpCode {
    Add = 0,
    Sub,
    Mul,
    Div,
};

class Expr {
  public:
    Expr() {
    }
    virtual ~Expr() {
    }
    virtual llvm::Value *AcceptVisitor(Visitor *v) {
        return nullptr;
    }
};

class FactorExpr : public Expr {
  public:
    int number;

  public:
    FactorExpr(int num);
    llvm::Value *AcceptVisitor(Visitor *v) override {
        return v->VisitFactorExpr(this);
    }
};

class BinaryExpr : public Expr {
  public:
    OpCode op;
    std::shared_ptr<Expr> leftExpr;
    std::shared_ptr<Expr> rightExpr;

  public:
    BinaryExpr(std::shared_ptr<Expr> left, OpCode op, std::shared_ptr<Expr> right);
    llvm::Value *AcceptVisitor(Visitor *v) override {
        return v->VisitBinaryExpr(this);
    }
};

class Program {
  public:
    std::vector<std::shared_ptr<Expr>> exprs;

  public:
    Program(std::vector<std::shared_ptr<Expr>> exprs);
    llvm::Value *AcceptVisitor(Visitor *v) {
        return v->VisitProgram(this);
    }
};

#endif // _AST_H_