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

/// @brief stmt : decl-stmt | expr-stmt | null-stmt | if-stmt | block-stmt | for-stmt
std::shared_ptr<ASTNode> Parser::ParserStmt() {
    if (token.tokenTy == TokenType::Semi) { ///< null-stmt
        Advance();
        return nullptr;
    } else if (token.tokenTy == TokenType::KW_int) { ///< decl-stmt
        return ParserDeclStmt();
    } else if (token.tokenTy == TokenType::KW_if) { ///< if-stmt
        return ParserIfStmt();
    } else if (token.tokenTy == TokenType::KW_for) { ///< for-stmt
        return ParserForStmt();
    } else if (token.tokenTy == TokenType::LeftBrace) { ///< block-stmt
        return ParserBlockStmt();
    } else { ///< expr-stmt
        return ParserExprStmt();
    }
}

/// @brief decl-stmt : "int" identifier ("=" expr)? ("," identifier ("=" expr)?)* ";"
std::shared_ptr<ASTNode> Parser::ParserDeclStmt() {
    Consume(TokenType::KW_int);
    CType *cTy    = CType::getIntTy();
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

std::shared_ptr<ASTNode> Parser::ParserBlockStmt() {
    sema.EnterScope();
    Consume(TokenType::LeftBrace);
    auto blockStmts = std::make_shared<BlockStmts>();
    while (token.tokenTy != TokenType::RightBrace) {
        blockStmts->nodeVec.push_back(ParserStmt());
    }
    Consume(TokenType::RightBrace);
    sema.ExitScope();
    return blockStmts;
}

/// @brief expr-stmt : expr ";"
std::shared_ptr<ASTNode> Parser::ParserExprStmt() {
    auto expr = ParserExpr();
    Consume(TokenType::Semi);
    return expr;
}

/// @brief if-stmt : "if" "(" expr ")" stmt  ("else" stmt )?
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

/// @brief for-stmt : "for" "(" expr?       ; expr? ";" expr? ")"  stmt
///                 : "for" "(" decl-stmt?  ; expr? ";" expr? ")"  stmt
std::shared_ptr<ASTNode> Parser::ParserForStmt() {
    Consume(TokenType::KW_for);
    Consume(TokenType::LeftParent);
    sema.EnterScope();
    std::shared_ptr<ASTNode> initNode = nullptr, condNode = nullptr, thenNode = nullptr,
                             bodyNode = nullptr;
    if (IsTypeName()) {
        initNode = ParserDeclStmt();
    } else {
        if (token.tokenTy != TokenType::Semi) {
            initNode = ParserExpr();
        }
        Consume(TokenType::Semi);
    }

    if (token.tokenTy != TokenType::Semi) {
        condNode = ParserExpr();
    }
    Consume(TokenType::Semi);
    if (token.tokenTy != TokenType::RightParent) {
        thenNode = ParserExpr();
    }
    Consume(TokenType::RightParent);
    bodyNode  = ParserStmt();
    auto node = sema.SemaForStmtNode(initNode, condNode, thenNode, bodyNode);
    sema.ExitScope();
    return node;
}

/// @brief expr : assign-expr | equal-expr
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
    return ParserEqualExpr();
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

/// @brief equal-expr  : relational-expr ( ("==" | "!=") relational-expr)*
std::shared_ptr<ASTNode> Parser::ParserEqualExpr() {
    auto left = ParserRelationalExpr();
    while (token.tokenTy == TokenType::EqualEqual || token.tokenTy == TokenType::NotEqual) {
        OpCode op;
        if (token.tokenTy == TokenType::EqualEqual) {
            op = OpCode::EqualEqual;
        } else {
            op = OpCode::NotEqual;
        }
        Advance();
        auto right = ParserRelationalExpr();
        left       = sema.SemaBinaryExprNode(left, op, right);
    }
    return left;
}

/// @brief relational-expr : add-expr (( ">" |"<" | "<=" | ">=") add-expr)*
std::shared_ptr<ASTNode> Parser::ParserRelationalExpr() {
    auto left = ParserAddExpr();
    while (token.tokenTy == TokenType::Less || token.tokenTy == TokenType::LessEqual ||
           token.tokenTy == TokenType::Greater || token.tokenTy == TokenType::GreaterEqual) {
        OpCode op;
        if (token.tokenTy == TokenType::Less) {
            op = OpCode::Less;
        } else if (token.tokenTy == TokenType::LessEqual) {
            op = OpCode::LessEqual;
        } else if (token.tokenTy == TokenType::Greater) {
            op = OpCode::Greater;
        } else {
            op = OpCode::GreaterEqual;
        }
        Advance();
        auto right = ParserAddExpr();
        left       = sema.SemaBinaryExprNode(left, op, right);
    }
    return left;
}

/// @brief  add-expr : mult-expr ( ("+" | "-") mult-expr)*
std::shared_ptr<ASTNode> Parser::ParserAddExpr() {
    auto left = ParserMultExpr();
    // a + b + c + d...
    while (token.tokenTy == TokenType::Plus || token.tokenTy == TokenType::Minus) {
        OpCode op;
        if (token.tokenTy == TokenType::Plus) {
            op = OpCode::Add;
        } else {
            op = OpCode::Sub;
        }
        Advance();

        auto right = ParserMultExpr();
        left       = sema.SemaBinaryExprNode(left, op, right);
    }
    return left;
}

/// @brief mult-expr : primary-expr ( ("*" | "/") primary-expr)*
std::shared_ptr<ASTNode> Parser::ParserMultExpr() {
    auto left = ParserPrimaryExpr();
    // a * b * c * d...
    while (token.tokenTy == TokenType::Star || token.tokenTy == TokenType::Slash) {
        OpCode op;
        if (token.tokenTy == TokenType::Star) {
            op = OpCode::Mul;
        } else {
            op = OpCode::Div;
        }
        Advance();
        auto right   = ParserPrimaryExpr();
        auto binExpr = sema.SemaBinaryExprNode(left, op, right);
        left         = binExpr;
    }
    return left;
}

/// @brief primary-expr : identifier | number | "(" expr")"
std::shared_ptr<ASTNode> Parser::ParserPrimaryExpr() {
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

bool Parser::IsTypeName() {
    if (token.tokenTy == TokenType::KW_int) {
        return true;
    }
    return false;
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