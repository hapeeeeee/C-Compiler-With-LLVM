#pragma once
#ifndef _AST_H_
#define _AST_H_

#include <memory>
#include <vector>
enum class OpCode {
    add = 0,
    sub,
    mul,
    div,
};

class Expr {
  public:
    Expr();
    virtual ~Expr() {
    }
};

class Factor : public Expr {
  public:
    int number;

  public:
    Factor(int num);
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