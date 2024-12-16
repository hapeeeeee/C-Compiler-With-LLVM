#include "Lexer.h"

Lexer::Lexer(llvm::StringRef sourceCode) {
    workPtr        = sourceCode.begin();
    eofPtr         = sourceCode.end();
    workRowHeadPtr = sourceCode.begin();
    workRow        = 1;
}

bool Lexer::IsWhiteSpace(char ch) {
    return ch == ' ' || ch == '\r' || ch == '\n';
}

bool Lexer::IsDigit(char ch) {
    return ch >= '0' && ch <= '9';
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

    if (IsDigit(*workPtr)) {
        int number = 0;
        while (IsDigit(*workPtr)) {
            number = number * 10 + (*workPtr - '0');
        }
        tok.tokenTy = TokenType::Number;
        tok.value   = number;
    }
}
