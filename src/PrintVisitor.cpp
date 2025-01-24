#include "include/PrintVisitor.h"
#include "llvm/Support/raw_ostream.h"

PrintVisitor::PrintVisitor(std::shared_ptr<Program> program) {
    VisitProgram(program.get());
}

llvm::Value *PrintVisitor::VisitProgram(Program *program) {
    llvm::outs() << "Program :\n--------------------\n\n";
    // for (std::shared_ptr<ASTNode> &stmt : program->stmts) {
    //     stmt->AcceptVisitor(this);
    //     llvm::outs() << "\n";
    // }
    program->node->AcceptVisitor(this);
    llvm::outs() << "\n-----------------------------\n";
    return nullptr;
}

llvm::Value *PrintVisitor::VisitDeclStmts(DeclStmts *declStmts) {
    for (auto node : declStmts->nodeVec) {
        node->AcceptVisitor(this);
        llvm::outs() << "\n";
    }
    return nullptr;
}

llvm::Value *PrintVisitor::VisitVariableDecl(VariableDecl *variableDecl) {
    variableDecl->cType->AcceptVisitor(this);
    llvm::outs() << llvm::StringRef(variableDecl->token.ptr, variableDecl->token.length);
    if (variableDecl->initNode) {
        llvm::outs() << " = ";
        variableDecl->initNode->AcceptVisitor(this);
    }
    return nullptr;
}

llvm::Value *PrintVisitor::VisitBlockStmts(BlockStmts *blockStmts) {
    llvm::outs() << "{\n";
    for (auto node : blockStmts->nodeVec) {
        llvm::outs() << "  ";
        node->AcceptVisitor(this);
        llvm::outs() << "\n";
    }
    llvm::outs() << "}\n";
    return nullptr;
}

llvm::Value *PrintVisitor::VisitIfStmt(IfStmt *ifStmt) {
    llvm::outs() << "if (";
    ifStmt->condExpr->AcceptVisitor(this);
    llvm::outs() << ")";
    ifStmt->thenStmt->AcceptVisitor(this);
    if (ifStmt->elseStmt) {
        llvm::outs() << " \nelse ";
        ifStmt->elseStmt->AcceptVisitor(this);
        llvm::outs() << "\n";
    }
    return nullptr;
}

llvm::Value *PrintVisitor::VisitForStmt(ForStmt *forStmt) {
    llvm::outs() << "for ( ";
    if (forStmt->initNode) {
        forStmt->initNode->AcceptVisitor(this);
    }
    llvm::outs() << "; ";
    if (forStmt->condNode) {
        forStmt->condNode->AcceptVisitor(this);
    }
    llvm::outs() << "; ";
    if (forStmt->thenNode) {
        forStmt->thenNode->AcceptVisitor(this);
    }
    llvm::outs() << ") ";
    if (forStmt->bodyNode) {
        forStmt->bodyNode->AcceptVisitor(this);
    }
    return nullptr;
}

llvm::Value *PrintVisitor::VisitBreakStmt(BreakStmt *breakStmt) {
    llvm::outs() << "break";
    return nullptr;
}

llvm::Value *PrintVisitor::VisitContinueStmt(ContinueStmt *continueStmtStmt) {
    llvm::outs() << "continue";
    return nullptr;
}

llvm::Value *PrintVisitor::VisitSizeofExpr(SizeofExpr *sizeofExpr) {
    llvm::outs() << "sizeof";
    if (sizeofExpr->sizeofTY) {
        llvm::outs() << "(";
        sizeofExpr->sizeofTY->AcceptVisitor(this);
        llvm::outs() << ")";
    } else {
        sizeofExpr->expr->AcceptVisitor(this);
    }

    return nullptr;
}

llvm::Value *PrintVisitor::VisitUnaryExpr(UnaryExpr *unaryExpr) {
    switch (unaryExpr->op) {
    case UnaryOpCode::Positive: {
        llvm::outs() << "+";
        break;
    }
    case UnaryOpCode::Negative: {
        llvm::outs() << "-";
        break;
    }
    case UnaryOpCode::Deref: {
        llvm::outs() << "*";
        break;
    }
    case UnaryOpCode::Addr: {
        llvm::outs() << "&";
        break;
    }
    case UnaryOpCode::Inc: {
        llvm::outs() << "++";
        break;
    }
    case UnaryOpCode::Dec: {
        llvm::outs() << "--";
        break;
    }
    case UnaryOpCode::LogicNot: {
        llvm::outs() << "!";
        break;
    }
    case UnaryOpCode::BitNot: {
        llvm::outs() << "~";
        break;
    }
    }

    unaryExpr->expr->AcceptVisitor(this);
    return nullptr;
}

llvm::Value *PrintVisitor::VisitBinaryExpr(BinaryExpr *binaryExpr) {
    binaryExpr->leftExpr->AcceptVisitor(this);

    switch (binaryExpr->op) {
    case BinOpCode::Add:
        llvm::outs() << "+";
        break;
    case BinOpCode::Sub:
        llvm::outs() << "-";
        break;
    case BinOpCode::Mul:
        llvm::outs() << "*";
        break;
    case BinOpCode::Div:
        llvm::outs() << "/";
        break;
    case BinOpCode::Mod:
        llvm::outs() << "%";
        break;
    case BinOpCode::LeftShift:
        llvm::outs() << "<<";
        break;
    case BinOpCode::RightShift:
        llvm::outs() << ">>";
        break;
    case BinOpCode::EqualEqual:
        llvm::outs() << "==";
        break;
    case BinOpCode::NotEqual:
        llvm::outs() << "!=";
        break;
    case BinOpCode::Less:
        llvm::outs() << "<";
        break;
    case BinOpCode::Greater:
        llvm::outs() << ">";
        break;
    case BinOpCode::LessEqual:
        llvm::outs() << "<=";
        break;
    case BinOpCode::GreaterEqual:
        llvm::outs() << ">=";
        break;
    case BinOpCode::LogicOr:
        llvm::outs() << "||";
        break;
    case BinOpCode::LogicAnd:
        llvm::outs() << "&&";
        break;
    case BinOpCode::BitOr:
        llvm::outs() << "|";
        break;
    case BinOpCode::BitXor:
        llvm::outs() << "^";
        break;
    case BinOpCode::BitAnd:
        llvm::outs() << "&";
        break;
    case BinOpCode::Comma: {
        llvm::outs() << ",";
        break;
    }
    case BinOpCode::Assign: {
        llvm::outs() << "=";
        break;
    }
    case BinOpCode::AddAssign: {
        llvm::outs() << "+=";
        break;
    }
    case BinOpCode::SubAssign: {
        llvm::outs() << "-=";
        break;
    }
    case BinOpCode::MulAssign: {
        llvm::outs() << "*=";
        break;
    }
    case BinOpCode::DivAssign: {
        llvm::outs() << "/=";
        break;
    }
    case BinOpCode::ModAssign: {
        llvm::outs() << "%=";
        break;
    }
    case BinOpCode::OrAssign: {
        llvm::outs() << "|=";
        break;
    }
    case BinOpCode::AndAssign: {
        llvm::outs() << "&=";
        break;
    }
    case BinOpCode::XorAssign: {
        llvm::outs() << "^=";
        break;
    }
    case BinOpCode::LeftShiftAssign: {
        llvm::outs() << "<<=";
        break;
    }
    case BinOpCode::RightShiftAssign: {
        llvm::outs() << ">>=";
        break;
    }
    case BinOpCode::AddAdd: {
        llvm::outs() << "++";
        break;
    }
    case BinOpCode::SubSub: {
        llvm::outs() << "--";
        break;
    }
    default:
        break;
    }
    llvm::outs() << " ";

    binaryExpr->rightExpr->AcceptVisitor(this);
    return nullptr;
}

llvm::Value *PrintVisitor::VisitThreeExpr(ThreeExpr *threeExpr) {
    threeExpr->condExpr->AcceptVisitor(this);
    llvm::outs() << "? ";
    threeExpr->trueExpr->AcceptVisitor(this);
    llvm::outs() << ": ";
    threeExpr->falseExpr->AcceptVisitor(this);
    llvm::outs() << ";";

    return nullptr;
}

llvm::Value *PrintVisitor::VisitPostIncExpr(PostIncExpr *postIncExpr) {
    postIncExpr->leftNode->AcceptVisitor(this);
    llvm::outs() << "++";
    return nullptr;
}

llvm::Value *PrintVisitor::VisitPostDecExpr(PostDecExpr *postDecExpr) {
    postDecExpr->leftNode->AcceptVisitor(this);
    llvm::outs() << "--";
    return nullptr;
}

llvm::Value *PrintVisitor::VisitNumberExpr(NumberExpr *numberExpr) {
    llvm::outs() << numberExpr->token.value << " ";
    return nullptr;
}

llvm::Value *PrintVisitor::VisitVariableAssessExpr(VariableAssessExpr *variableAssessExpr) {
    llvm::outs() << llvm::StringRef(variableAssessExpr->token.ptr, variableAssessExpr->token.length);
    return nullptr;
}

llvm::Type *PrintVisitor::VisitCPrimaryType(CPrimaryType *ty) {
    if (ty->GetTypeKind() == CType::CTypeKind::TY_Int) {
        llvm::outs() << "int ";
    }
    return nullptr;
}

llvm::Type *PrintVisitor::VisitCPointType(CPointType *ty) {
    ty->GetBaseType()->AcceptVisitor(this);
    llvm::outs() << "*";
    return nullptr;
}
