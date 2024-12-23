#pragma once
#ifndef _CODEGEN_H_
#define _CODEGEN_H_
#include "Ast.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

class CodeGen : public Visitor {
  public:
    CodeGen(std::shared_ptr<Program> program);
    void VisitProgram(Program *program) override;
    void VisitBinaryExpr(BinaryExpr *binaryExpr) override;
    void VisitFactorExpr(FactorExpr *factorExpr) override;

  private:
    llvm::LLVMContext llvmContext;
    llvm::IRBuilder<> irBuilder{llvmContext};
    std::shared_ptr<llvm::Module> llvmModule;
};

#endif // _CODEGEN_H_