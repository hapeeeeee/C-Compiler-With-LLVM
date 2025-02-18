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

void Scope::AddObjSymbol(llvm::StringRef name, std::shared_ptr<CType> cType) {
    auto symbol = std::make_shared<Symbol>(name, SymbolKind::Obj, cType);
    envs.back()->objSymbolTable.insert({name, symbol});
}

std::shared_ptr<Symbol> Scope::FindObjSymbol(llvm::StringRef name) {
    for (auto it = envs.rbegin(); it != envs.rend(); it++) {
        llvm::StringMap<std::shared_ptr<Symbol>> &table = (*it)->objSymbolTable;
        if (table.count(name) > 0) {
            return table[name];
        }
    }
    return nullptr;
}

std::shared_ptr<Symbol> Scope::FindObjSymbolInCurrEnv(llvm::StringRef name) {
    llvm::StringMap<std::shared_ptr<Symbol>> &table = envs.back()->objSymbolTable;
    if (table.count(name) > 0) {
        return table[name];
    }
    return nullptr;
}

void Scope::AddTagSymbol(llvm::StringRef name, std::shared_ptr<CType> cType) {
    auto symbol = std::make_shared<Symbol>(name, SymbolKind::Tag, cType);
    envs.back()->tagSymbolTable.insert({name, symbol});
}

std::shared_ptr<Symbol> Scope::FindTagSymbol(llvm::StringRef name) {
    for (auto it = envs.rbegin(); it != envs.rend(); it++) {
        llvm::StringMap<std::shared_ptr<Symbol>> &table = (*it)->tagSymbolTable;
        if (table.count(name) > 0) {
            return table[name];
        }
    }
    return nullptr;
}

std::shared_ptr<Symbol> Scope::FindTagSymbolInCurrEnv(llvm::StringRef name) {
    llvm::StringMap<std::shared_ptr<Symbol>> &table = envs.back()->tagSymbolTable;
    if (table.count(name) > 0) {
        return table[name];
    }
    return nullptr;
}
