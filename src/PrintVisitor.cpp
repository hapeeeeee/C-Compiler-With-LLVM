#include "include/PrintVisitor.h"
#include "llvm/Support/raw_ostream.h"

PrintVisitor::PrintVisitor(std::shared_ptr<Program> program) {
    VisitProgram(program.get());
}

llvm::Value *PrintVisitor::VisitProgram(Program *program) {
    llvm::outs() << "Program :\n--------------------\n\n";
    for (std::shared_ptr<Expr> &expr : program->exprs) {
        expr->AcceptVisitor(this);
        llvm::outs() << "\n";
    }
    llvm::outs() << "\n-----------------------------\n";
    return nullptr;
}

llvm::Value *PrintVisitor::VisitBinaryExpr(BinaryExpr *binaryExpr) {
    binaryExpr->leftExpr->AcceptVisitor(this);
    binaryExpr->rightExpr->AcceptVisitor(this);

    switch (binaryExpr->op) {
    case OpCode::Add:
        llvm::outs() << "+";
        break;
    case OpCode::Sub:
        llvm::outs() << "-";
        break;
    case OpCode::Mul:
        llvm::outs() << "*";
        break;
    case OpCode::Div:
        llvm::outs() << "/";
        break;
    default:
        break;
    }
    llvm::outs() << " ";
    return nullptr;
}

llvm::Value *PrintVisitor::VisitFactorExpr(FactorExpr *factorExpr) {
    llvm::outs() << factorExpr->number << " ";
    return nullptr;
}