#include "include/Ast.h"

FactorExpr::FactorExpr(int num) : number(num) {
}

BinaryExpr::BinaryExpr(std::shared_ptr<Expr> left, OpCode op, std::shared_ptr<Expr> right)
    : leftExpr(left), op(op), rightExpr(right) {
}

Program::Program(std::vector<std::shared_ptr<Expr>> exprs) : exprs(exprs) {
}