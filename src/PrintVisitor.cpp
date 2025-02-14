#include "include/PrintVisitor.h"
#include "llvm/Support/raw_ostream.h"

PrintVisitor::PrintVisitor(std::shared_ptr<Program> program, llvm::raw_ostream *out) : out(out) {
    VisitProgram(program.get());
}

llvm::Value *PrintVisitor::VisitProgram(Program *program) {
    program->node->AcceptVisitor(this);
    return nullptr;
}

llvm::Value *PrintVisitor::VisitDeclStmts(DeclStmts *declStmts) {
    int i = 0, size = declStmts->nodeVec.size();
    for (auto node : declStmts->nodeVec) {
        node->AcceptVisitor(this);
        if (i != size - 1) {
            *out << ";";
        }
        ++i;
    }
    return nullptr;
}

llvm::Value *PrintVisitor::VisitVariableDecl(VariableDecl *variableDecl) {
    variableDecl->cType->AcceptVisitor(this);
    *out << llvm::StringRef(variableDecl->token.ptr, variableDecl->token.length);
    if (variableDecl->initNode) {
        *out << "=";
        variableDecl->initNode->AcceptVisitor(this);
    }
    return nullptr;
}

llvm::Value *PrintVisitor::VisitBlockStmts(BlockStmts *blockStmts) {
    *out << "{";
    for (auto node : blockStmts->nodeVec) {
        node->AcceptVisitor(this);
        *out << ";";
    }
    *out << "}";
    return nullptr;
}

llvm::Value *PrintVisitor::VisitIfStmt(IfStmt *ifStmt) {
    *out << "if(";
    ifStmt->condExpr->AcceptVisitor(this);
    *out << ")";
    ifStmt->thenStmt->AcceptVisitor(this);
    if (ifStmt->elseStmt) {
        *out << "else";
        ifStmt->elseStmt->AcceptVisitor(this);
    }
    return nullptr;
}

llvm::Value *PrintVisitor::VisitForStmt(ForStmt *forStmt) {
    *out << "for(";
    if (forStmt->initNode) {
        forStmt->initNode->AcceptVisitor(this);
    }
    *out << ";";
    if (forStmt->condNode) {
        forStmt->condNode->AcceptVisitor(this);
    }
    *out << ";";
    if (forStmt->thenNode) {
        forStmt->thenNode->AcceptVisitor(this);
    }
    *out << ")";
    if (forStmt->bodyNode) {
        forStmt->bodyNode->AcceptVisitor(this);
    }
    return nullptr;
}

llvm::Value *PrintVisitor::VisitBreakStmt(BreakStmt *breakStmt) {
    *out << "break";
    return nullptr;
}

llvm::Value *PrintVisitor::VisitContinueStmt(ContinueStmt *continueStmtStmt) {
    *out << "continue";
    return nullptr;
}

llvm::Value *PrintVisitor::VisitSizeofExpr(SizeofExpr *sizeofExpr) {
    *out << "sizeof ";
    if (sizeofExpr->sizeofTY) {
        *out << "(";
        sizeofExpr->sizeofTY->AcceptVisitor(this);
        *out << ")";
    } else {
        sizeofExpr->expr->AcceptVisitor(this);
    }

    return nullptr;
}

llvm::Value *PrintVisitor::VisitUnaryExpr(UnaryExpr *unaryExpr) {
    switch (unaryExpr->op) {
    case UnaryOpCode::Positive: {
        *out << "+";
        break;
    }
    case UnaryOpCode::Negative: {
        *out << "-";
        break;
    }
    case UnaryOpCode::Deref: {
        *out << "*";
        break;
    }
    case UnaryOpCode::Addr: {
        *out << "&";
        break;
    }
    case UnaryOpCode::Inc: {
        *out << "++";
        break;
    }
    case UnaryOpCode::Dec: {
        *out << "--";
        break;
    }
    case UnaryOpCode::LogicNot: {
        *out << "!";
        break;
    }
    case UnaryOpCode::BitNot: {
        *out << "~";
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
        *out << "+";
        break;
    case BinOpCode::Sub:
        *out << "-";
        break;
    case BinOpCode::Mul:
        *out << "*";
        break;
    case BinOpCode::Div:
        *out << "/";
        break;
    case BinOpCode::Mod:
        *out << "%";
        break;
    case BinOpCode::LeftShift:
        *out << "<<";
        break;
    case BinOpCode::RightShift:
        *out << ">>";
        break;
    case BinOpCode::EqualEqual:
        *out << "==";
        break;
    case BinOpCode::NotEqual:
        *out << "!=";
        break;
    case BinOpCode::Less:
        *out << "<";
        break;
    case BinOpCode::Greater:
        *out << ">";
        break;
    case BinOpCode::LessEqual:
        *out << "<=";
        break;
    case BinOpCode::GreaterEqual:
        *out << ">=";
        break;
    case BinOpCode::LogicOr:
        *out << "||";
        break;
    case BinOpCode::LogicAnd:
        *out << "&&";
        break;
    case BinOpCode::BitOr:
        *out << "|";
        break;
    case BinOpCode::BitXor:
        *out << "^";
        break;
    case BinOpCode::BitAnd:
        *out << "&";
        break;
    case BinOpCode::Comma: {
        *out << ",";
        break;
    }
    case BinOpCode::Assign: {
        *out << "=";
        break;
    }
    case BinOpCode::AddAssign: {
        *out << "+=";
        break;
    }
    case BinOpCode::SubAssign: {
        *out << "-=";
        break;
    }
    case BinOpCode::MulAssign: {
        *out << "*=";
        break;
    }
    case BinOpCode::DivAssign: {
        *out << "/=";
        break;
    }
    case BinOpCode::ModAssign: {
        *out << "%=";
        break;
    }
    case BinOpCode::OrAssign: {
        *out << "|=";
        break;
    }
    case BinOpCode::AndAssign: {
        *out << "&=";
        break;
    }
    case BinOpCode::XorAssign: {
        *out << "^=";
        break;
    }
    case BinOpCode::LeftShiftAssign: {
        *out << "<<=";
        break;
    }
    case BinOpCode::RightShiftAssign: {
        *out << ">>=";
        break;
    }
    default:
        break;
    }
    binaryExpr->rightExpr->AcceptVisitor(this);
    return nullptr;
}

llvm::Value *PrintVisitor::VisitThreeExpr(ThreeExpr *threeExpr) {
    threeExpr->condExpr->AcceptVisitor(this);
    *out << "?";
    threeExpr->trueExpr->AcceptVisitor(this);
    *out << ":";
    threeExpr->falseExpr->AcceptVisitor(this);

    return nullptr;
}

llvm::Value *PrintVisitor::VisitPostIncExpr(PostIncExpr *postIncExpr) {
    postIncExpr->leftNode->AcceptVisitor(this);
    *out << "++";
    return nullptr;
}

llvm::Value *PrintVisitor::VisitPostDecExpr(PostDecExpr *postDecExpr) {
    postDecExpr->leftNode->AcceptVisitor(this);
    *out << "--";
    return nullptr;
}

llvm::Value *PrintVisitor::VisitPostSubscriptExpr(PostSubscriptExpr *postSubscriptExpr) {
    postSubscriptExpr->leftNode->AcceptVisitor(this);
    *out << "[";
    postSubscriptExpr->node->AcceptVisitor(this);
    *out << "]";
    return nullptr;
}

llvm::Value *PrintVisitor::VisitNumberExpr(NumberExpr *numberExpr) {
    *out << numberExpr->token.value;
    return nullptr;
}

llvm::Value *PrintVisitor::VisitVariableAssessExpr(VariableAssessExpr *variableAssessExpr) {
    *out << llvm::StringRef(variableAssessExpr->token.ptr, variableAssessExpr->token.length);
    return nullptr;
}

llvm::Type *PrintVisitor::VisitCPrimaryType(CPrimaryType *ty) {
    if (ty->GetTypeKind() == CType::CTypeKind::TY_Int) {
        *out << "int ";
    }
    return nullptr;
}

llvm::Type *PrintVisitor::VisitCPointType(CPointType *ty) {
    ty->GetBaseType()->AcceptVisitor(this);
    *out << "*";
    return nullptr;
}

llvm::Type *PrintVisitor::VisitCArrayType(CArrayType *ty) {
    *out << "[" << ty->GetElementCount() << "]";
    ty->GetElementType()->AcceptVisitor(this);
    return nullptr;
}