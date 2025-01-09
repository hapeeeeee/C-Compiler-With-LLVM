#include "include/CodeGen.h"
#include "llvm/IR/Verifier.h"

using namespace llvm;

CodeGen::CodeGen(std::shared_ptr<Program> program) {
    llvmModule = std::make_shared<Module>("Literal Expr", llvmContext);
    VisitProgram(program.get());
}

llvm::Value *CodeGen::VisitProgram(Program *program) {
    FunctionType *printfFuncTy = FunctionType::get(
        irBuilder.getInt32Ty(), {llvm::PointerType::get(irBuilder.getInt8Ty(), 0)}, true);
    Function *printfFunc = Function::Create(
        printfFuncTy, GlobalValue::LinkageTypes::ExternalLinkage, "printf", llvmModule.get());

    FunctionType *mainFuncTy = FunctionType::get(irBuilder.getInt32Ty(), false);
    Function *mainFunc       = Function::Create(
        mainFuncTy, GlobalValue::LinkageTypes::ExternalLinkage, "main", llvmModule.get());
    BasicBlock *entryBB = BasicBlock::Create(llvmContext, "entry", mainFunc);
    irBuilder.SetInsertPoint(entryBB);

    llvm::Value *lastVal;
    for (std::shared_ptr<ASTNode> &stmt : program->stmts) {
        lastVal = stmt->AcceptVisitor(this);
    }
    irBuilder.CreateCall(printfFunc, {irBuilder.CreateGlobalString("lastVal: %d\n"), lastVal});
    irBuilder.CreateRet(irBuilder.getInt32(0));

    verifyFunction(*mainFunc);
    llvmModule->print(llvm::outs(), nullptr);
    return nullptr;
}

llvm::Value *CodeGen::VisitBinaryExpr(BinaryExpr *binaryExpr) {
    llvm::Value *left  = binaryExpr->leftExpr->AcceptVisitor(this);
    llvm::Value *right = binaryExpr->rightExpr->AcceptVisitor(this);

    switch (binaryExpr->op) {
    case OpCode::Add:
        return irBuilder.CreateNSWAdd(left, right);
        break;
    case OpCode::Sub:
        return irBuilder.CreateNSWSub(left, right);
        break;
    case OpCode::Mul:
        return irBuilder.CreateNSWMul(left, right);
        break;
    case OpCode::Div:
        return irBuilder.CreateSDiv(left, right);
        break;
    default:
        break;
    }
    return nullptr;
}

llvm::Value *CodeGen::VisitNumberExpr(NumberExpr *numberExpr) {
    return irBuilder.getInt32(numberExpr->token.value);
}

llvm::Value *CodeGen::VisitVariableDecl(VariableDecl *variableDecl) {
    llvm::Type *ty = nullptr;
    if (variableDecl->cType == CType::getIntTy()) {
        ty = irBuilder.getInt32Ty();
    }

    llvm::StringRef name(variableDecl->token.ptr, variableDecl->token.length);
    llvm::Value *value = irBuilder.CreateAlloca(ty, nullptr, name);
    varAddrTypeMap.insert({name, {value, ty}});
    return value;
}

llvm::Value *CodeGen::VisitVariableAssessExpr(VariableAssessExpr *variableAssessExpr) {
    llvm::StringRef name(variableAssessExpr->token.ptr, variableAssessExpr->token.length);
    std::pair<llvm::Value *, llvm::Type *> pair = varAddrTypeMap[name];
    llvm::Value *value                          = pair.first;
    llvm::Type *ty                              = pair.second;
    return irBuilder.CreateLoad(ty, value, name);
}

llvm::Value *CodeGen::VisitAssignExpr(AssignExpr *assignExpr) {
    llvm::StringRef name(assignExpr->token.ptr, assignExpr->token.length);
    auto leftExpr                               = assignExpr->leftExpr;
    VariableAssessExpr *leftAccessExpr          = (VariableAssessExpr *)leftExpr.get();
    std::pair<llvm::Value *, llvm::Type *> pair = varAddrTypeMap[name];

    llvm::Type *leftValueTy    = pair.second;
    llvm::Value *leftValueAddr = pair.first;
    llvm::Value *rightValue    = assignExpr->rightExpr->AcceptVisitor(this);
    irBuilder.CreateStore(rightValue, leftValueAddr);
    return irBuilder.CreateLoad(leftValueTy, leftValueAddr, name);
}