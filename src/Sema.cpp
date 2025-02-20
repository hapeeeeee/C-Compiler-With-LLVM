#include "include/Sema.h"

std::shared_ptr<ASTNode>
Sema::SemaIfStmtNode(std::shared_ptr<ASTNode> condExpr, std::shared_ptr<ASTNode> thenStmt, std::shared_ptr<ASTNode> elseStmt) {
    auto ifStmt      = std::make_shared<IfStmt>();
    ifStmt->condExpr = condExpr;
    ifStmt->thenStmt = thenStmt;
    ifStmt->elseStmt = elseStmt;
    return ifStmt;
}

std::shared_ptr<ASTNode> Sema::SemaForStmtNode(std::shared_ptr<ASTNode> initNode,
                                               std::shared_ptr<ASTNode> condNode,
                                               std::shared_ptr<ASTNode> thenNode,
                                               std::shared_ptr<ASTNode> bodyNode) {
    auto for_stmt      = std::make_shared<ForStmt>();
    for_stmt->initNode = initNode;
    for_stmt->condNode = condNode;
    for_stmt->thenNode = thenNode;
    for_stmt->bodyNode = bodyNode;
    return for_stmt;
}

std::shared_ptr<ASTNode> Sema::SemaVariableDeclNode(std::shared_ptr<CType> cType, Token &tok) {
    llvm::StringRef content = llvm::StringRef(tok.ptr, tok.length);
    // Check is redefined for symbol
    std::shared_ptr<Symbol> symbol = scope.FindObjSymbolInCurrEnv(content);
    if (symbol && (mode == Mode::Normal)) {
        diager.Report(llvm::SMLoc::getFromPointer(tok.ptr), diag::error_redefined, content);
    }

    if (mode == Mode::Normal) {
        scope.AddObjSymbol(content, cType);
    }

    auto variableDecl   = std::make_shared<VariableDecl>();
    variableDecl->token = tok;
    variableDecl->cType = cType;
    return variableDecl;
}

std::shared_ptr<VariableDecl::InitValue>
Sema::SemaDeclInitValue(std::shared_ptr<ASTNode> value, std::shared_ptr<CType> declTy, std::vector<int> &offsetList, Token &tok) {
    if (value->cType->GetTypeKind() != declTy->GetTypeKind() && (mode == Mode::Normal)) {
        diager.Report(llvm::SMLoc::getFromPointer(tok.ptr), diag::error_miss, "same type");
    }

    auto initValue        = std::make_shared<VariableDecl::InitValue>();
    initValue->value      = value;
    initValue->ty         = declTy;
    initValue->offsetList = offsetList;
    return initValue;
}

std::shared_ptr<ASTNode> Sema::SemaVariableAccessExprNode(Token &tok) {
    llvm::StringRef content        = llvm::StringRef(tok.ptr, tok.length);
    std::shared_ptr<Symbol> symbol = scope.FindObjSymbol(content);
    if (!symbol && (mode == Mode::Normal)) {
        diager.Report(llvm::SMLoc::getFromPointer(tok.ptr), diag::error_undefined, content);
    }

    auto expr   = std::make_shared<VariableAssessExpr>();
    expr->token = tok;
    expr->cType = symbol->cType;
    return expr;
}

std::shared_ptr<ASTNode> Sema::SemaBinaryExprNode(std::shared_ptr<ASTNode> left, BinOpCode op, std::shared_ptr<ASTNode> right) {
    auto binaryExpr   = std::make_shared<BinaryExpr>(left, op, right);
    binaryExpr->cType = left->cType;
    return binaryExpr;
}

std::shared_ptr<ASTNode>
Sema::SemaThreeExprNode(std::shared_ptr<ASTNode> left, std::shared_ptr<ASTNode> mid, std::shared_ptr<ASTNode> right, Token tok) {
    auto node       = std::make_shared<ThreeExpr>();
    node->condExpr  = left;
    node->trueExpr  = mid;
    node->falseExpr = right;
    if (node->trueExpr->cType->GetTypeKind() != node->falseExpr->cType->GetTypeKind() && (mode == Mode::Normal)) {
        diager.Report(llvm::SMLoc::getFromPointer(tok.ptr), diag::unsame_typename);
    }
    node->cType = node->trueExpr->cType;
    return node;
}

std::shared_ptr<ASTNode> Sema::SemaNumberExprNode(std::shared_ptr<CType> cType, Token &tok) {
    auto expr   = std::make_shared<NumberExpr>();
    expr->token = tok;
    expr->cType = cType;
    return expr;
}

std::shared_ptr<ASTNode> Sema::SemaSizeofExprNode(std::shared_ptr<ASTNode> expr, std::shared_ptr<CType> sizeofTY) {
    auto node      = std::make_shared<SizeofExpr>();
    node->sizeofTY = sizeofTY;
    node->expr     = expr;
    assert(sizeofTY || expr);
    node->cType = CType::IntType;
    return node;
}

std::shared_ptr<ASTNode> Sema::SemaUnaryExprNode(UnaryOpCode op, std::shared_ptr<ASTNode> expr, Token tok) {
    auto node  = std::make_shared<UnaryExpr>();
    node->op   = op;
    node->expr = expr;

    switch (op) {
    case UnaryOpCode::Negative:
    case UnaryOpCode::Positive:
    case UnaryOpCode::LogicNot:
    case UnaryOpCode::BitNot: {
        if (expr->cType->GetTypeKind() != CType::CTypeKind::TY_Int && (mode == Mode::Normal)) {
            diager.Report(llvm::SMLoc::getFromPointer(tok.ptr), diag::unexcept_type, "int");
        }
        node->cType = expr->cType;
        break;
    }
    case UnaryOpCode::Addr: { ///< type of `&a` should be ptr of type of `a`
        node->cType = std::make_shared<CPointType>(expr->cType);
        break;
    }
    case UnaryOpCode::Deref: { ///< type of `*a` should be value of ptr of `a`
        if (expr->cType->GetTypeKind() != CType::CTypeKind::TY_Point && (mode == Mode::Normal)) {
            diager.Report(llvm::SMLoc::getFromPointer(tok.ptr), diag::unexcept_type, "pointer");
        }
        CPointType *pointType = llvm::dyn_cast<CPointType>(expr->cType.get());
        node->cType           = pointType->GetBaseType();
        break;
    }
    case UnaryOpCode::Inc:
    case UnaryOpCode::Dec: {
        node->cType = expr->cType;
        break;
    }
    default:
        break;
    }
    return node;
}

std::shared_ptr<ASTNode> Sema::SemaPostIncExprNode(std::shared_ptr<ASTNode> leftNode) {
    auto node      = std::make_shared<PostIncExpr>();
    node->leftNode = leftNode;
    node->cType    = leftNode->cType;
    return node;
}

std::shared_ptr<ASTNode> Sema::SemaPostDecExprNode(std::shared_ptr<ASTNode> leftNode) {
    auto node      = std::make_shared<PostDecExpr>();
    node->leftNode = leftNode;
    node->cType    = leftNode->cType;
    return node;
}

/// a[offest] ::=  (void *)a + (offest * a->elementType->size)
std::shared_ptr<ASTNode>
Sema::SemaPostSubscriptExprNode(std::shared_ptr<ASTNode> leftNode, std::shared_ptr<ASTNode> offestNode, Token tok) {
    CType::CTypeKind leftTyKind = leftNode->cType->GetTypeKind();
    if (leftTyKind != CType::CTypeKind::TY_Array && leftTyKind != CType::CTypeKind::TY_Point && (mode == Mode::Normal)) {
        diager.Report(llvm::SMLoc::getFromPointer(tok.ptr), diag::unexcept_type, "arrry or pointer");
    }

    auto node      = std::make_shared<PostSubscriptExpr>();
    node->leftNode = leftNode;
    node->node     = offestNode;

    if (leftTyKind == CType::CTypeKind::TY_Array) {
        CArrayType *arrTy = llvm::dyn_cast<CArrayType>(leftNode->cType.get());
        node->cType       = arrTy->GetElementType();
    } else {
        CPointType *PointerTy = llvm::dyn_cast<CPointType>(leftNode->cType.get());
        node->cType           = PointerTy->GetBaseType();
    }

    return node;
}

std::shared_ptr<ASTNode> Sema::SemaPostMemberDotNode(std::shared_ptr<ASTNode> leftNode, Token tok) {
    if (leftNode->cType->GetTypeKind() != CType::CTypeKind::TY_Record) {
        diager.Report(llvm::SMLoc::getFromPointer(tok.ptr), diag::unexcept_type, "Struct or Union");
    }

    CRecordType *recordTy = llvm::dyn_cast<CRecordType>(leftNode->cType.get());

    auto &members = recordTy->GetMerbers();
    Member targetMember;
    bool isFound = false;
    for (auto &m : members) {
        if (m.name == llvm::StringRef(tok.ptr, tok.length)) {
            isFound      = true;
            targetMember = m;
            break;
        }
    }
    if (!isFound) {
        diager.Report(llvm::SMLoc::getFromPointer(tok.ptr), diag::error_miss, "this field in Struct or Union");
    }
    auto node      = std::make_shared<PostMemberDotExpr>();
    node->cType    = targetMember.cType;
    node->token    = tok;
    node->leftNode = leftNode;
    node->member   = targetMember;
    return node;
}

std::shared_ptr<ASTNode> Sema::SemaPostMemberArrowNode(std::shared_ptr<ASTNode> leftNode, Token tok) {
    if (leftNode->cType->GetTypeKind() != CType::CTypeKind::TY_Point) {
        diager.Report(llvm::SMLoc::getFromPointer(tok.ptr), diag::unexcept_type, "Pointer Type");
    }

    CPointType *pointerTy = llvm::dyn_cast<CPointType>(leftNode->cType.get());
    if (pointerTy->GetBaseType()->GetTypeKind() != CType::CTypeKind::TY_Record) {
        diager.Report(llvm::SMLoc::getFromPointer(tok.ptr), diag::unexcept_type, "Struct or Union");
    }

    CRecordType *recordTy = llvm::dyn_cast<CRecordType>(pointerTy->GetBaseType().get());
    auto &members         = recordTy->GetMerbers();
    Member targetMember;
    bool isFound = false;
    for (auto &m : members) {
        if (m.name == llvm::StringRef(tok.ptr, tok.length)) {
            isFound      = true;
            targetMember = m;
            break;
        }
    }
    if (!isFound) {
        diager.Report(llvm::SMLoc::getFromPointer(tok.ptr), diag::error_miss, "this field in Struct or Union");
    }
    auto node      = std::make_shared<PostMemberArrowExpr>();
    node->cType    = targetMember.cType;
    node->token    = tok;
    node->leftNode = leftNode;
    node->member   = targetMember;
    return node;
}

std::shared_ptr<CType> Sema::SemaTagDecl(std::vector<Member> &members, TagKind tagKind, Token &tok) {
    llvm::StringRef content        = llvm::StringRef(tok.ptr, tok.length);
    std::shared_ptr<Symbol> symbol = scope.FindTagSymbolInCurrEnv(content);
    if (symbol && (mode == Mode::Normal)) {
        diager.Report(llvm::SMLoc::getFromPointer(tok.ptr), diag::error_undefined, content);
    }
    auto recordTy = std::make_shared<CRecordType>(content, members, tagKind);
    if (mode == Mode::Normal) {
        scope.AddTagSymbol(content, recordTy);
    }
    return recordTy;
}

std::shared_ptr<CType> Sema::SemaTagAccess(Token &tok) {
    llvm::StringRef content        = llvm::StringRef(tok.ptr, tok.length);
    std::shared_ptr<Symbol> symbol = scope.FindTagSymbol(content);
    if (!symbol && (mode == Mode::Normal)) {
        diager.Report(llvm::SMLoc::getFromPointer(tok.ptr), diag::error_undefined, content);
    }

    return symbol->cType;
}

void Sema::EnterScope() {
    scope.EnterScope();
}

void Sema::ExitScope() {
    scope.ExitScope();
}

void Sema::SetMode(Mode mode) {
    this->mode = mode;
}
