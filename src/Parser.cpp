#include "include/Parser.h"

Parser::Parser(Lexer &lex, Sema &sema) : lexer(lex), sema(sema) {
    Advance();
}

/// @brief prog : block-stmt
std::shared_ptr<Program> Parser::ParserProgram() {
    auto program = std::make_shared<Program>();
    if (token.tokenTy != TokenType::Eof) {
        program->node = Parser::ParserBlockStmt();
    }
    IsExcept(TokenType::Eof);
    return program;
}

/// @brief stmt : decl-stmt | expr-stmt | null-stmt | if-stmt | block-stmt | for-stmt  | break-stmt | continue-stmt
std::shared_ptr<ASTNode> Parser::ParserStmt() {
    if (token.tokenTy == TokenType::Semi) { ///< null-stmt
        Advance();
        return nullptr;
    } else if (IsTypeName(token.tokenTy)) { ///< decl-stmt
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

/// @brief decl-stmt            : decl-spec init-declarator-list? ";"
//         init-declarator-list : declarator ("=" initializer)? ("," declarator ("=" initializer)?)*
std::shared_ptr<ASTNode> Parser::ParserDeclStmt() {
    std::shared_ptr<CType> cTy = ParserDeclSpec();

    // handle null decl stmt, like `int;`
    if (token.tokenTy == TokenType::Semi) {
        Consume(TokenType::Semi);
        return nullptr;
    }

    auto declNode = std::make_shared<DeclStmts>();
    // int a = 1, b = 1;
    while (token.tokenTy != TokenType::Semi) {
        declNode->nodeVec.push_back(ParserDeclarator(cTy));
        if (token.tokenTy == TokenType::Comma) {
            Advance();
        }
    }
    Consume(TokenType::Semi);
    return declNode;
}

/// @brief decl-spec : "int"
std::shared_ptr<CType> Parser::ParserDeclSpec() {
    if (token.tokenTy == TokenType::KW_int) {
        Advance();
        return CType::IntType;
    } else if (token.tokenTy == TokenType::KW_sturct || token.tokenTy == TokenType::KW_union) {
        return ParserDeclStructOrUnionSpec();
    }
    GetDiagnostics().Report(
        llvm::SMLoc::getFromPointer(token.ptr), diag::unexpect_typename, llvm::StringRef(token.ptr, token.length));
    return nullptr;
}

/// @brief struct-union-spec : struct-or-union identifier "{" (decl-spec declarator)* "}"
std::shared_ptr<CType> Parser::ParserDeclStructOrUnionSpec() {
    TagKind tagKind;
    Token tmp;
    if (token.tokenTy == TokenType::KW_sturct) {
        tagKind = TagKind::kSturct;
    } else if (token.tokenTy == TokenType::KW_union) {
        tagKind = TagKind::kUnion;
    } else {
        assert(0);
        return nullptr;
    }
    Advance();
    bool isAnony = false;
    if (token.tokenTy == TokenType::LeftBrace) {
        isAnony = true;
    } else {
        IsExcept(TokenType::Identifier);
        tmp = token;
        Consume(TokenType::Identifier);
    }

    // sturct A;
    if (token.tokenTy != TokenType::LeftBrace) {
        return sema.SemaTagAccess(tmp);
    }

    // sturct A{int a, b, *p; int **p;};
    Consume(TokenType::LeftBrace);
    sema.EnterScope();
    std::vector<Member> members;
    while (token.tokenTy != TokenType::RightBrace) {
        auto node         = ParserDeclStmt();
        auto declStmtNode = llvm::dyn_cast<DeclStmts>(node.get());

        for (auto &declNode : declStmtNode->nodeVec) {
            Member m;
            m.cType = declNode->cType;
            m.name  = llvm::StringRef(declNode->token.ptr, declNode->token.length);
            members.push_back(m);
        }
    }
    sema.ExitScope();
    Consume(TokenType::RightBrace);
    if (isAnony) {
        return sema.SemaAnonyTagDecl(members, tagKind);
    } else {
        return sema.SemaTagDecl(members, tagKind, tmp);
    }
}

/// @brief declarator : "*"* direct-declarator
std::shared_ptr<ASTNode> Parser::ParserDeclarator(std::shared_ptr<CType> baseType) {
    while (token.tokenTy == TokenType::Star) {
        Consume(TokenType::Star);
        baseType = std::make_shared<CPointType>(baseType);
    }

    return ParserDirectDeclarator(baseType);
}

/// @brief direct-declarator : identifier | "(" declarator ")" | direct-declarator "[" assign-expr "]"
std::shared_ptr<ASTNode> Parser::ParserDirectDeclarator(std::shared_ptr<CType> baseType) {
    std::shared_ptr<ASTNode> declNode = nullptr;
    if (token.tokenTy == TokenType::LeftParent) {
        Token beginTok = token;
        lexer.SaveState();

        Consume(TokenType::LeftParent);
        sema.SetMode(Sema::Mode::Skip);
        ParserDeclarator(CType::IntType);
        Consume(TokenType::RightParent);
        baseType = ParserDirectDeclaratorSuffix(baseType);

        sema.SetMode(Sema::Mode::Normal);
        lexer.RestoreState();
        token = beginTok;

        Consume(TokenType::LeftParent);
        declNode = ParserDeclarator(baseType);
        Consume(TokenType::RightParent);
        ParserDirectDeclaratorSuffix(CType::IntType);
    } else if (token.tokenTy == TokenType::Identifier) {
        Token ident = token;
        Consume(TokenType::Identifier);
        baseType = ParserDirectDeclaratorSuffix(baseType);
        declNode = sema.SemaVariableDeclNode(baseType, ident);
    } else {
        IsExcept(TokenType::Identifier);
    }

    if (token.tokenTy == TokenType::Equal) {
        Advance();
        auto newNode = llvm::dyn_cast<VariableDecl>(declNode.get());
        std::vector<int> offsetList{0};
        ParserInitializer(newNode->initValues, newNode->cType, offsetList, false);
    }
    return declNode;
}

std::shared_ptr<CType> Parser::ParserDirectDeclaratorSuffix(std::shared_ptr<CType> baseType) {
    if (token.tokenTy == TokenType::LeftBracket) {
        return ParserDirectDeclaratorArraySuffix(baseType);
    }
    return baseType;
}

/// @brief Parse "[" assign-expr "]"
std::shared_ptr<CType> Parser::ParserDirectDeclaratorArraySuffix(std::shared_ptr<CType> baseType) {
    if (token.tokenTy != TokenType::LeftBracket) {
        return baseType;
    }

    Consume(TokenType::LeftBracket);
    IsExcept(TokenType::Number);
    int count = token.value;
    Consume(TokenType::Number);
    Consume(TokenType::RightBracket);
    return std::make_shared<CArrayType>(ParserDirectDeclaratorArraySuffix(baseType), count);
}

/// @brief initializer : assign-expr| "{" initializer (("," initializer)?)* "}"
bool Parser::ParserInitializer(std::vector<std::shared_ptr<VariableDecl::InitValue>> &initValues,
                               std::shared_ptr<CType> declTy,
                               std::vector<int> &offsetList,
                               bool hasLeftBrace) {
    if (token.tokenTy == TokenType::RightBrace) {
        if (!hasLeftBrace) {
            GetDiagnostics().Report(llvm::SMLoc::getFromPointer(token.ptr), diag::error_miss, "{");
        }
        return true;
    }

    if (token.tokenTy == TokenType::LeftBrace) {
        Consume(TokenType::LeftBrace);

        if (declTy->GetTypeKind() == CType::CTypeKind::TY_Array) {
            auto arrTy = llvm::dyn_cast<CArrayType>(declTy.get());
            int size   = arrTy->GetElementCount(); // 2
            for (int i = 0; i < size; i++) {
                if (i > 0 && token.tokenTy == TokenType::Comma) {
                    Consume(TokenType::Comma);
                }
                offsetList.push_back(i);
                bool isEnd = ParserInitializer(initValues, arrTy->GetElementType(), offsetList, true);
                offsetList.pop_back();
                if (isEnd) {
                    break;
                }
            }
        } else if (declTy->GetTypeKind() == CType::CTypeKind::TY_Record) {
            auto recordTy = llvm::dyn_cast<CRecordType>(declTy.get());
            auto members  = recordTy->GetMerbers();
            if (recordTy->GetTagKind() == TagKind::kSturct) {
                for (int i = 0; i < members.size(); i++) {
                    if (i > 0 && token.tokenTy == TokenType::Comma) {
                        Consume(TokenType::Comma);
                    }
                    offsetList.push_back(i);
                    bool isEnd = ParserInitializer(initValues, members[i].cType, offsetList, true);
                    offsetList.pop_back();
                    if (isEnd) {
                        break;
                    }
                }
            } else {
                if (members.size() > 0) {
                    offsetList.push_back(0);
                    ParserInitializer(initValues, members[0].cType, offsetList, true);
                    offsetList.pop_back();
                }
            }
        }
        Consume(TokenType::RightBrace);
    } else {
        Token tmp      = token;
        auto initNode  = ParserAssignExpr();
        auto initValue = sema.SemaDeclInitValue(initNode, declTy, offsetList, tmp);
        initValues.push_back(initValue);
    }
    return false;
}

/// @brief block-stmt : "{" stmt* "}"
std::shared_ptr<ASTNode> Parser::ParserBlockStmt() {
    sema.EnterScope();
    Consume(TokenType::LeftBrace);
    auto blockStmts = std::make_shared<BlockStmts>();
    while (token.tokenTy != TokenType::RightBrace) {
        auto stmt = ParserStmt();
        if (stmt) {
            blockStmts->nodeVec.push_back(stmt);
        }
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
    std::shared_ptr<ASTNode> initNode = nullptr, condNode = nullptr, thenNode = nullptr, bodyNode = nullptr;
    if (IsTypeName(token.tokenTy)) {
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
        GetDiagnostics().Report(llvm::SMLoc::getFromPointer(token.ptr), diag::error_break_not_in_loop);
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
        GetDiagnostics().Report(llvm::SMLoc::getFromPointer(token.ptr), diag::error_continue_not_in_loop);
    }
    Consume(TokenType::KW_continue);
    Consume(TokenType::Semi);
    auto node        = std::make_shared<ContinueStmt>();
    node->fatherNode = nodesContainContinue.back();
    return node;
}

/// @brief expr : assign-expr (, assign-expr)*
std::shared_ptr<ASTNode> Parser::ParserExpr() {
    auto leftNode = ParserAssignExpr();
    while (token.tokenTy == TokenType::Comma) {
        Consume(TokenType::Comma);
        auto rightNode = ParserAssignExpr();
        leftNode       = sema.SemaBinaryExprNode(leftNode, BinOpCode::Comma, rightNode);
    }
    return leftNode;
}

/// @brief assign-expr : conditional-expr ("="|"+="|"-="|"*="|"/="|"%="|"|="|"&="|"^="|"<<="|">>=" assign-expr)+
std::shared_ptr<ASTNode> Parser::ParserAssignExpr() {
    Token tmp     = token;
    auto leftNode = ParserConditionalExpr();
    if (!IsAssignOperation()) {
        return leftNode;
    }

    BinOpCode op;
    switch (token.tokenTy) {
    case TokenType::Equal: {
        op = BinOpCode::Assign;
        break;
    }
    case TokenType::PlusEqual: {
        op = BinOpCode::AddAssign;
        break;
    }
    case TokenType::MinusEqual: {
        op = BinOpCode::SubAssign;
        break;
    }
    case TokenType::StarEqual: {
        op = BinOpCode::MulAssign;
        break;
    }
    case TokenType::SlashEqual: {
        op = BinOpCode::DivAssign;
        break;
    }
    case TokenType::PercentEqual: {
        op = BinOpCode::ModAssign;
        break;
    }
    case TokenType::LessLessEqual: {
        op = BinOpCode::LeftShiftAssign;
        break;
    }
    case TokenType::GreaterGreaterEqual: {
        op = BinOpCode::RightShiftAssign;
        break;
    }
    case TokenType::AmpEqual: {
        op = BinOpCode::AndAssign;
        break;
    }
    case TokenType::PipeEqual: {
        op = BinOpCode::OrAssign;
        break;
    }
    case TokenType::CaretEqual: {
        op = BinOpCode::XorAssign;
        break;
    }
    }
    Advance();
    return sema.SemaBinaryExprNode(leftNode, op, ParserAssignExpr());
}

/// @brief conditional-expr : logicor-expr ("?" expr ":" conditional)?
std::shared_ptr<ASTNode> Parser::ParserConditionalExpr() {
    auto leftNode = ParserLogicOrExpr();
    if (token.tokenTy != TokenType::Question) {
        return leftNode;
    }
    Consume(TokenType::Question);
    auto midNode = ParserExpr();
    Consume(TokenType::Colon);
    Token tmp      = token;
    auto rightNode = ParserConditionalExpr();
    return sema.SemaThreeExprNode(leftNode, midNode, rightNode, tmp);
}

/// @brief logicor-expr : logicand-expr ("||" logicand-expr)*
std::shared_ptr<ASTNode> Parser::ParserLogicOrExpr() {
    auto left = ParserLogicAndExpr();
    while (token.tokenTy == TokenType::PipePipe) {
        BinOpCode op = BinOpCode::LogicOr;
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
        BinOpCode op = BinOpCode::LogicAnd;
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
        BinOpCode op = BinOpCode::BitOr;
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
        BinOpCode op = BinOpCode::BitXor;
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
        BinOpCode op = BinOpCode::BitAnd;
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
        BinOpCode op;
        if (token.tokenTy == TokenType::EqualEqual) {
            op = BinOpCode::EqualEqual;
        } else {
            op = BinOpCode::NotEqual;
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
    while (token.tokenTy == TokenType::Less || token.tokenTy == TokenType::LessEqual || token.tokenTy == TokenType::Greater ||
           token.tokenTy == TokenType::GreaterEqual) {
        BinOpCode op;
        if (token.tokenTy == TokenType::Less) {
            op = BinOpCode::Less;
        } else if (token.tokenTy == TokenType::LessEqual) {
            op = BinOpCode::LessEqual;
        } else if (token.tokenTy == TokenType::Greater) {
            op = BinOpCode::Greater;
        } else {
            op = BinOpCode::GreaterEqual;
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

    // printf("%s", left->nodeKind);
    while (token.tokenTy == TokenType::LessLess || token.tokenTy == TokenType::GreaterGreater) {
        BinOpCode op;
        if (token.tokenTy == TokenType::LessLess) {
            op = BinOpCode::LeftShift;
        } else {
            op = BinOpCode::RightShift;
        }
        Advance();
        auto right = ParserAddExpr();
        // printf("%d", right->nodeKind);
        left = sema.SemaBinaryExprNode(left, op, right);
    }
    return left;
}

/// @brief  add-expr : mult-expr ( ("+" | "-") mult-expr)*
std::shared_ptr<ASTNode> Parser::ParserAddExpr() {
    auto left = ParserMultExpr();
    // a + b + c + d
    //             +
    //           /   \
    //         +      d
    //       /   \
    //      +     c
    //    /  \   
    // a(int) b(int)
    while (token.tokenTy == TokenType::Plus || token.tokenTy == TokenType::Minus) {
        BinOpCode op;
        if (token.tokenTy == TokenType::Plus) {
            op = BinOpCode::Add;
        } else {
            op = BinOpCode::Sub;
        }
        Advance();

        auto right = ParserMultExpr();
        left       = sema.SemaBinaryExprNode(left, op, right);
    }
    return left;
}

/// @brief mult-expr : unary-expr  ( ("*" | "/" | "%") unary-expr )*
std::shared_ptr<ASTNode> Parser::ParserMultExpr() {
    auto left = ParserUnaryExpr();
    // a * b * c * d...
    while (token.tokenTy == TokenType::Star || token.tokenTy == TokenType::Slash || token.tokenTy == TokenType::Percent) {
        BinOpCode op;
        if (token.tokenTy == TokenType::Star) {
            op = BinOpCode::Mul;
        } else if (token.tokenTy == TokenType::Slash) {
            op = BinOpCode::Div;
        } else {
            op = BinOpCode::Mod;
        }
        Advance();
        left = sema.SemaBinaryExprNode(left, op, ParserUnaryExpr());
    }
    return left;
}

/// @brief unary-expr : postfix-expr | ("++"|"--"|"&"|"*"|"-"|"~"|"!"|"sizeof") unary-expr | "sizeof" "(" type-name ")"
std::shared_ptr<ASTNode> Parser::ParserUnaryExpr() {
    // auto node = ;
    if (!IsUnaryOperation()) {
        return ParserPostfixExpr();
    }

    if (token.tokenTy == TokenType::KW_Sizeof) {
        Consume(TokenType::KW_Sizeof);

        bool isTypeName = false;
        if (token.tokenTy == TokenType::LeftParent) {
            lexer.SaveState();
            Token nextTok;
            lexer.NextToken(nextTok);
            isTypeName = IsTypeName(nextTok.tokenTy);
            lexer.RestoreState();
        }

        std::shared_ptr<CType> sizeofTy     = nullptr;
        std::shared_ptr<ASTNode> sizeofExpr = nullptr;
        if (isTypeName) {
            Consume(TokenType::LeftParent);
            sizeofTy = ParserType();
            Consume(TokenType::RightParent);
        } else {
            sizeofExpr = ParserUnaryExpr();
        }
        auto sizeofNode = sema.SemaSizeofExprNode(sizeofExpr, sizeofTy);
        return sizeofNode;
    }

    UnaryOpCode op;
    switch (token.tokenTy) {
    case TokenType::PlusPlus: {
        op = UnaryOpCode::Inc;
        break;
    }
    case TokenType::MinusMinus: {
        op = UnaryOpCode::Dec;
        break;
    }
    case TokenType::Amp: {
        op = UnaryOpCode::Addr;
        break;
    }
    case TokenType::Star: {
        op = UnaryOpCode::Deref;
        break;
    }
    case TokenType::Minus: {
        op = UnaryOpCode::Negative;
        break;
    }
    case TokenType::Plus: {
        op = UnaryOpCode::Positive;
        break;
    }
    case TokenType::Tilde: {
        op = UnaryOpCode::BitNot;
        break;
    }
    case TokenType::Exclaim: {
        op = UnaryOpCode::LogicNot;
        break;
    }
    }
    Advance();
    Token tmp      = token;
    auto unaryNode = sema.SemaUnaryExprNode(op, ParserUnaryExpr(), token);
    return unaryNode;
}

/// @brief postfix-expr : primary-expr | postfix-expr ("++" | "--")* | postfix-expr "[" expr "]"
std::shared_ptr<ASTNode> Parser::ParserPostfixExpr() {
    auto left = ParserPrimaryExpr();
    while (true) {
        if (token.tokenTy == TokenType::PlusPlus) {
            left = sema.SemaPostIncExprNode(left);
            Consume(TokenType::PlusPlus);
            continue;
        }
        if (token.tokenTy == TokenType::MinusMinus) {
            left = sema.SemaPostDecExprNode(left);
            Consume(TokenType::MinusMinus);
            continue;
        }
        if (token.tokenTy == TokenType::LeftBracket) {
            Token tmp = token;
            Consume(TokenType::LeftBracket);
            left = sema.SemaPostSubscriptExprNode(left, ParserExpr(), tmp);
            Consume(TokenType::RightBracket);
            continue;
        }
        if (token.tokenTy == TokenType::Dot) {
            Consume(TokenType::Dot);
            left = sema.SemaPostMemberDotNode(left, token);
            Consume(TokenType::Identifier);
            continue;
        }
        if (token.tokenTy == TokenType::Arrow) {
            Consume(TokenType::Arrow);
            left = sema.SemaPostMemberArrowNode(left, token);
            Consume(TokenType::Identifier);
            continue;
        }
        break;
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

/// @brief like `sizeof(int**[4][3])`
std::shared_ptr<CType> Parser::ParserType() {
    std::shared_ptr<CType> baseType = nullptr;
    if (token.tokenTy == TokenType::KW_int) {
        baseType = CType::IntType;
    }
    assert(baseType);

    Consume(token.tokenTy);
    while (token.tokenTy == TokenType::Star) {
        baseType = std::make_shared<CPointType>(baseType);
        Consume(TokenType::Star);
    }

    if (token.tokenTy == TokenType::LeftBracket) {
        baseType = ParserDirectDeclaratorArraySuffix(baseType);
    }

    return baseType;
}

bool Parser::IsTypeName(TokenType ty) {
    if (ty == TokenType::KW_int) {
        return true;
    } else if (ty == TokenType::KW_sturct || ty == TokenType::KW_union) {
        return true;
    }
    return false;
}

bool Parser::IsAssignOperation() {
    return token.tokenTy == TokenType::Equal                  ///< a = 1
           || token.tokenTy == TokenType::PlusPlus            ///< a++
           || token.tokenTy == TokenType::PlusEqual           ///< a += 1
           || token.tokenTy == TokenType::MinusMinus          ///< a--
           || token.tokenTy == TokenType::MinusEqual          ///< a -= 1
           || token.tokenTy == TokenType::StarEqual           ///< a *= 1
           || token.tokenTy == TokenType::SlashEqual          ///< a /= 1
           || token.tokenTy == TokenType::PercentEqual        ///< a %= 1
           || token.tokenTy == TokenType::LessLessEqual       ///< a <<= 1
           || token.tokenTy == TokenType::GreaterGreaterEqual ///< a >>= 1
           || token.tokenTy == TokenType::AmpEqual            ///< a &= 1
           || token.tokenTy == TokenType::PipeEqual           ///< a |= 1
           || token.tokenTy == TokenType::CaretEqual;         ///< a ^= 1
}

bool Parser::IsUnaryOperation() {
    return token.tokenTy == TokenType::PlusPlus      ///< a++
           || token.tokenTy == TokenType::MinusMinus ///< a--
           || token.tokenTy == TokenType::Amp        ///< &a
           || token.tokenTy == TokenType::Star       ///< *a
           || token.tokenTy == TokenType::Minus      ///< -1
           || token.tokenTy == TokenType::Plus       ///< +1
           || token.tokenTy == TokenType::Tilde      ///< ~a
           || token.tokenTy == TokenType::Exclaim    ///< !a
           || token.tokenTy == TokenType::KW_Sizeof; ///< sizeof
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