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

    for (std::shared_ptr<Expr> &expr : program->exprs) {
        llvm::Value *exprRet = expr->AcceptVisitor(this);
        irBuilder.CreateCall(printfFunc,
                             {irBuilder.CreateGlobalStringPtr("exprRet: %d\n"), exprRet});
    }

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
        return irBuilder.CreateNSWAdd(left, right);
        break;
    default:
        break;
    }
    return nullptr;
}

llvm::Value *CodeGen::VisitFactorExpr(FactorExpr *factorExpr) {
    return irBuilder.getInt32(factorExpr->number);
}