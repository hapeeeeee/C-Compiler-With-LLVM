#include "include/Parser.h"

Parser::Parser(Lexer &lex, Sema &sema) : lexer(lex), sema(sema) {
    Advance();
}

/// @brief  prog : stmt*
///         stmt : decl-stmt | expr-stmt | null-stmt
std::shared_ptr<Program> Parser::ParserProgram() {
    std::vector<std::shared_ptr<ASTNode>> stmts;
    while (token.tokenTy != TokenType::Eof) {
        if (token.tokenTy == TokenType::Semi) { ///< null-stmt
            Advance();
            continue;
        } else if (token.tokenTy == TokenType::KW_int) { ///< decl-stmt
            const auto &exprs = ParserDeclStmt();
            stmts.insert(stmts.end(), exprs.begin(), exprs.end());
        } else { ///< expr-stmt
            auto expr = ParserExprStmt();
            stmts.push_back(expr);
        }
    }
    auto program = std::make_shared<Program>(std::move(stmts));
    return program;
}

/// @brief decl-stmt : "int" identifier ("=" expr)? ("," identifier ("=" expr)?)* ";"
std::vector<std::shared_ptr<ASTNode>> Parser::ParserDeclStmt() {
    Consume(TokenType::KW_int);
    CType *cTy = CType::getIntTy();

    std::vector<std::shared_ptr<ASTNode>> declArr;
    // int a = 1, c = 2, d;
    while (token.tokenTy != TokenType::Semi) {
        Token variableToken = token;
        auto variableDecl   = sema.SemaVariableDeclNode(cTy, token);
        declArr.push_back(variableDecl);
        llvm::StringRef variableName = variableToken.content;
        assert(Consume(TokenType::Identifier));

        if (token.tokenTy == TokenType::Equal) {
            Advance();
            auto left       = sema.SemaVariableAccessExprNode(variableToken);
            auto right      = ParserExpr();
            auto assignExpr = sema.SemaAssignExprNode(left, right);
            declArr.push_back(assignExpr);
        }

        if (token.tokenTy == TokenType::Comma) {
            Advance();
        }
    }
    Consume(TokenType::Semi);
    return declArr;
}

/// @brief expr-stmt : expr ";"
std::shared_ptr<ASTNode> Parser::ParserExprStmt() {
    auto expr = ParserExpr();
    Consume(TokenType::Semi);
    return expr;
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
    Consume(TokenType::Equal);
    return sema.SemaAssignExprNode(leftExpr, ParserExpr());
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
        auto factorExpr = sema.SemaNumberExprNode(token.cType, token);
        Advance();
        return factorExpr;
    }
}

bool Parser::IsExcept(TokenType tokTy) {
    return token.tokenTy == tokTy;
}

bool Parser::Consume(TokenType tokTy) {
    if (IsExcept(tokTy)) {
        Advance();
        return true;
    }
    return false;
}

void Parser::Advance() {
    lexer.NextToken(token);
}