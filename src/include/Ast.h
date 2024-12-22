#pragma once
#ifndef _AST_H_
#define _AST_H_

#include <memory>
#include <vector>
enum class OpCode {
    Add = 0,
    Sub,
    Mul,
    Div,
};

class Expr {
  public:
    Expr();
    virtual ~Expr() {
    }
};

class FactorExpr : public Expr {
  public:
    int number;

  public:
    FactorExpr(int num);
};

class BinaryExpr : public Expr {
  public:
    OpCode op;
    std::shared_ptr<Expr> leftExpr;
    std::shared_ptr<Expr> rightExpr;

  public:
    BinaryExpr(std::shared_ptr<Expr> left, OpCode op, std::shared_ptr<Expr> right);
};

class Program {
  public:
    std::vector<std::shared_ptr<Expr>> exprs;

  public:
    Program(std::vector<std::shared_ptr<Expr>> exprs);
};

#endif // _AST_H_