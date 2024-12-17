#include "Lexer.h"
#include "llvm/Support/ErrorOr.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        llvm::outs() << "Error " << argv[0] << ": no input file\n";
        return 0;
    }

    const char *file_name = argv[1];
    static llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> buf =
        llvm::MemoryBuffer::getFile(file_name);

    if (!buf) {
        llvm::errs() << "can't open file: " << file_name << "\n";
        return -1;
    }

    std::unique_ptr<llvm::MemoryBuffer> memBuf = std::move(*buf);
    Lexer lex(memBuf->getBuffer());
    Token tok;
    while (tok.tokenTy != TokenType::Eof) {
        lex.NextToken(tok);
        tok.Dump();
    }
    return 0;
}
