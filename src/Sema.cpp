#include "include/Sema.h"
#include "llvm/Support/Casting.h"

std::shared_ptr<ASTNode> Sema::SemaVariableDeclNode(CType *cType, Token &tok) {
    // Check is redefined for symbol
    std::shared_ptr<Symbol> symbol = scope.FindVarSymbolInCurrEnv(tok.content);
    if (symbol) {
        llvm::errs() << "Error: redefine variable: " << tok.content << ". row: " << tok.row
                     << " , col: " << tok.col << "\n";
    }
    scope.AddSymbol(tok.content, SymbolKind::LocalVariable, cType);
    auto variableDecl   = std::make_shared<VariableDecl>();
    variableDecl->name  = tok.content;
    variableDecl->cType = cType;
    return variableDecl;
}

std::shared_ptr<ASTNode> Sema::SemaAssignExprNode(std::shared_ptr<ASTNode> left,
                                                  std::shared_ptr<ASTNode> right) {
    if (!left || !right) {
        llvm::errs() << "Error: left and right expr can't be null in assign expr\n";
    }
    if (!llvm::isa<VariableAssessExpr>(left.get())) {
        llvm::errs() << "Error: left in assign expr must be left value\n";
    }
    return std::make_shared<AssignExpr>(left, right);
}

std::shared_ptr<ASTNode> Sema::SemaVariableAccessExprNode(CType *cType, Token &tok) {
    std::shared_ptr<Symbol> symbol = scope.FindVarSymbol(tok.content);
    if (!symbol) {
        llvm::errs() << "Error: use undefined variable: " << tok.content << ". row: " << tok.row
                     << " , col: " << tok.col << "\n";
    }

    auto expr   = std::make_shared<VariableAssessExpr>();
    expr->name  = tok.content;
    expr->cType = cType;

    return expr;
}