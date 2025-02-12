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
    std::shared_ptr<Symbol> symbol = scope.FindVarSymbolInCurrEnv(content);
    if (symbol) {
        diager.Report(llvm::SMLoc::getFromPointer(tok.ptr), diag::error_redefined, content);
    }
    scope.AddSymbol(content, SymbolKind::LocalVariable, cType);

    auto variableDecl   = std::make_shared<VariableDecl>();
    variableDecl->token = tok;
    variableDecl->cType = cType;
    return variableDecl;
}

std::shared_ptr<ASTNode> Sema::SemaVariableAccessExprNode(Token &tok) {
    llvm::StringRef content        = llvm::StringRef(tok.ptr, tok.length);
    std::shared_ptr<Symbol> symbol = scope.FindVarSymbol(content);
    if (!symbol) {
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
    if (node->trueExpr->cType->GetTypeKind() != node->falseExpr->cType->GetTypeKind()) {
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
        if (expr->cType->GetTypeKind() != CType::CTypeKind::TY_Int) {
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
        if (expr->cType->GetTypeKind() != CType::CTypeKind::TY_Point) {
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

void Sema::EnterScope() {
    scope.EnterScope();
}

void Sema::ExitScope() {
    scope.ExitScope();
}