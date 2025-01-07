#include "include/Sema.h"
#include "llvm/Support/Casting.h"

std::shared_ptr<ASTNode> Sema::SemaVariableDeclNode(CType *cType, Token &tok) {
    llvm::StringRef content = llvm::StringRef(tok.ptr, tok.length);
    // Check is redefined for symbol
    std::shared_ptr<Symbol> symbol = scope.FindVarSymbolInCurrEnv(content);
    if (symbol) {
        llvm::errs() << "Error: redefine variable: " << content << ". row: " << tok.row
                     << " , col: " << tok.col << "\n";
    }
    scope.AddSymbol(content, SymbolKind::LocalVariable, cType);
    auto variableDecl   = std::make_shared<VariableDecl>();
    variableDecl->token = tok;
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

std::shared_ptr<ASTNode> Sema::SemaVariableAccessExprNode(Token &tok) {
    llvm::StringRef content        = llvm::StringRef(tok.ptr, tok.length);
    std::shared_ptr<Symbol> symbol = scope.FindVarSymbol(content);
    if (!symbol) {
        llvm::errs() << "Error: use undefined variable: " << content << ". row: " << tok.row
                     << " , col: " << tok.col << "\n";
    }

    auto expr   = std::make_shared<VariableAssessExpr>();
    expr->name  = content;
    expr->cType = symbol->cType;
    return expr;
}

std::shared_ptr<ASTNode>
Sema::SemaBinaryExprNode(std::shared_ptr<ASTNode> left, OpCode op, std::shared_ptr<ASTNode> right) {
    return std::make_shared<BinaryExpr>(left, op, right);
}

std::shared_ptr<ASTNode> Sema::SemaNumberExprNode(CType *cType, Token &tok) {
    auto expr   = std::make_shared<NumberExpr>(tok.value);
    expr->cType = cType;
    return expr;
}