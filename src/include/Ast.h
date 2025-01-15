#pragma once
#ifndef _AST_H_
#define _AST_H_

#include "CType.h"
#include "Lexer.h"

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
class DeclStmts;
class BlockStmts;
class IfStmt;
class ForStmt;
class BreakStmt;
class ContinueStmt;

/// @brief Base class for the visitor in the Visitor design pattern.
/// @details This class defines a set of pure virtual functions to visit different nodes of an
/// Abstract Syntax Tree (AST). By inheriting from this class and implementing its virtual
/// functions, specific visitors can be created, such as `PrintVisitor` for printing AST node
/// information or `CodeGen` for generating LLVM IR code. This class enables functionality to be
/// extended without modifying the AST node classes.
class Visitor {
  public:
    virtual ~Visitor() {
    }
    virtual llvm::Value *VisitProgram(Program *program)                                  = 0;
    virtual llvm::Value *VisitDeclStmts(DeclStmts *declStmts)                            = 0;
    virtual llvm::Value *VisitVariableDecl(VariableDecl *variableDecl)                   = 0;
    virtual llvm::Value *VisitBlockStmts(BlockStmts *blockStmts)                         = 0;
    virtual llvm::Value *VisitIfStmt(IfStmt *ifStmt)                                     = 0;
    virtual llvm::Value *VisitForStmt(ForStmt *forStmt)                                  = 0;
    virtual llvm::Value *VisitBreakStmt(BreakStmt *breakStmt)                            = 0;
    virtual llvm::Value *VisitContinueStmt(ContinueStmt *continueStmt)                   = 0;
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
        ND_DeclStmts = 0,
        ND_BlockStmts,
        ND_VariableDecl,
        ND_IfStmt,
        ND_ForStmt,
        ND_BreakStmt,
        ND_ContinueStmt,
        ND_BinaryExpr,
        ND_NumberExpr,
        ND_VariableAssessExpr,
        ND_AssignExpr,
    };

  public:
    CType *cType;
    Nodekind nodeKind;
    Token token;

  public:
    ASTNode(Nodekind kind) : nodeKind(kind) {
    }
    virtual ~ASTNode() {
    }
    virtual llvm::Value *AcceptVisitor(Visitor *v) {
        return nullptr;
    }
};

class DeclStmts : public ASTNode {
  public:
    std::vector<std::shared_ptr<ASTNode>> nodeVec;

  public:
    DeclStmts() : ASTNode(Nodekind::ND_DeclStmts) {
    }

    llvm::Value *AcceptVisitor(Visitor *v) override {
        return v->VisitDeclStmts(this);
    }

    static bool classof(const ASTNode *node) {
        return node->nodeKind == Nodekind::ND_DeclStmts;
    }
};

class VariableDecl : public ASTNode {
  public:
    VariableDecl() : ASTNode(Nodekind::ND_VariableDecl) {
    }

    llvm::Value *AcceptVisitor(Visitor *v) override {
        return v->VisitVariableDecl(this);
    }

    static bool classof(const ASTNode *node) {
        return node->nodeKind == Nodekind::ND_VariableDecl;
    }
};

class BlockStmts : public ASTNode {
  public:
    std::vector<std::shared_ptr<ASTNode>> nodeVec;

  public:
    BlockStmts() : ASTNode(Nodekind::ND_BlockStmts) {
    }

    llvm::Value *AcceptVisitor(Visitor *v) override {
        return v->VisitBlockStmts(this);
    }

    static bool classof(const ASTNode *node) {
        return node->nodeKind == Nodekind::ND_BlockStmts;
    }
};

class IfStmt : public ASTNode {
  public:
    std::shared_ptr<ASTNode> condExpr;
    std::shared_ptr<ASTNode> thenStmt;
    std::shared_ptr<ASTNode> elseStmt;

  public:
    IfStmt() : ASTNode(Nodekind::ND_IfStmt) {
    }

    llvm::Value *AcceptVisitor(Visitor *v) override {
        return v->VisitIfStmt(this);
    }

    static bool classof(const ASTNode *node) {
        return node->nodeKind == Nodekind::ND_IfStmt;
    }
};

class ForStmt : public ASTNode {
  public:
    std::shared_ptr<ASTNode> initNode;
    std::shared_ptr<ASTNode> condNode;
    std::shared_ptr<ASTNode> thenNode;
    std::shared_ptr<ASTNode> bodyNode;

  public:
    ForStmt() : ASTNode(Nodekind::ND_ForStmt) {
    }

    llvm::Value *AcceptVisitor(Visitor *v) override {
        return v->VisitForStmt(this);
    }

    static bool classof(const ASTNode *node) {
        return node->nodeKind == Nodekind::ND_ForStmt;
    }
};

class BreakStmt : public ASTNode {
  public:
    std::shared_ptr<ASTNode> fatherNode;

  public:
    BreakStmt() : ASTNode(Nodekind::ND_BreakStmt) {
    }

    llvm::Value *AcceptVisitor(Visitor *v) override {
        return v->VisitBreakStmt(this);
    }

    static bool classof(const ASTNode *node) {
        return node->nodeKind == Nodekind::ND_BreakStmt;
    }
};

class ContinueStmt : public ASTNode {
  public:
    std::shared_ptr<ASTNode> fatherNode;

  public:
    ContinueStmt() : ASTNode(Nodekind::ND_ContinueStmt) {
    }

    llvm::Value *AcceptVisitor(Visitor *v) override {
        return v->VisitContinueStmt(this);
    }

    static bool classof(const ASTNode *node) {
        return node->nodeKind == Nodekind::ND_ContinueStmt;
    }
};

enum class OpCode {
    Add = 0,      ///< +
    Sub,          ///< -
    Mul,          ///< *
    Div,          ///< /
    EqualEqual,   ///< ==
    NotEqual,     ///< !=
    Less,         ///< <
    Greater,      ///< >
    LessEqual,    ///< <=
    GreaterEqual, ///< >=
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

    static bool classof(const ASTNode *node) {
        return node->nodeKind == Nodekind::ND_BinaryExpr;
    }
};

class NumberExpr : public ASTNode {
  public:
    NumberExpr() : ASTNode(Nodekind::ND_NumberExpr) {
    }

    llvm::Value *AcceptVisitor(Visitor *v) override {
        return v->VisitNumberExpr(this);
    }

    static bool classof(const ASTNode *node) {
        return node->nodeKind == Nodekind::ND_NumberExpr;
    }
};

class VariableAssessExpr : public ASTNode {
  public:
    VariableAssessExpr() : ASTNode(Nodekind::ND_VariableAssessExpr) {
    }

    llvm::Value *AcceptVisitor(Visitor *v) override {
        return v->VisitVariableAssessExpr(this);
    }

    static bool classof(const ASTNode *node) {
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

    static bool classof(const ASTNode *node) {
        return node->nodeKind == Nodekind::ND_AssignExpr;
    }
};

#endif // _AST_H_