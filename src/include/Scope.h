#pragma once
#ifndef _SCOPE_H_
#define _SCOPE_H_

#include "CType.h"
#include "llvm/ADT/StringMap.h"
#include <memory>

enum class SymbolKind {
    LocalVariable = 0,
};

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