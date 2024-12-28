#pragma once
#ifndef _AST_H_
#define _AST_H_

#include "CType.h"
#include "llvm/IR/Value.h"
#include <memory>
#include <vector>

class Program;
class ASTNode;
class VariableDecl;
class BinaryExpr;
class NumberExpr;
class VariableAssessExpr;
class AssignExpr;

/// @brief
class Visitor {
  public:
    virtual ~Visitor() {
    }
    virtual llvm::Value *VisitProgram(Program *program)                                  = 0;
    virtual llvm::Value *VisitVariableDecl(VariableDecl *variableDecl)                   = 0;
    virtual llvm::Value *VisitBinaryExpr(BinaryExpr *binaryExpr)                         = 0;
    virtual llvm::Value *VisitNumberExpr(NumberExpr *numberExpr)                         = 0;
    virtual llvm::Value *VisitVariableAssessExpr(VariableAssessExpr *variableAssessExpr) = 0;
    virtual llvm::Value *VisitAssignExpr(AssignExpr *assignExpr)                         = 0;
};

class Program {
  public:
    std::vector<std::shared_ptr<ASTNode>> stmts;

  public:
    Program(std::vector<std::shared_ptr<ASTNode>> stmts);
    llvm::Value *AcceptVisitor(Visitor *v) {
        return v->VisitProgram(this);
    }
};

class ASTNode {
  public:
    enum Nodekind {
        ND_VariableDecl = 0,
        ND_BinaryExpr,
        ND_NumberExpr,
        ND_VariableAssessExpr,
        ND_AssignExpr,
    };

  public:
    CType *cType;
    Nodekind nodeKind;

  public:
    ASTNode(Nodekind kind) : nodeKind(kind) {
    }
    virtual ~ASTNode() {
    }
    virtual llvm::Value *AcceptVisitor(Visitor *v) {
        return nullptr;
    }
};

class VariableDecl : public ASTNode {
  public:
    llvm::StringRef name;

  public:
    VariableDecl() : ASTNode(Nodekind::ND_VariableDecl) {
    }

    llvm::Value *AcceptVisitor(Visitor *v) override {
        return v->VisitVariableDecl(this);
    }

    static bool ClassOf(ASTNode *node) {
        return node->nodeKind == Nodekind::ND_VariableDecl;
    }
};

enum class OpCode {
    Add = 0,
    Sub,
    Mul,
    Div,
};

class BinaryExpr : public ASTNode {
  public:
    OpCode op;
    std::shared_ptr<ASTNode> leftExpr;
    std::shared_ptr<ASTNode> rightExpr;

  public:
    BinaryExpr(std::shared_ptr<ASTNode> left, OpCode op, std::shared_ptr<ASTNode> right)
        : leftExpr(left), op(op), rightExpr(right), ASTNode(Nodekind::ND_BinaryExpr) {
    }

    llvm::Value *AcceptVisitor(Visitor *v) override {
        return v->VisitBinaryExpr(this);
    }

    static bool ClassOf(ASTNode *node) {
        return node->nodeKind == Nodekind::ND_BinaryExpr;
    }
};

class NumberExpr : public ASTNode {
  public:
    int number;

  public:
    NumberExpr(int num) : number(num), ASTNode(Nodekind::ND_NumberExpr) {
    }

    llvm::Value *AcceptVisitor(Visitor *v) override {
        return v->VisitNumberExpr(this);
    }

    static bool ClassOf(ASTNode *node) {
        return node->nodeKind == Nodekind::ND_NumberExpr;
    }
};

class VariableAssessExpr : public ASTNode {
  public:
    llvm::StringRef name;

  public:
    VariableAssessExpr() : ASTNode(Nodekind::ND_VariableAssessExpr) {
    }

    llvm::Value *AcceptVisitor(Visitor *v) override {
        return v->VisitVariableAssessExpr(this);
    }

    static bool ClassOf(ASTNode *node) {
        return node->nodeKind == Nodekind::ND_VariableAssessExpr;
    }
};

class AssignExpr : public ASTNode {
  public:
    std::shared_ptr<ASTNode> leftExpr;
    std::shared_ptr<ASTNode> rightExpr;

  public:
    AssignExpr(std::shared_ptr<ASTNode> left, std::shared_ptr<ASTNode> right)
        : leftExpr(left), rightExpr(right), ASTNode(Nodekind::ND_AssignExpr) {
    }

    llvm::Value *AcceptVisitor(Visitor *v) override {
        return v->VisitAssignExpr(this);
    }

    static bool ClassOf(ASTNode *node) {
        return node->nodeKind == Nodekind::ND_AssignExpr;
    }
};

#endif // _AST_H_