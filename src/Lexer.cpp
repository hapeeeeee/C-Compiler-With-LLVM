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
        int number = 0, len = 0;
        while (IsDigit(*workPtr)) {
            number = number * 10 + (*workPtr - '0');
            workPtr++;
            len++;
        }
        tok.setMember(TokenType::Number, tokenStart, len, number);
    } else {
        switch (*workPtr) {
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

bool Lexer::IsWhiteSpace(char ch) {
    return ch == ' ' || ch == '\r' || ch == '\n';
}

bool Lexer::IsDigit(char ch) {
    return ch >= '0' && ch <= '9';
}