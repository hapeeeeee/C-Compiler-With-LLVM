#include "llvm/Support/ErrorOr.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"

#include "include/CodeGen.h"
#include "include/Diagnostics.h"
#include "include/Lexer.h"
#include "include/Parser.h"
#include "include/PrintVisitor.h"
#include "include/Sema.h"

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

    llvm::SourceMgr mgr;
    Diagnostics diag(mgr);
    mgr.AddNewSourceBuffer(std::move(*buf), llvm::SMLoc());

    // std::unique_ptr<llvm::MemoryBuffer> memBuf = std::move(*buf);
    Lexer lex(mgr, diag);
    Token tok;
    // lex.Run(tok);
    Sema sema(diag);
    Parser parser(lex, sema);
    std::shared_ptr<Program> program = parser.ParserProgram();
    // PrintVisitor printVisitor(program);
    CodeGen codeGen(program);

    return 0;
}
