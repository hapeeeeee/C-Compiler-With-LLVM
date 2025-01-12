#include "include/Parser.h"

Parser::Parser(Lexer &lex, Sema &sema) : lexer(lex), sema(sema) {
    Advance();
}

/// @brief  prog : stmt*
std::shared_ptr<Program> Parser::ParserProgram() {
    std::vector<std::shared_ptr<ASTNode>> stmts;
    while (token.tokenTy != TokenType::Eof) {
        auto stmt = Parser::ParserStmt();
        if (stmt) {
            stmts.push_back(stmt);
        }
    }
    auto program = std::make_shared<Program>(std::move(stmts));
    return program;
}

/// @brief stmt : decl-stmt | expr-stmt | null-stmt | if-stmt
std::shared_ptr<ASTNode> Parser::ParserStmt() {
    if (token.tokenTy == TokenType::Semi) { ///< null-stmt
        Advance();
        return nullptr;
    } else if (token.tokenTy == TokenType::KW_int) { ///< decl-stmt
        return ParserDeclStmt();
    } else if (token.tokenTy == TokenType::KW_if) {
        return ParserIfStmt();
    } else { ///< expr-stmt
        return ParserExprStmt();
    }
}

/// @brief decl-stmt : "int" identifier ("=" expr)? ("," identifier ("=" expr)?)* ";"
std::shared_ptr<ASTNode> Parser::ParserDeclStmt() {
    Consume(TokenType::KW_int);
    CType *cTy = CType::getIntTy();

    auto declNode = std::make_shared<DeclStmts>();
    // int a = 1, c = 2, d;
    while (token.tokenTy != TokenType::Semi) {
        Token variableToken = token;
        auto variableDecl   = sema.SemaVariableDeclNode(cTy, token);
        declNode->nodeVec.push_back(variableDecl);
        assert(Consume(TokenType::Identifier));

        if (token.tokenTy == TokenType::Equal) {
            Token tok = token;
            Advance();
            auto left       = sema.SemaVariableAccessExprNode(variableToken);
            auto right      = ParserExpr();
            auto assignExpr = sema.SemaAssignExprNode(left, right, tok);
            declNode->nodeVec.push_back(assignExpr);
        }

        if (token.tokenTy == TokenType::Comma) {
            Advance();
        }
    }
    Consume(TokenType::Semi);
    return declNode;
}

/// @brief expr-stmt : expr ";"
std::shared_ptr<ASTNode> Parser::ParserExprStmt() {
    auto expr = ParserExpr();
    Consume(TokenType::Semi);
    return expr;
}

/// @brief if-stmt : "if" "(" expr ")" "{" stmt  "}" ("else" "{" stmt "}")?
std::shared_ptr<ASTNode> Parser::ParserIfStmt() {
    Consume(TokenType::KW_if);
    Consume(TokenType::LeftParent);
    auto condExpr = ParserExpr();
    Consume(TokenType::RightParent);
    auto thenStmt                     = ParserStmt();
    std::shared_ptr<ASTNode> elseStmt = nullptr;
    if (token.tokenTy == TokenType::KW_else) {
        Consume(TokenType::KW_else);
        elseStmt = ParserStmt();
    }
    return sema.SemaIfStmtNode(condExpr, thenStmt, elseStmt);
}

/// @brief expr        : assign-expr | add-expr
///        assign-expr : identifier ("=" expr)+
///        add-expr    : mult-expr ( ("+" | "_") mult-expr)*
std::shared_ptr<ASTNode> Parser::ParserExpr() {
    bool isAssignExpr = false;
    lexer.SaveState();
    if (token.tokenTy == TokenType::Identifier) {
        Token tmp;
        lexer.NextToken(tmp);
        if (tmp.tokenTy == TokenType::Equal) {
            isAssignExpr = true;
        }
    }
    lexer.RestoreState();

    if (isAssignExpr) {
        return ParserAssignExpr();
    }

    auto left = ParserTerm();
    // a + b + c + d...
    while (token.tokenTy == TokenType::Plus || token.tokenTy == TokenType::Minus) {
        OpCode op;
        if (token.tokenTy == TokenType::Plus) {
            op = OpCode::Add;
        } else {
            op = OpCode::Sub;
        }
        Advance();

        auto right   = ParserTerm();
        auto binExpr = sema.SemaBinaryExprNode(left, op, right);
        left         = binExpr;
    }
    return left;
}

/// @brief assign-expr : identifier ("=" expr)+
std::shared_ptr<ASTNode> Parser::ParserAssignExpr() {
    IsExcept(TokenType::Identifier);
    auto leftExpr = sema.SemaVariableAccessExprNode(token);
    Advance();
    Token tok = token;
    Consume(TokenType::Equal);
    return sema.SemaAssignExprNode(leftExpr, ParserExpr(), tok);
}

/// @brief term  : factor(("*" | "/") factor)*
std::shared_ptr<ASTNode> Parser::ParserTerm() {
    auto left = ParserFactor();
    // a * b * c * d...
    while (token.tokenTy == TokenType::Star || token.tokenTy == TokenType::Slash) {
        OpCode op;
        if (token.tokenTy == TokenType::Star) {
            op = OpCode::Mul;
        } else {
            op = OpCode::Div;
        }
        Advance();
        auto right   = ParserFactor();
        auto binExpr = sema.SemaBinaryExprNode(left, op, right);
        left         = binExpr;
    }
    return left;
}

/// @brief factor : identifier | number | "(" expr")"
std::shared_ptr<ASTNode> Parser::ParserFactor() {
    if (token.tokenTy == TokenType::LeftParent) {
        Advance();
        auto expr = ParserExpr();
        assert(IsExcept(TokenType::RightParent));
        Advance();
        return expr;
    } else if (token.tokenTy == TokenType::Identifier) {
        auto factorExpr = sema.SemaVariableAccessExprNode(token);
        Advance();
        return factorExpr;
    } else {
        IsExcept(TokenType::Number);
        auto factorExpr = sema.SemaNumberExprNode(token.cType, token);
        Advance();
        return factorExpr;
    }
}

bool Parser::IsExcept(TokenType tokTy) {
    if (token.tokenTy != tokTy) {
        GetDiagnostics().Report(llvm::SMLoc::getFromPointer(token.ptr),
                                diag::error_except,
                                Token::GetSpellingText(tokTy),
                                llvm::StringRef(token.ptr, token.length));
        return false;
    }
    return true;
}

bool Parser::Consume(TokenType tokTy) {
    if (IsExcept(tokTy)) {
        Advance();
        return true;
    }
    GetDiagnostics().Report(llvm::SMLoc::getFromPointer(token.ptr),
                            diag::error_except,
                            Token::GetSpellingText(tokTy),
                            llvm::StringRef(token.ptr, token.length));
    return false;
}

void Parser::Advance() {
    lexer.NextToken(token);
}

Diagnostics &Parser::GetDiagnostics() {
    return lexer.GetDiagnostics();
}