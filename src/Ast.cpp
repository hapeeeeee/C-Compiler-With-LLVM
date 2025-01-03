#include "include/Ast.h"

Program::Program(std::vector<std::shared_ptr<ASTNode>> stmts) : stmts(stmts) {
}