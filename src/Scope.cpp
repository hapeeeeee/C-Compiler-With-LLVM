#include "include/Scope.h"
#include "llvm/Support/raw_ostream.h"

Scope::Scope() {
    envs.push_back(std::make_shared<Env>());
}

void Scope::EnterScope() {
    envs.push_back(std::make_shared<Env>());
}

void Scope::ExitScope() {
    envs.pop_back();
}

void Scope::AddSymbol(llvm::StringRef name, SymbolKind symbolKind, CType *cType) {
    auto symbol = std::make_shared<Symbol>(name, symbolKind, cType);
    envs.back()->variableSymbolTable.insert({name, symbol});
}

std::shared_ptr<Symbol> Scope::FindVarSymbol(llvm::StringRef name) {
    for (auto it = envs.rbegin(); it != envs.rend(); it++) {
        llvm::StringMap<std::shared_ptr<Symbol>> &table = (*it)->variableSymbolTable;
        if (table.count(name) > 0) {
            return table[name];
        }
    }
    return nullptr;
}

std::shared_ptr<Symbol> Scope::FindVarSymbolInCurrEnv(llvm::StringRef name) {
    llvm::StringMap<std::shared_ptr<Symbol>> &table = envs.back()->variableSymbolTable;
    if (table.count(name) >= 1) {
        return table[name];
    }
    return nullptr;
}