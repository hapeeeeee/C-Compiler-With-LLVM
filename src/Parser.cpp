#include "include/Parser.h"

Parser::Parser(Lexer &lex) : lexer(lex) {
    Advance();
}

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

/**
    prog   : (expr? ";")*
    expr   : term(("+" | "-") term)* ;
    term   : factor(("*" | "/") factor)* ;
    factor : number | "(" expr")";
    number : ([0-9])+;
 */

// 解析表达式
std::shared_ptr<Expr> Parser::ParserExpr() {
    // 解析左侧表达式
    auto left = ParserTerm();
    // 如果中间是加号或者减号 创建二元表达式
    // 这里要一直判断，因为会遇到a+b+c+d...的情况
    while (token.tokenTy == TokenType::Plus || token.tokenTy == TokenType::Minus) {
        OpCode op;
        if (token.tokenTy == TokenType::Plus) {
            op = OpCode::add;
        } else {
            op = OpCode::sub;
        }
        Advance();
        auto right   = ParserTerm();
        auto binExpr = std::make_shared<BinaryExpr>(left, op, right);
        left         = binExpr;
    }

    // 返回新表达式
    return left;
}

std::shared_ptr<Expr> Parser::ParserTerm() {
}

std::shared_ptr<Expr> Parser::ParserFactor() {
}

bool Parser::IsExcept(TokenType tokTy) {
}
// 消费这个Token
bool Parser::Consume(TokenType tokTy) {
}
// 前进一个Token
void Parser::Advance() {
    lexer.NextToken(token);
}