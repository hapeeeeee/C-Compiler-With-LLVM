#pragma once
#ifndef _CODEGEN_H_
#define _CODEGEN_H_
#include "Ast.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

/// @brief Code generation class for generating LLVM IR.
/// @details This class implements the `Visitor` interface to traverse the Abstract Syntax Tree
/// (AST) and generate corresponding LLVM IR code. It manages an LLVM context, IR builder, and
/// module, which are used to create IR instructions and define program structure.
///
/// The class also maintains a mapping between variable names and their
/// corresponding LLVM IR values and types, enabling efficient code generation
/// for variable declarations, assignments, and accesses.
class CodeGen : public Visitor {
  public:
    CodeGen(std::shared_ptr<Program> program);
    llvm::Value *VisitProgram(Program *program) override;
    llvm::Value *VisitVariableDecl(VariableDecl *VariableDecl) override;
    llvm::Value *VisitBinaryExpr(BinaryExpr *binaryExpr) override;
    llvm::Value *VisitNumberExpr(NumberExpr *numberExpr) override;
    llvm::Value *VisitVariableAssessExpr(VariableAssessExpr *variableAssessExpr) override;
    llvm::Value *VisitAssignExpr(AssignExpr *assignExpr) override;

  private:
    llvm::LLVMContext llvmContext;
    llvm::IRBuilder<> irBuilder{llvmContext};
    std::shared_ptr<llvm::Module> llvmModule;

    llvm::StringMap<std::pair<llvm::Value *, llvm::Type *>> varAddrTypeMap;
};

#endif // _CODEGEN_H_