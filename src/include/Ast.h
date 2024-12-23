#pragma once
#ifndef _AST_H_
#define _AST_H_

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
    virtual void VisitProgram(Program *program)          = 0;
    virtual void VisitBinaryExpr(BinaryExpr *binaryExpr) = 0;
    virtual void VisitFactorExpr(FactorExpr *factorExpr) = 0;
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
    virtual void AcceptVisitor(Visitor *v) {
    }
};

class FactorExpr : public Expr {
  public:
    int number;

  public:
    FactorExpr(int num);
    void AcceptVisitor(Visitor *v) override {
        v->VisitFactorExpr(this);
    }
};

class BinaryExpr : public Expr {
  public:
    OpCode op;
    std::shared_ptr<Expr> leftExpr;
    std::shared_ptr<Expr> rightExpr;

  public:
    BinaryExpr(std::shared_ptr<Expr> left, OpCode op, std::shared_ptr<Expr> right);
    void AcceptVisitor(Visitor *v) override {
        v->VisitBinaryExpr(this);
    }
};

class Program {
  public:
    std::vector<std::shared_ptr<Expr>> exprs;

  public:
    Program(std::vector<std::shared_ptr<Expr>> exprs);
    void AcceptVisitor(Visitor *v) {
        v->VisitProgram(this);
    }
};

#endif // _AST_H_