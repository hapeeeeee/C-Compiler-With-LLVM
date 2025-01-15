#include "include/Lexer.h"

llvm::StringRef Token::GetSpellingText(TokenType ty) {
    switch (ty) {
    case TokenType::Number:
        return "number";
    case TokenType::Equal:
        return "=";
    case TokenType::EqualEqual:
        return "==";
    case TokenType::NotEqual:
        return "!=";
    case TokenType::Less:
        return "<";
    case TokenType::Greater:
        return ">";
    case TokenType::LessEqual:
        return "<=";
    case TokenType::GreaterEqual:
        return ">=";
    case TokenType::Minus:
        return "-";
    case TokenType::Plus:
        return "+";
    case TokenType::Star:
        return "*";
    case TokenType::Slash:
        return "/";
    case TokenType::LeftParent:
        return "(";
    case TokenType::RightParent:
        return ")";
    case TokenType::LeftBrace:
        return "{";
    case TokenType::RightBrace:
        return "}";
    case TokenType::Comma:
        return ",";
    case TokenType::Semi:
        return ";";
    case TokenType::Identifier:
        return "Identifier";
    case TokenType::KW_int:
        return "int";
    case TokenType::Eof:
        return "Eof";
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
        case '!': {
            const char *workNextPtr = workPtr + 1;
            if (*workNextPtr == '=') {
                tok.setMember(TokenType::NotEqual, workPtr, 2);
                workPtr += 2;
            } else {
                diager.Report(
                    llvm::SMLoc::getFromPointer(workPtr), diag::error_unknown_char, workPtr);
            }
            break;
        }
        case '=': {
            const char *workNextPtr = workPtr + 1;
            if (*workNextPtr == '=') {
                tok.setMember(TokenType::EqualEqual, workPtr, 2);
                workPtr++;
            } else {
                tok.setMember(TokenType::Equal, workPtr, 1);
            }
            workPtr++;
            break;
        }
        case '<': {
            const char *workNextPtr = workPtr + 1;
            if (*workNextPtr == '=') {
                tok.setMember(TokenType::LessEqual, workPtr, 2);
                workPtr++;
            } else {
                tok.setMember(TokenType::Less, workPtr, 1);
            }
            workPtr++;
            break;
        }
        case '>': {
            const char *workNextPtr = workPtr + 1;
            if (workNextPtr && *workNextPtr == '=') {
                tok.setMember(TokenType::GreaterEqual, workPtr, 2);
                workPtr++;
            } else {
                tok.setMember(TokenType::Greater, workPtr, 1);
            }
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
        case '{': {
            tok.setMember(TokenType::LeftBrace, workPtr, 1);
            workPtr++;
            break;
        }
        case '}': {
            tok.setMember(TokenType::RightBrace, workPtr, 1);
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
    } else if (llvm::StringRef(tok.ptr, tok.length) == "if") {
        tok.tokenTy = TokenType::KW_if;
    } else if (llvm::StringRef(tok.ptr, tok.length) == "else") {
        tok.tokenTy = TokenType::KW_else;
    } else if (llvm::StringRef(tok.ptr, tok.length) == "for") {
        tok.tokenTy = TokenType::KW_for;
    } else if (llvm::StringRef(tok.ptr, tok.length) == "break") {
        tok.tokenTy = TokenType::KW_break;
    } else if (llvm::StringRef(tok.ptr, tok.length) == "continue") {
        tok.tokenTy = TokenType::KW_continue;
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
