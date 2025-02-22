#pragma once
#ifndef _SEMA_H_
#define _SEMA_H_

#include "Ast.h"
#include "Lexer.h"
#include "Scope.h"

/// @brief Performs semantic analysis for the program.
/// @details The `Sema` class is responsible for performing semantic analysis on Abstract Syntax
/// Tree (AST) nodes. It validates and processes various constructs in the program, such as variable
/// declarations, expressions, and operations. It ensures that the program adheres to semantic rules
/// and prepares the AST for further compilation stages.
class Sema {
  public:
    enum Mode {
        Normal = 0,
        Skip,
    };

  public:
    Sema(Diagnostics &diager) : diager(diager), mode(Mode::Normal) {
    }
    std::shared_ptr<ASTNode> SemaFuncDecl(std::shared_ptr<CType> funcTy, std::shared_ptr<ASTNode> blockStmt, Token &tok);
    std::shared_ptr<ASTNode>
    SemaBlockStmtNode(std::shared_ptr<ASTNode> condExpr, std::shared_ptr<ASTNode> thenStmt, std::shared_ptr<ASTNode> elseStmt);

    std::shared_ptr<ASTNode>
    SemaIfStmtNode(std::shared_ptr<ASTNode> condExpr, std::shared_ptr<ASTNode> thenStmt, std::shared_ptr<ASTNode> elseStmt);

    std::shared_ptr<ASTNode> SemaForStmtNode(std::shared_ptr<ASTNode> initNode,
                                             std::shared_ptr<ASTNode> condNode,
                                             std::shared_ptr<ASTNode> thenNode,
                                             std::shared_ptr<ASTNode> bodyNode);

    std::shared_ptr<ASTNode> SemaVariableDeclNode(std::shared_ptr<CType> cType, Token &tok);
    std::shared_ptr<VariableDecl::InitValue>
    SemaDeclInitValue(std::shared_ptr<ASTNode> value, std::shared_ptr<CType> declTy, std::vector<int> &offsetList, Token &tok);

    std::shared_ptr<ASTNode> SemaAssignExprNode(std::shared_ptr<ASTNode> left, std::shared_ptr<ASTNode> right, Token tok);

    std::shared_ptr<ASTNode> SemaVariableAccessExprNode(Token &tok);

    std::shared_ptr<ASTNode> SemaSizeofExprNode(std::shared_ptr<ASTNode> expr, std::shared_ptr<CType> sizeofTY);
    std::shared_ptr<ASTNode> SemaUnaryExprNode(UnaryOpCode op, std::shared_ptr<ASTNode> expr, Token tok);

    std::shared_ptr<ASTNode> SemaBinaryExprNode(std::shared_ptr<ASTNode> left, BinOpCode op, std::shared_ptr<ASTNode> right);

    std::shared_ptr<ASTNode>
    SemaThreeExprNode(std::shared_ptr<ASTNode> left, std::shared_ptr<ASTNode> mid, std::shared_ptr<ASTNode> right, Token tok);

    std::shared_ptr<ASTNode> SemaPostIncExprNode(std::shared_ptr<ASTNode> leftNode);
    std::shared_ptr<ASTNode> SemaPostDecExprNode(std::shared_ptr<ASTNode> leftNode);
    std::shared_ptr<ASTNode>
    SemaPostSubscriptExprNode(std::shared_ptr<ASTNode> leftNode, std::shared_ptr<ASTNode> offestNode, Token tok);
    std::shared_ptr<ASTNode> SemaPostMemberDotNode(std::shared_ptr<ASTNode> leftNode, Token tok);
    std::shared_ptr<ASTNode> SemaPostMemberArrowNode(std::shared_ptr<ASTNode> leftNode, Token tok);

    std::shared_ptr<ASTNode> SemaNumberExprNode(std::shared_ptr<CType> cType, Token &tok);

    std::shared_ptr<CType> SemaTagDecl(std::vector<Member> &member, TagKind tagKind, Token &tok);
    std::shared_ptr<CType> SemaAnonyTagDecl(std::vector<Member> &members, TagKind tagKind);
    std::shared_ptr<CType> SemaTagAccess(Token &tok);

    void EnterScope();
    void ExitScope();
    void SetMode(Mode mode);

  private:
    Scope scope;
    Diagnostics &diager;
    Mode mode;
};

#endif // _SEMA_H_