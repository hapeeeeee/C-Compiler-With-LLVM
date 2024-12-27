#include "include/Lexer.h"

Lexer::Lexer(llvm::StringRef sourceCode) {
    workPtr        = sourceCode.begin();
    eofPtr         = sourceCode.end();
    workRowHeadPtr = sourceCode.begin();
    workRow        = 1;
}

void Lexer::NextToken(Token &tok) {
    while (IsWhiteSpace(*workPtr)) {
        if (*workPtr == '\n') {
            workRow++;
            workRowHeadPtr = workPtr + 1;
        }
        workPtr++;
    }

    tok.row = workRow;
    tok.col = workPtr - workRowHeadPtr + 1;

    if (workPtr >= eofPtr) {
        tok.tokenTy = TokenType::Eof;
        return;
    }

    const char *tokenStart = workPtr;
    if (IsDigit(*workPtr)) {
        int number = 0;
        while (IsDigit(*workPtr)) {
            number = number * 10 + (*workPtr - '0');
            workPtr++;
        }
        tok.setMember(TokenType::Number, tokenStart, workPtr - tokenStart, number);
    } else if (IsLetter(*workPtr)) {
        while (IsLetter(*workPtr)) {
            workPtr++;
        }
        tok.setMember(TokenType::Identifier, tokenStart, workPtr - tokenStart);
        KeyWordHandle(tok);
    } else {
        switch (*workPtr) {
        case '=': {
            tok.setMember(TokenType::Equal, workPtr, 1);
            workPtr++;
            break;
        }
        case '+': {
            tok.setMember(TokenType::Plus, workPtr, 1);
            workPtr++;
            break;
        }
        case '-': {
            tok.setMember(TokenType::Minus, workPtr, 1);
            workPtr++;
            break;
        }
        case '*': {
            tok.setMember(TokenType::Star, workPtr, 1);
            workPtr++;
            break;
        }
        case '/': {
            tok.setMember(TokenType::Slash, workPtr, 1);
            workPtr++;
            break;
        }
        case '(': {
            tok.setMember(TokenType::LeftParent, workPtr, 1);
            workPtr++;
            break;
        }
        case ')': {
            tok.setMember(TokenType::RightParent, workPtr, 1);
            workPtr++;
            break;
        }
        case ',': {
            tok.setMember(TokenType::Comma, workPtr, 1);
            workPtr++;
            break;
        }
        case ';': {
            tok.setMember(TokenType::Semi, workPtr, 1);
            workPtr++;
            break;
        }
        default:
            tok.tokenTy = TokenType::Unknown;
            workPtr++;
            break;
        }
    }
}

void Lexer::Run(Token &tok) {
    NextToken(tok);
    while (tok.tokenTy != TokenType::Eof) {
        tok.Dump();
        NextToken(tok);
    }
}

void Lexer::KeyWordHandle(Token &tok) {
    if (tok.content == "int") {
        tok.tokenTy = TokenType::KW_int;
    }
}

bool Lexer::IsWhiteSpace(char ch) {
    return ch == ' ' || ch == '\r' || ch == '\n';
}

bool Lexer::IsDigit(char ch) {
    return ch >= '0' && ch <= '9';
}

bool Lexer::IsLetter(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_';
}
