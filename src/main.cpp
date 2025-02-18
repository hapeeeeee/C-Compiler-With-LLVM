#include "llvm/Support/ErrorOr.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"

#include "include/CodeGen.h"
#include "include/Diagnostics.h"
#include "include/Lexer.h"
#include "include/Parser.h"
#include "include/PrintVisitor.h"
#include "include/Sema.h"

// #define LLVM_JIT
#ifdef LLVM_JIT
#include "llvm/IR/Verifier.h"
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/Support/TargetSelect.h>
#endif

int main(int argc, char *argv[]) {
#ifdef LLVM_JIT
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    LLVMLinkInMCJIT();
#endif
    if (argc < 2) {
        llvm::outs() << "Error " << argv[0] << ": no input file\n";
        return 0;
    }

    const char *file_name                                         = argv[1];
    static llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> buf = llvm::MemoryBuffer::getFile(file_name);

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

    std::string s;
    llvm::raw_string_ostream ss(s);
    PrintVisitor printVisitor(program, &ss);
    llvm::outs() << s;

    // CodeGen codegen(program);
    // auto &module = codegen.GetModule();
    // {
    //     llvm::EngineBuilder builder(std::move(module));
    //     std::string error;
    //     auto ptr = std::make_unique<llvm::SectionMemoryManager>();
    //     auto ref = ptr.get();
    //     std::unique_ptr<llvm::ExecutionEngine> ee(builder.setErrorStr(&error)
    //                                                   .setEngineKind(llvm::EngineKind::JIT)
    //                                                   .setOptLevel(llvm::CodeGenOptLevel::None)
    //                                                   .setSymbolResolver(std::move(ptr))
    //                                                   .create());
    //     ref->finalizeMemory(&error);
    //     void *addr = (void *)ee->getFunctionAddress("main");
    //     int res    = ((int (*)())addr)();
    //     llvm::errs() << "result: " << res << "\n";
    // }

    return 0;
}
