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

llvm::Value *PrintVisitor::VisitDeclStmts(DeclStmts *declStmts) {
    for (auto node : declStmts->nodeVec) {
        node->AcceptVisitor(this);
    }
}

llvm::Value *PrintVisitor::VisitVariableDecl(VariableDecl *variableDecl) {
    if (variableDecl->cType == CType::getIntTy()) {
        llvm::outs() << "int "
                     << llvm::StringRef(variableDecl->token.ptr, variableDecl->token.length) << ";";
    }
    return nullptr;
}

llvm::Value *PrintVisitor::VisitIfStmt(IfStmt *ifStmt) {
    llvm::outs() << "if (";
    ifStmt->condExpr->AcceptVisitor(this);
    llvm::outs() << ") {";
    ifStmt->thenStmt->AcceptVisitor(this);
    if (ifStmt->elseStmt) {
        llvm::outs() << "} \nelse {";
        ifStmt->elseStmt->AcceptVisitor(this);
        llvm::outs() << "}\n";
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
    llvm::outs() << numberExpr->token.value << " ";
    return nullptr;
}

llvm::Value *PrintVisitor::VisitVariableAssessExpr(VariableAssessExpr *variableAssessExpr) {
    llvm::outs() << llvm::StringRef(variableAssessExpr->token.ptr,
                                    variableAssessExpr->token.length);
    return nullptr;
}

llvm::Value *PrintVisitor::VisitAssignExpr(AssignExpr *assignExpr) {
    assignExpr->leftExpr->AcceptVisitor(this);
    llvm::outs() << " = ";
    assignExpr->rightExpr->AcceptVisitor(this);
    return nullptr;
}
