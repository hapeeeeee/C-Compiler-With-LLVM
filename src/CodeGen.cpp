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
    currFunc = mainFunc;

    llvm::Value *lastVal;
    for (std::shared_ptr<ASTNode> &stmt : program->stmts) {
        lastVal = stmt->AcceptVisitor(this);
    }
    if (lastVal) {
        irBuilder.CreateCall(printfFunc, {irBuilder.CreateGlobalString("lastVal: %d\n"), lastVal});
    } else {
        irBuilder.CreateCall(printfFunc,
                             {irBuilder.CreateGlobalString("last inst is not expr.\n")});
    }

    irBuilder.CreateRet(irBuilder.getInt32(0));

    verifyFunction(*mainFunc);
    llvmModule->print(llvm::outs(), nullptr);
    return nullptr;
}

llvm::Value *CodeGen::VisitDeclStmts(DeclStmts *declStmts) {
    llvm::Value *lastVal = nullptr;
    for (auto node : declStmts->nodeVec) {
        lastVal = node->AcceptVisitor(this);
    }
    return lastVal;
}

llvm::Value *CodeGen::VisitBlockStmts(BlockStmts *blockStmts) {
    llvm::Value *lastVal = nullptr;
    for (auto node : blockStmts->nodeVec) {
        lastVal = node->AcceptVisitor(this);
    }
    return lastVal;
}

llvm::Value *CodeGen::VisitBinaryExpr(BinaryExpr *binaryExpr) {

    llvm::Value *val;
    switch (binaryExpr->op) {
    case OpCode::Add: {
        llvm::Value *left  = binaryExpr->leftExpr->AcceptVisitor(this);
        llvm::Value *right = binaryExpr->rightExpr->AcceptVisitor(this);
        return irBuilder.CreateNSWAdd(left, right);
    }
    case OpCode::Sub: {
        llvm::Value *left  = binaryExpr->leftExpr->AcceptVisitor(this);
        llvm::Value *right = binaryExpr->rightExpr->AcceptVisitor(this);
        return irBuilder.CreateNSWSub(left, right);
    }
    case OpCode::Mul: {
        llvm::Value *left  = binaryExpr->leftExpr->AcceptVisitor(this);
        llvm::Value *right = binaryExpr->rightExpr->AcceptVisitor(this);
        return irBuilder.CreateNSWMul(left, right);
    }
    case OpCode::Div: {
        llvm::Value *left  = binaryExpr->leftExpr->AcceptVisitor(this);
        llvm::Value *right = binaryExpr->rightExpr->AcceptVisitor(this);
        return irBuilder.CreateSDiv(left, right);
    }
    case OpCode::Mod: {
        llvm::Value *left  = binaryExpr->leftExpr->AcceptVisitor(this);
        llvm::Value *right = binaryExpr->rightExpr->AcceptVisitor(this);
        return irBuilder.CreateSRem(left, right);
    }
    case OpCode::LeftShift: {
        llvm::Value *left  = binaryExpr->leftExpr->AcceptVisitor(this);
        llvm::Value *right = binaryExpr->rightExpr->AcceptVisitor(this);
        return irBuilder.CreateShl(left, right);
    }
    case OpCode::RightShift: {
        llvm::Value *left  = binaryExpr->leftExpr->AcceptVisitor(this);
        llvm::Value *right = binaryExpr->rightExpr->AcceptVisitor(this);
        return irBuilder.CreateLShr(left, right);
        return nullptr;
    }
    case OpCode::BitOr: {
        llvm::Value *left  = binaryExpr->leftExpr->AcceptVisitor(this);
        llvm::Value *right = binaryExpr->rightExpr->AcceptVisitor(this);
        return irBuilder.CreateOr(left, right);
    }
    case OpCode::BitXor: {
        llvm::Value *left  = binaryExpr->leftExpr->AcceptVisitor(this);
        llvm::Value *right = binaryExpr->rightExpr->AcceptVisitor(this);
        return irBuilder.CreateXor(left, right);
    }
    case OpCode::BitAnd: {
        llvm::Value *left  = binaryExpr->leftExpr->AcceptVisitor(this);
        llvm::Value *right = binaryExpr->rightExpr->AcceptVisitor(this);
        return irBuilder.CreateAnd(left, right);
    }
    case OpCode::EqualEqual: {
        llvm::Value *left  = binaryExpr->leftExpr->AcceptVisitor(this);
        llvm::Value *right = binaryExpr->rightExpr->AcceptVisitor(this);
        val                = irBuilder.CreateICmpEQ(left, right);
        return irBuilder.CreateIntCast(val, irBuilder.getInt32Ty(), true);
    }
    case OpCode::NotEqual: {
        llvm::Value *left  = binaryExpr->leftExpr->AcceptVisitor(this);
        llvm::Value *right = binaryExpr->rightExpr->AcceptVisitor(this);
        val                = irBuilder.CreateICmpNE(left, right);
        return irBuilder.CreateIntCast(val, irBuilder.getInt32Ty(), true);
    }
    case OpCode::Less: {
        llvm::Value *left  = binaryExpr->leftExpr->AcceptVisitor(this);
        llvm::Value *right = binaryExpr->rightExpr->AcceptVisitor(this);
        val                = irBuilder.CreateICmpSLT(left, right);
        return irBuilder.CreateIntCast(val, irBuilder.getInt32Ty(), true);
    }
    case OpCode::Greater: {
        llvm::Value *left  = binaryExpr->leftExpr->AcceptVisitor(this);
        llvm::Value *right = binaryExpr->rightExpr->AcceptVisitor(this);
        val                = irBuilder.CreateICmpSGT(left, right);
        return irBuilder.CreateIntCast(val, irBuilder.getInt32Ty(), true);
    }
    case OpCode::LessEqual: {
        llvm::Value *left  = binaryExpr->leftExpr->AcceptVisitor(this);
        llvm::Value *right = binaryExpr->rightExpr->AcceptVisitor(this);
        val                = irBuilder.CreateICmpSLE(left, right);
        return irBuilder.CreateIntCast(val, irBuilder.getInt32Ty(), true);
    }
    case OpCode::GreaterEqual: {
        llvm::Value *left  = binaryExpr->leftExpr->AcceptVisitor(this);
        llvm::Value *right = binaryExpr->rightExpr->AcceptVisitor(this);
        val                = irBuilder.CreateICmpSGE(left, right);
        return irBuilder.CreateIntCast(val, irBuilder.getInt32Ty(), true);
    }
    case OpCode::LogicOr: {
        //  currBB || nextBB { mergeBB }
        //      currBB(calc leftExpr)
        //     1/     \0
        //   trueBB   nextBB(calc rightExpr)
        //      \     /
        //      mergeBB
        auto trueBB  = llvm::BasicBlock::Create(llvmContext, "trueBB", currFunc);
        auto nextBB  = llvm::BasicBlock::Create(llvmContext, "nextBB", currFunc);
        auto mergeBB = llvm::BasicBlock::Create(llvmContext, "mergeBB", currFunc);

        llvm::Value *left = binaryExpr->leftExpr->AcceptVisitor(this);
        val               = irBuilder.CreateICmpNE(left, irBuilder.getInt32(0));
        irBuilder.CreateCondBr(val, trueBB, nextBB);

        irBuilder.SetInsertPoint(trueBB);
        irBuilder.CreateBr(mergeBB);

        irBuilder.SetInsertPoint(nextBB);
        llvm::Value *right = binaryExpr->rightExpr->AcceptVisitor(this);
        right              = irBuilder.CreateICmpNE(right, irBuilder.getInt32(0));
        right              = irBuilder.CreateZExt(right, irBuilder.getInt32Ty());
        irBuilder.CreateBr(mergeBB);

        irBuilder.SetInsertPoint(mergeBB);
        llvm::PHINode *phi = irBuilder.CreatePHI(irBuilder.getInt32Ty(), 2);
        phi->addIncoming(irBuilder.getInt32(1), trueBB);
        phi->addIncoming(right, nextBB);
        return phi;
        break;
    }

    case OpCode::LogicAnd: { // currBB && nextBB { mergeBB }
        //      currBB(calc leftExpr)
        //     0/     \1
        //   falseBB   nextBB(calc rightExpr)
        //      \     /
        //      mergeBB
        auto falseBB = llvm::BasicBlock::Create(llvmContext, "falseBB", currFunc);
        auto nextBB  = llvm::BasicBlock::Create(llvmContext, "nextBB", currFunc);
        auto mergeBB = llvm::BasicBlock::Create(llvmContext, "mergeBB", currFunc);

        llvm::Value *left = binaryExpr->leftExpr->AcceptVisitor(this);
        val               = irBuilder.CreateICmpNE(left, irBuilder.getInt32(0));
        irBuilder.CreateCondBr(val, nextBB, falseBB);

        irBuilder.SetInsertPoint(nextBB);
        llvm::Value *right = binaryExpr->rightExpr->AcceptVisitor(this);
        val                = irBuilder.CreateICmpNE(left, irBuilder.getInt32(0));
        irBuilder.CreateBr(mergeBB);

        irBuilder.SetInsertPoint(falseBB);
        irBuilder.CreateBr(mergeBB);

        irBuilder.SetInsertPoint(mergeBB);
        llvm::PHINode *phi = irBuilder.CreatePHI(irBuilder.getInt32Ty(), 2);
        phi->addIncoming(irBuilder.getInt32(0), falseBB);
        phi->addIncoming(right, nextBB);

        return phi;
        break;
    }

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

llvm::Value *CodeGen::VisitIfStmt(IfStmt *ifStmt) {
    llvm::BasicBlock *condBB = llvm::BasicBlock::Create(llvmContext, "cond", currFunc);
    llvm::BasicBlock *thenBB = llvm::BasicBlock::Create(llvmContext, "then", currFunc);
    llvm::BasicBlock *elseBB = nullptr;
    if (ifStmt->elseStmt) {
        elseBB = llvm::BasicBlock::Create(llvmContext, "else", currFunc);
    }
    llvm::BasicBlock *lastBB = llvm::BasicBlock::Create(llvmContext, "last", currFunc);
    irBuilder.CreateBr(condBB);
    irBuilder.SetInsertPoint(condBB);
    llvm::Value *val     = ifStmt->condExpr->AcceptVisitor(this);
    llvm::Value *condVal = irBuilder.CreateICmpNE(val, irBuilder.getInt32(0));
    if (ifStmt->elseStmt) {
        irBuilder.CreateCondBr(condVal, thenBB, elseBB);
        irBuilder.SetInsertPoint(thenBB);
        ifStmt->thenStmt->AcceptVisitor(this);
        irBuilder.CreateBr(lastBB);

        irBuilder.SetInsertPoint(elseBB);
        ifStmt->elseStmt->AcceptVisitor(this);
        irBuilder.CreateBr(lastBB);
    } else {
        irBuilder.CreateCondBr(condVal, thenBB, lastBB);
        irBuilder.SetInsertPoint(thenBB);
        ifStmt->thenStmt->AcceptVisitor(this); // if {then(break) }
        irBuilder.CreateBr(lastBB);            // death
    }
    irBuilder.SetInsertPoint(lastBB);
    return nullptr;
}

llvm::Value *CodeGen::VisitForStmt(ForStmt *forStmt) {
    llvm::BasicBlock *initBB = llvm::BasicBlock::Create(llvmContext, "for.init", currFunc);
    llvm::BasicBlock *condBB = llvm::BasicBlock::Create(llvmContext, "for.cond", currFunc);
    llvm::BasicBlock *thenBB = llvm::BasicBlock::Create(llvmContext, "for.then", currFunc);
    llvm::BasicBlock *bodyBB = llvm::BasicBlock::Create(llvmContext, "for.body", currFunc);
    llvm::BasicBlock *lastBB = llvm::BasicBlock::Create(llvmContext, "for.last", currFunc);

    breakTargetBBs.insert({forStmt, lastBB});
    continueTargetBBs.insert({forStmt, thenBB});

    irBuilder.CreateBr(initBB);
    irBuilder.SetInsertPoint(initBB);
    if (forStmt->initNode) {
        forStmt->initNode->AcceptVisitor(this);
    }

    irBuilder.CreateBr(condBB);
    irBuilder.SetInsertPoint(condBB);
    if (forStmt->condNode) {
        llvm::Value *val     = forStmt->condNode->AcceptVisitor(this);
        llvm::Value *condVal = irBuilder.CreateICmpNE(val, irBuilder.getInt32(0));
        irBuilder.CreateCondBr(condVal, bodyBB, lastBB);
    } else {
        irBuilder.CreateBr(bodyBB);
    }

    irBuilder.SetInsertPoint(bodyBB);
    if (forStmt->bodyNode) {
        forStmt->bodyNode->AcceptVisitor(this);
    }
    irBuilder.CreateBr(thenBB);

    irBuilder.SetInsertPoint(thenBB);
    if (forStmt->thenNode) {
        forStmt->thenNode->AcceptVisitor(this);
    }
    irBuilder.CreateBr(condBB);

    irBuilder.SetInsertPoint(lastBB);
    return nullptr;
}

llvm::Value *CodeGen::VisitBreakStmt(BreakStmt *breakStmt) {
    auto a                     = breakStmt->fatherNode.get();
    llvm::BasicBlock *targetBB = breakTargetBBs[a];
    irBuilder.CreateBr(targetBB);

    llvm::BasicBlock *deathBB = llvm::BasicBlock::Create(llvmContext, "for.break.death", currFunc);
    irBuilder.SetInsertPoint(deathBB);
    return nullptr;
}

llvm::Value *CodeGen::VisitContinueStmt(ContinueStmt *continueStmt) {
    llvm::BasicBlock *targetBB = continueTargetBBs[continueStmt->fatherNode.get()];
    irBuilder.CreateBr(targetBB);

    llvm::BasicBlock *deathBB =
        llvm::BasicBlock::Create(llvmContext, "for.continue.death", currFunc);
    irBuilder.SetInsertPoint(deathBB);
    return nullptr;
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