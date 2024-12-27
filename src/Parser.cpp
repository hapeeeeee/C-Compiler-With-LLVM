#include "include/Parser.h"

Parser::Parser(Lexer &lex) : lexer(lex) {
    Advance();
}

/// @brief prog : (expr? ";")*
std::shared_ptr<Program> Parser::ParserProgram() {
    std::vector<std::shared_ptr<Expr>> exprs;
    while (token.tokenTy != TokenType::Eof) {
        if (token.tokenTy == TokenType::Semi) {
            Advance();
            continue;
        }
        auto expr = ParserExpr();
        exprs.push_back(expr);
    }
    auto program = std::make_shared<Program>(std::move(exprs));
    return program;
}

/// @brief expr : term(("+" | "-") term)*
std::shared_ptr<Expr> Parser::ParserExpr() {
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
std::shared_ptr<Expr> Parser::ParserTerm() {
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

/// @brief factor : number | "(" expr")";
std::shared_ptr<Expr> Parser::ParserFactor() {
    if (token.tokenTy == TokenType::LeftParent) {
        Advance();
        auto expr = ParserExpr();
        assert(IsExcept(TokenType::RightParent));
        Advance();
        return expr;
    }

    auto factorExpr = std::make_shared<FactorExpr>(token.value);
    Advance();
    return factorExpr;
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