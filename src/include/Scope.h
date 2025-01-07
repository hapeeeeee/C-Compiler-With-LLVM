#pragma once
#ifndef _SCOPE_H_
#define _SCOPE_H_

#include "CType.h"
#include "llvm/ADT/StringMap.h"
#include <memory>

enum class SymbolKind {
    LocalVariable = 0,
};

/// @brief Represents a symbol in C language.
/// @details This class is used to describe a symbol in the program, such as variables, functions,
/// or other named entities. Each symbol is associated with a name, a kind, and a type.
class Symbol {
  public:
    Symbol(llvm::StringRef name, SymbolKind symbolKind, CType *cType)
        : name(name), symbolKind(symbolKind), cType(cType) {
    }
    CType *cType;

  private:
    llvm::StringRef name;
    SymbolKind symbolKind;
};

class Env {
  public:
    llvm::StringMap<std::shared_ptr<Symbol>> variableSymbolTable;
};

/// @brief Represents an environment for managing symbols.
/// @details This class contains a symbol table for storing and accessing variable symbols.
class Scope {
  public:
    Scope();
    void EnterScope();
    void ExitScope();
    void AddSymbol(llvm::StringRef name, SymbolKind symbolKind, CType *cType);
    std::shared_ptr<Symbol> FindVarSymbol(llvm::StringRef name);
    std::shared_ptr<Symbol> FindVarSymbolInCurrEnv(llvm::StringRef name);

  private:
    std::vector<std::shared_ptr<Env>> envs;
};

#endif // _SCOPE_H_