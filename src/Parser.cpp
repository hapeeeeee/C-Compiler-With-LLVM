#include "include/Parser.h"

Parser::Parser(Lexer &lex, Sema &sema) : lexer(lex), sema(sema) {
    Advance();
}

/// @brief prog : (decl-stmt | expr-stmt)*
std::shared_ptr<Program> Parser::ParserProgram() {
    std::vector<std::shared_ptr<ASTNode>> stmts;
    while (token.tokenTy != TokenType::Eof) {
        if (token.tokenTy == TokenType::Semi) {
            Advance();
            continue;
        } else if (token.tokenTy == TokenType::KW_int) {
            const auto &exprs = ParserDecl();
            stmts.insert(stmts.end(), exprs.begin(), exprs.end());
        } else {
            auto expr = ParserExpr();
            stmts.push_back(expr);
        }
    }
    auto program = std::make_shared<Program>(std::move(stmts));
    return program;
}

/// @brief decl-stam : type-decl identifier ("=" expr)? (, identifier ("=" expr))* ";"
std::vector<std::shared_ptr<ASTNode>> Parser::ParserDecl() {
    Consume(TokenType::KW_int);
    CType *cTy = CType::getIntTy();

    std::vector<std::shared_ptr<ASTNode>> declArr;
    // int a = 1, c = 2, d;
    while (token.tokenTy != TokenType::Semi) {
        auto variableDecl = sema.SemaVariableDeclNode(cTy, token);
        declArr.push_back(variableDecl);
        llvm::StringRef variableName = token.content;
        assert(Consume(TokenType::Identifier));

        if (token.tokenTy == TokenType::Equal) {
            Advance();
            auto left       = std::make_shared<VariableAssessExpr>();
            left->name      = variableName;
            auto right      = ParserExpr();
            auto assignExpr = std::make_shared<AssignExpr>(left, right);
            declArr.push_back(assignExpr);
        }

        if (token.tokenTy == TokenType::Comma) {
            Advance();
        }
    }
    Consume(TokenType::Semi);
    return declArr;
}

/// @brief expr : term(("+" | "-") term)*
std::shared_ptr<ASTNode> Parser::ParserExpr() {
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
        auto binExpr = std::make_shared<BinaryExpr>(left, op, right);
        left         = binExpr;
    }
    return left;
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
        auto binExpr = std::make_shared<BinaryExpr>(left, op, right);
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
        auto factorExpr  = std::make_shared<VariableAssessExpr>();
        factorExpr->name = token.content;
        Advance();
        return factorExpr;
    } else {
        auto factorExpr   = std::make_shared<NumberExpr>(token.value);
        factorExpr->cType = token.cType;
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