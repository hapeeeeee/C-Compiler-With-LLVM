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

/// @brief stmt : decl-stmt | expr-stmt | null-stmt | if-stmt | block-stmt |
///               for-stmt  | break-stmt | continue-stmt
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
    } else if (token.tokenTy == TokenType::KW_break) { ///< for-stmt
        return ParserBreakStmt();
    } else if (token.tokenTy == TokenType::KW_continue) { ///< for-stmt
        return ParserContinueStmt();
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
    auto for_stmt = std::make_shared<ForStmt>();
    nodesContainBreak.push_back(for_stmt);
    nodesContainContinue.push_back(for_stmt);
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
    bodyNode           = ParserStmt();
    for_stmt->initNode = initNode;
    for_stmt->condNode = condNode;
    for_stmt->thenNode = thenNode;
    for_stmt->bodyNode = bodyNode;
    sema.ExitScope();
    nodesContainBreak.pop_back();
    nodesContainContinue.pop_back();

    return for_stmt;
}

/// @brief  break-stmt : "break" ";"
std::shared_ptr<ASTNode> Parser::ParserBreakStmt() {
    if (nodesContainBreak.size() == 0) {
        GetDiagnostics().Report(llvm::SMLoc::getFromPointer(token.ptr),
                                diag::error_break_not_in_loop);
    }
    Consume(TokenType::KW_break);
    Consume(TokenType::Semi);
    auto node        = std::make_shared<BreakStmt>();
    node->fatherNode = nodesContainBreak.back();
    return node;
}

/// @brief  continue-stmt : "continue" ";"
std::shared_ptr<ASTNode> Parser::ParserContinueStmt() {
    if (nodesContainContinue.size() == 0) {
        GetDiagnostics().Report(llvm::SMLoc::getFromPointer(token.ptr),
                                diag::error_continue_not_in_loop);
    }
    Consume(TokenType::KW_continue);
    Consume(TokenType::Semi);
    auto node        = std::make_shared<ContinueStmt>();
    node->fatherNode = nodesContainContinue.back();
    return node;
}

/// @brief expr : assign-expr | logicor-expr
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

    return ParserLogicOrExpr();
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

/// @brief logicor-expr : logicand-expr ("||" logicand-expr)*
std::shared_ptr<ASTNode> Parser::ParserLogicOrExpr() {
    auto left = ParserLogicAndExpr();
    while (token.tokenTy == TokenType::PipePipe) {
        OpCode op = OpCode::LogicOr;
        Advance();
        auto right = ParserLogicAndExpr();
        left       = sema.SemaBinaryExprNode(left, op, right);
    }
    return left;
}

/// @brief logicand-expr : bitor-expr ("&&" bitor-expr)*
std::shared_ptr<ASTNode> Parser::ParserLogicAndExpr() {
    auto left = ParserBitOrExpr();
    while (token.tokenTy == TokenType::AmpAmp) {
        OpCode op = OpCode::LogicAnd;
        Advance();
        auto right = ParserBitOrExpr();
        left       = sema.SemaBinaryExprNode(left, op, right);
    }
    return left;
}

/// @brief bitor-expr : bitxor-expr ("|" bitxor-expr)*
std::shared_ptr<ASTNode> Parser::ParserBitOrExpr() {
    auto left = ParserBitXorExpr();
    while (token.tokenTy == TokenType::Pipe) {
        OpCode op = OpCode::BitOr;
        Advance();
        auto right = ParserBitXorExpr();
        left       = sema.SemaBinaryExprNode(left, op, right);
    }
    return left;
}

/// @brief bitxor-expr : bitand-expr ("^" bitand-expr)*
std::shared_ptr<ASTNode> Parser::ParserBitXorExpr() {
    auto left = ParserBitAndExpr();
    while (token.tokenTy == TokenType::Caret) {
        OpCode op = OpCode::BitXor;
        Advance();
        auto right = ParserBitAndExpr();
        left       = sema.SemaBinaryExprNode(left, op, right);
    }
    return left;
}

/// @brief bitand-expr : equal-expr ("&" equal-expr)*
std::shared_ptr<ASTNode> Parser::ParserBitAndExpr() {
    auto left = ParserEqualExpr();
    while (token.tokenTy == TokenType::Amp) {
        OpCode op = OpCode::BitAnd;
        Advance();
        auto right = ParserEqualExpr();
        left       = sema.SemaBinaryExprNode(left, op, right);
    }
    return left;
}

/// @brief equal-expr : relational-expr ( ("==" | "!=") relational-expr)*
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

/// @brief relational-expr : shift-expr (( ">" |"<" | "<=" | ">=") shift-expr)*
std::shared_ptr<ASTNode> Parser::ParserRelationalExpr() {
    auto left = ParserShiftExpr();
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
        auto right = ParserShiftExpr();
        left       = sema.SemaBinaryExprNode(left, op, right);
    }
    return left;
}

/// @brief shift-expr : add-expr ( ("<<" | ">>") add-expr )*
std::shared_ptr<ASTNode> Parser::ParserShiftExpr() {
    auto left = ParserAddExpr();
    while (token.tokenTy == TokenType::LessLess || token.tokenTy == TokenType::GreaterGreater) {
        OpCode op;
        if (token.tokenTy == TokenType::LessLess) {
            op = OpCode::LeftShift;
        } else {
            op = OpCode::RightShift;
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

/// @brief mult-expr : primary-expr ( ("*" | "/" | "%") primary-expr)*
std::shared_ptr<ASTNode> Parser::ParserMultExpr() {
    auto left = ParserPrimaryExpr();
    // a * b * c * d...
    while (token.tokenTy == TokenType::Star || token.tokenTy == TokenType::Slash ||
           token.tokenTy == TokenType::Percent) {
        OpCode op;
        if (token.tokenTy == TokenType::Star) {
            op = OpCode::Mul;
        } else if (token.tokenTy == TokenType::Slash) {
            op = OpCode::Div;
        } else {
            op = OpCode::Mod;
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