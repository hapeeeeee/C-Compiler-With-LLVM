#pragma once
#ifndef _PRINTVISITOR_H_
#define _PRINTVISITOR_H_
#include "Ast.h"

class PrintVisitor : public Visitor, public TypeVisitor {
  public:
    llvm::raw_ostream *out;

  public:
    PrintVisitor(std::shared_ptr<Program> program, llvm::raw_ostream *out);
    llvm::Value *VisitProgram(Program *program) override;
    llvm::Value *VisitDeclStmts(DeclStmts *declStmts) override;
    llvm::Value *VisitVariableDecl(VariableDecl *variableDecl) override;
    llvm::Value *VisitBlockStmts(BlockStmts *blockStmts) override;
    llvm::Value *VisitIfStmt(IfStmt *ifStmt) override;
    llvm::Value *VisitForStmt(ForStmt *forStmt) override;
    llvm::Value *VisitBreakStmt(BreakStmt *breakStmt) override;
    llvm::Value *VisitContinueStmt(ContinueStmt *continueStmt) override;
    llvm::Value *VisitSizeofExpr(SizeofExpr *sizeofExpr) override;
    llvm::Value *VisitUnaryExpr(UnaryExpr *unaryExpr) override;
    llvm::Value *VisitBinaryExpr(BinaryExpr *binaryExpr) override;
    llvm::Value *VisitThreeExpr(ThreeExpr *threeExpr) override;
    llvm::Value *VisitPostIncExpr(PostIncExpr *postIncExpr) override;
    llvm::Value *VisitPostDecExpr(PostDecExpr *postDecExpr) override;
    llvm::Value *VisitPostSubscriptExpr(PostSubscriptExpr *postSubscriptExpr) override;
    llvm::Value *VisitNumberExpr(NumberExpr *numberExpr) override;
    llvm::Value *VisitVariableAssessExpr(VariableAssessExpr *variableAssessExpr) override;

    llvm::Type *VisitCPrimaryType(CPrimaryType *ty) override;
    llvm::Type *VisitCPointType(CPointType *ty) override;
    llvm::Type *VisitCArrayType(CArrayType *ty) override;
};

#endif // _PRINTVISITOR_H_
