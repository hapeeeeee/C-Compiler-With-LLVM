#pragma once
#ifndef _PRINTVISITOR_H_
#define _PRINTVISITOR_H_
#include "Ast.h"

class PrintVisitor : public Visitor, public TypeVisitor {
  public:
    PrintVisitor(std::shared_ptr<Program> program);
    virtual llvm::Value *VisitProgram(Program *program) override;
    virtual llvm::Value *VisitDeclStmts(DeclStmts *declStmts) override;
    virtual llvm::Value *VisitVariableDecl(VariableDecl *variableDecl) override;
    virtual llvm::Value *VisitBlockStmts(BlockStmts *blockStmts) override;
    virtual llvm::Value *VisitIfStmt(IfStmt *ifStmt) override;
    virtual llvm::Value *VisitForStmt(ForStmt *forStmt) override;
    virtual llvm::Value *VisitBreakStmt(BreakStmt *breakStmt) override;
    virtual llvm::Value *VisitContinueStmt(ContinueStmt *continueStmt) override;
    virtual llvm::Value *VisitSizeofExpr(SizeofExpr *sizeofExpr) override;
    virtual llvm::Value *VisitUnaryExpr(UnaryExpr *unaryExpr) override;
    virtual llvm::Value *VisitBinaryExpr(BinaryExpr *binaryExpr) override;
    virtual llvm::Value *VisitThreeExpr(ThreeExpr *threeExpr) override;
    virtual llvm::Value *VisitPostIncExpr(PostIncExpr *postIncExpr) override;
    virtual llvm::Value *VisitPostDecExpr(PostDecExpr *postDecExpr) override;
    virtual llvm::Value *VisitNumberExpr(NumberExpr *numberExpr) override;
    virtual llvm::Value *VisitVariableAssessExpr(VariableAssessExpr *variableAssessExpr) override;

    virtual llvm::Type *VisitCPrimaryType(CPrimaryType *ty) override;
    virtual llvm::Type *VisitCPointType(CPointType *ty) override;
};

#endif // _PRINTVISITOR_H_
