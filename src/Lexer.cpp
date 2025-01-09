#include "include/Lexer.h"

llvm::StringRef Token::GetSpellingText(TokenType ty) {
    switch (ty) {
    case TokenType::Number:
        return "number";
        break;
    case TokenType::Equal:
        return "=";
        break;
    case TokenType::Minus:
        return "-";
        break;
    case TokenType::Plus:
        return "+";
        break;
    case TokenType::Star:
        return "*";
        break;
    case TokenType::Slash:
        return "/";
        break;
    case TokenType::LeftParent:
        return "(";
        break;
    case TokenType::RightParent:
        return ")";
        break;
    case TokenType::Comma:
        return ",";
        break;
    case TokenType::Semi:
        return ";";
        break;
    case TokenType::Identifier:
        return "Identifier";
        break;
    case TokenType::KW_int:
        return "int";
        break;
    case TokenType::Eof:
        return "Eof";
        break;
    default:
        llvm::llvm_unreachable_internal();
        break;
    }
}

Lexer::Lexer(llvm::SourceMgr &mgr, Diagnostics &diag) : mgr(mgr), diager(diag) {
    unsigned int id     = mgr.getMainFileID();
    llvm::StringRef buf = mgr.getMemoryBuffer(id)->getBuffer();
    workPtr             = buf.begin();
    eofPtr              = buf.end();
    workRowHeadPtr      = buf.begin();
    workRow             = 1;
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
        tok.setMember(
            TokenType::Number, tokenStart, workPtr - tokenStart, number, CType::getIntTy());
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
            diager.Report(llvm::SMLoc::getFromPointer(workPtr), diag::error_unknown_char, workPtr);
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

void Lexer::SaveState() {
    state.eofPtr         = eofPtr;
    state.workPtr        = workPtr;
    state.workRow        = workRow;
    state.workRowHeadPtr = workRowHeadPtr;
}

void Lexer::RestoreState() {
    eofPtr         = state.eofPtr;
    workPtr        = state.workPtr;
    workRow        = state.workRow;
    workRowHeadPtr = state.workRowHeadPtr;
}

Diagnostics &Lexer::GetDiagnostics() {
    return diager;
}

void Lexer::KeyWordHandle(Token &tok) {
    if (llvm::StringRef(tok.ptr, tok.length) == "int") {
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
