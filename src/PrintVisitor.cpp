#include "include/PrintVisitor.h"
#include "llvm/Support/raw_ostream.h"

PrintVisitor::PrintVisitor(std::shared_ptr<Program> program) {
    VisitProgram(program.get());
}

llvm::Value *PrintVisitor::VisitProgram(Program *program) {
    llvm::outs() << "Program :\n--------------------\n\n";
    for (std::shared_ptr<ASTNode> &stmt : program->stmts) {
        stmt->AcceptVisitor(this);
        llvm::outs() << "\n";
    }
    llvm::outs() << "\n-----------------------------\n";
    return nullptr;
}

llvm::Value *PrintVisitor::VisitVariableDecl(VariableDecl *variableDecl) {
    if (variableDecl->cType == CType::getIntTy()) {
        llvm::outs() << "int " << variableDecl->name << ";";
    }

    return nullptr;
}

llvm::Value *PrintVisitor::VisitBinaryExpr(BinaryExpr *binaryExpr) {
    binaryExpr->leftExpr->AcceptVisitor(this);

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

    binaryExpr->rightExpr->AcceptVisitor(this);
    return nullptr;
}

llvm::Value *PrintVisitor::VisitNumberExpr(NumberExpr *numberExpr) {
    llvm::outs() << numberExpr->number << " ";
    return nullptr;
}

llvm::Value *PrintVisitor::VisitVariableAssessExpr(VariableAssessExpr *variableAssessExpr) {
    llvm::outs() << variableAssessExpr->name;
    return nullptr;
}

llvm::Value *PrintVisitor::VisitAssignExpr(AssignExpr *assignExpr) {
    assignExpr->leftExpr->AcceptVisitor(this);
    llvm::outs() << " = ";
    assignExpr->rightExpr->AcceptVisitor(this);
    return nullptr;
}