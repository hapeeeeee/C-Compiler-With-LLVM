#include "include/Sema.h"

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

// std::shared_ptr<ASTNode> Sema::SemaBinaryExprNode(VariableDecl) {
// }
