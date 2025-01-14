#include "include/Sema.h"

std::shared_ptr<ASTNode> Sema::SemaIfStmtNode(std::shared_ptr<ASTNode> condExpr,
                                              std::shared_ptr<ASTNode> thenStmt,
                                              std::shared_ptr<ASTNode> elseStmt) {
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

std::shared_ptr<ASTNode> Sema::SemaVariableDeclNode(CType *cType, Token &tok) {
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

std::shared_ptr<ASTNode>
Sema::SemaAssignExprNode(std::shared_ptr<ASTNode> left, std::shared_ptr<ASTNode> right, Token tok) {
    assert(left && right);
    if (!llvm::isa<VariableAssessExpr>(left.get())) {
        diager.Report(llvm::SMLoc::getFromPointer(tok.ptr), diag::error_lvalue);
    }
    auto expr   = std::make_shared<AssignExpr>(left, right);
    expr->token = left->token;
    return expr;
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

std::shared_ptr<ASTNode>
Sema::SemaBinaryExprNode(std::shared_ptr<ASTNode> left, OpCode op, std::shared_ptr<ASTNode> right) {
    return std::make_shared<BinaryExpr>(left, op, right);
}

std::shared_ptr<ASTNode> Sema::SemaNumberExprNode(CType *cType, Token &tok) {
    auto expr   = std::make_shared<NumberExpr>();
    expr->token = tok;
    expr->cType = cType;
    return expr;
}

void Sema::EnterScope() {
    scope.EnterScope();
}

void Sema::ExitScope() {
    scope.ExitScope();
}