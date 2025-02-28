#pragma once
#ifndef _CODEGEN_H_
#define _CODEGEN_H_
#include "Ast.h"
#include "llvm/ADT/DenseMap.h"
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
class CodeGen : public Visitor, public TypeVisitor {
  public:
    CodeGen(std::shared_ptr<Program> program);

    std::unique_ptr<llvm::Module> &GetModule() {
        return llvmModule;
    }

    llvm::Value *VisitProgram(Program *program) override;
    llvm::Value *VisitDeclStmts(DeclStmts *declStmts) override;
    llvm::Value *VisitBlockStmts(BlockStmts *blockStmts) override;
    llvm::Value *VisitVariableDecl(VariableDecl *VariableDecl) override;
    llvm::Value *VisitIfStmt(IfStmt *ifStmt) override;
    llvm::Value *VisitForStmt(ForStmt *forStmt) override;
    llvm::Value *VisitBreakStmt(BreakStmt *breakStmt) override;
    llvm::Value *VisitContinueStmt(ContinueStmt *continueStmt) override;
    llvm::Value *VisitBinaryExpr(BinaryExpr *binaryExpr) override;
    llvm::Value *VisitNumberExpr(NumberExpr *numberExpr) override;
    llvm::Value *VisitSizeofExpr(SizeofExpr *sizeofExpr) override;
    llvm::Value *VisitUnaryExpr(UnaryExpr *unaryExpr) override;
    llvm::Value *VisitThreeExpr(ThreeExpr *threeExpr) override;
    llvm::Value *VisitPostIncExpr(PostIncExpr *postIncExpr) override;
    llvm::Value *VisitPostDecExpr(PostDecExpr *postDecExpr) override;
    llvm::Value *VisitPostSubscriptExpr(PostSubscriptExpr *postSubscriptExpr) override;
    llvm::Value *VisitPostMemberDotExpr(PostMemberDotExpr *postMemberDotExpr) override;
    llvm::Value *VisitPostMemberArrowExpr(PostMemberArrowExpr *postMemberArrowExpr) override;
    llvm::Value *VisitVariableAssessExpr(VariableAssessExpr *variableAssessExpr) override;
    llvm::Value *VisitFuncDeclStmt(FuncDeclStmt *funcDeclStmt) override;
    llvm::Value *VisitReturnStmt(ReturnStmt *returnStmt) override;
    llvm::Value *VisitPostFuncCallExpr(PostFuncCallExpr *postFuncCallExpr) override;

    llvm::Type *VisitCPrimaryType(CPrimaryType *ty) override;
    llvm::Type *VisitCPointType(CPointType *ty) override;
    llvm::Type *VisitCArrayType(CArrayType *ty) override;
    llvm::Type *VisitCRecordType(CRecordType *ty) override;
    llvm::Type *VisitCFuncType(CFuncType *ty) override;

  private:
    llvm::LLVMContext llvmContext;
    llvm::IRBuilder<> irBuilder{llvmContext};
    std::unique_ptr<llvm::Module> llvmModule;
    llvm::Function *currFunc{nullptr};

    llvm::DenseMap<ASTNode *, llvm::BasicBlock *> breakTargetBBs;    ///< Target block for the break statement
    llvm::DenseMap<ASTNode *, llvm::BasicBlock *> continueTargetBBs; ///< Target block for the continue statement
    llvm::SmallVector<llvm::StringMap<std::pair<llvm::Value *, llvm::Type *>>> localVarAddrTypeMap; // local var in function
    llvm::StringMap<std::pair<llvm::Value *, llvm::Type *>> globalVarAddrTypeMap;                   // function & global var

  private:
    void PushScope();
    void PopScope();
    void ClearVarScope();

    void AddLocalVarToMap(llvm::StringRef name, llvm::Value *val, llvm::Type *ty);
    void AddGlobalVarToMap(llvm::StringRef name, llvm::Value *val, llvm::Type *ty);
    std::pair<llvm::Value *, llvm::Type *> GetVarByName(llvm::StringRef name);
    std::pair<llvm::Value *, llvm::Type *> GetGlobalVarByName(llvm::StringRef name);
};

#endif // _CODEGEN_H_