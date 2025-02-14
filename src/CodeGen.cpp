#include "include/CodeGen.h"
#include "llvm/IR/Verifier.h"

using namespace llvm;

CodeGen::CodeGen(std::shared_ptr<Program> program) {
    llvmModule = std::make_unique<Module>("Literal Expr", llvmContext);
    VisitProgram(program.get());
}

llvm::Value *CodeGen::VisitProgram(Program *program) {
    // `printf` function
    FunctionType *printfFuncTy =
        FunctionType::get(irBuilder.getInt32Ty(), {llvm::PointerType::get(irBuilder.getInt8Ty(), 0)}, true);
    Function *printfFunc = Function::Create(printfFuncTy, GlobalValue::LinkageTypes::ExternalLinkage, "printf", llvmModule.get());

    // `main` function
    FunctionType *mainFuncTy = FunctionType::get(irBuilder.getInt32Ty(), false);
    Function *mainFunc       = Function::Create(mainFuncTy, GlobalValue::LinkageTypes::ExternalLinkage, "main", llvmModule.get());
    BasicBlock *entryBB      = BasicBlock::Create(llvmContext, "entry", mainFunc);
    irBuilder.SetInsertPoint(entryBB);
    currFunc = mainFunc;

    llvm::Value *lastVal;
    lastVal = program->node->AcceptVisitor(this);
    // if (lastVal) {
    //     irBuilder.CreateCall(printfFunc, {irBuilder.CreateGlobalString("lastVal: %d\n"), lastVal});
    // } else {
    //     irBuilder.CreateCall(printfFunc, {irBuilder.CreateGlobalString("last inst is not expr.\n")});
    // }
    // irBuilder.CreateRet(irBuilder.getInt32(0));

    irBuilder.CreateRet(lastVal);
    verifyFunction(*mainFunc);
    // llvmModule->print(llvm::outs(), nullptr);
    if (verifyModule(*llvmModule, &llvm::outs())) {
        llvmModule->print(llvm::outs(), nullptr);
    }

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
    llvm::Value *left  = nullptr;
    llvm::Value *right = nullptr;
    if (binaryExpr->op != BinOpCode::LogicOr && binaryExpr->op != BinOpCode::LogicAnd) {
        left  = binaryExpr->leftExpr->AcceptVisitor(this);
        right = binaryExpr->rightExpr->AcceptVisitor(this);
    }
    switch (binaryExpr->op) {
    case BinOpCode::Add: {
        llvm::Type *ty = binaryExpr->leftExpr->cType->AcceptVisitor(this);
        if (ty->isPointerTy()) {
            return irBuilder.CreateInBoundsGEP(ty, left, {right});
        } else if (ty->isIntegerTy()) {
            return irBuilder.CreateNSWAdd(left, right);
        }
    }
    case BinOpCode::Sub: {
        llvm::Type *ty = binaryExpr->leftExpr->cType->AcceptVisitor(this);
        if (ty->isPointerTy()) {
            llvm::Value *negRight = irBuilder.CreateNeg(right);
            return irBuilder.CreateInBoundsGEP(ty, left, {negRight});
        } else if (ty->isIntegerTy()) {
            return irBuilder.CreateNSWSub(left, right);
        }
    }
    case BinOpCode::Mul: {
        return irBuilder.CreateNSWMul(left, right);
    }
    case BinOpCode::Div: {
        return irBuilder.CreateSDiv(left, right);
    }
    case BinOpCode::Mod: {
        return irBuilder.CreateSRem(left, right);
    }
    case BinOpCode::LeftShift: {
        return irBuilder.CreateShl(left, right);
    }
    case BinOpCode::RightShift: {
        return irBuilder.CreateAShr(left, right);
    }
    case BinOpCode::BitOr: {
        return irBuilder.CreateOr(left, right);
    }
    case BinOpCode::BitXor: {
        return irBuilder.CreateXor(left, right);
    }
    case BinOpCode::BitAnd: {
        return irBuilder.CreateAnd(left, right);
    }
    case BinOpCode::EqualEqual: {
        val = irBuilder.CreateICmpEQ(left, right);
        return irBuilder.CreateIntCast(val, irBuilder.getInt32Ty(), true);
    }
    case BinOpCode::NotEqual: {
        val = irBuilder.CreateICmpNE(left, right);
        return irBuilder.CreateIntCast(val, irBuilder.getInt32Ty(), true);
    }
    case BinOpCode::Less: {
        val = irBuilder.CreateICmpSLT(left, right);
        return irBuilder.CreateIntCast(val, irBuilder.getInt32Ty(), true);
    }
    case BinOpCode::Greater: {
        val = irBuilder.CreateICmpSGT(left, right);
        return irBuilder.CreateIntCast(val, irBuilder.getInt32Ty(), true);
    }
    case BinOpCode::LessEqual: {
        val = irBuilder.CreateICmpSLE(left, right);
        return irBuilder.CreateIntCast(val, irBuilder.getInt32Ty(), true);
    }
    case BinOpCode::GreaterEqual: {
        val = irBuilder.CreateICmpSGE(left, right);
        return irBuilder.CreateIntCast(val, irBuilder.getInt32Ty(), true);
    }
    case BinOpCode::LogicOr: {
        //  currBB || nextBB { mergeBB }
        //      currBB(calc leftExpr)
        //     1/     \0
        //   trueBB   nextBB(calc rightExpr)
        //      \     /
        //      mergeBB
        auto trueBB  = llvm::BasicBlock::Create(llvmContext, "trueBB");
        auto nextBB  = llvm::BasicBlock::Create(llvmContext, "nextBB");
        auto mergeBB = llvm::BasicBlock::Create(llvmContext, "mergeBB");

        llvm::Value *left = binaryExpr->leftExpr->AcceptVisitor(this);
        val               = irBuilder.CreateICmpNE(left, irBuilder.getInt32(0));
        irBuilder.CreateCondBr(val, trueBB, nextBB);

        trueBB->insertInto(currFunc);
        irBuilder.SetInsertPoint(trueBB);
        irBuilder.CreateBr(mergeBB);

        nextBB->insertInto(currFunc);
        irBuilder.SetInsertPoint(nextBB);
        // Note: The right-hand side block code generation here might create new basic blocks. After
        // the generation is complete, the insertPoint might no longer be at nextBB, so the
        // subsequent phi->addIncoming(right, nextBB); might not be able to reach nextBB, leading to
        // a bug.
        llvm::Value *right = binaryExpr->rightExpr->AcceptVisitor(this);
        nextBB             = irBuilder.GetInsertBlock();
        right              = irBuilder.CreateICmpNE(right, irBuilder.getInt32(0));
        right              = irBuilder.CreateZExt(right, irBuilder.getInt32Ty());
        irBuilder.CreateBr(mergeBB);

        mergeBB->insertInto(currFunc);
        irBuilder.SetInsertPoint(mergeBB);
        llvm::PHINode *phi = irBuilder.CreatePHI(irBuilder.getInt32Ty(), 2);
        phi->addIncoming(irBuilder.getInt32(1), trueBB);
        phi->addIncoming(right, nextBB);
        return phi;
        break;
    }
    case BinOpCode::LogicAnd: { // currBB && nextBB { mergeBB }
        //      currBB(calc leftExpr)
        //     0/     \1
        //   falseBB   nextBB(calc rightExpr)
        //      \     /
        //      mergeBB
        auto falseBB = llvm::BasicBlock::Create(llvmContext, "falseBB");
        auto nextBB  = llvm::BasicBlock::Create(llvmContext, "nextBB", currFunc);
        auto mergeBB = llvm::BasicBlock::Create(llvmContext, "mergeBB");

        llvm::Value *left = binaryExpr->leftExpr->AcceptVisitor(this);
        val               = irBuilder.CreateICmpNE(left, irBuilder.getInt32(0));
        irBuilder.CreateCondBr(val, nextBB, falseBB);

        irBuilder.SetInsertPoint(nextBB);
        // Note: The right-hand side block code generation here might create new basic blocks. After
        // the generation is complete, the insertPoint might no longer be at nextBB, so the
        // subsequent phi->addIncoming(right, nextBB); might not be able to reach nextBB, leading to
        // a bug.
        llvm::Value *right = binaryExpr->rightExpr->AcceptVisitor(this);
        nextBB             = irBuilder.GetInsertBlock();
        val                = irBuilder.CreateICmpNE(left, irBuilder.getInt32(0));
        irBuilder.CreateBr(mergeBB);

        falseBB->insertInto(currFunc);
        irBuilder.SetInsertPoint(falseBB);
        irBuilder.CreateBr(mergeBB);

        mergeBB->insertInto(currFunc);
        irBuilder.SetInsertPoint(mergeBB);
        llvm::PHINode *phi = irBuilder.CreatePHI(irBuilder.getInt32Ty(), 2);
        phi->addIncoming(irBuilder.getInt32(0), falseBB);
        phi->addIncoming(right, nextBB);

        return phi;
        break;
    }
    // case BinOpCode::Comma: {
    //     break;
    // }
    case BinOpCode::Assign: {
        LoadInst *loadInst = llvm::dyn_cast<LoadInst>(left);
        assert(loadInst);
        irBuilder.CreateStore(right, loadInst->getPointerOperand());
        return right;
    }
    case BinOpCode::AddAssign: {
        LoadInst *loadInst = llvm::dyn_cast<LoadInst>(left);
        assert(loadInst);

        llvm::Type *ty = binaryExpr->leftExpr->cType->AcceptVisitor(this);
        if (ty->isPointerTy()) {
            llvm::Value *newVal = irBuilder.CreateInBoundsGEP(ty, left, {right});
            irBuilder.CreateStore(newVal, loadInst->getPointerOperand());
            return newVal;
        } else if (ty->isIntegerTy()) {
            llvm::Value *newVal = irBuilder.CreateAdd(left, right);
            irBuilder.CreateStore(newVal, loadInst->getPointerOperand());
            return newVal;
        }
    }
    case BinOpCode::SubAssign: {
        LoadInst *loadInst = llvm::dyn_cast<LoadInst>(left);
        assert(loadInst);

        llvm::Type *ty = binaryExpr->cType->AcceptVisitor(this);
        if (ty->isPointerTy()) {
            llvm::Value *newVal = irBuilder.CreateInBoundsGEP(ty, left, {irBuilder.CreateNeg(right)});
            irBuilder.CreateStore(newVal, loadInst->getPointerOperand());
        } else if (ty->isIntegerTy()) {
            llvm::Value *newVal = irBuilder.CreateSub(left, right);
            irBuilder.CreateStore(newVal, loadInst->getPointerOperand());
            return newVal;
        }
    }
    case BinOpCode::MulAssign: {
        LoadInst *loadInst = llvm::dyn_cast<LoadInst>(left);
        assert(loadInst);
        llvm::Value *val = irBuilder.CreateMul(left, right);
        irBuilder.CreateStore(val, loadInst->getPointerOperand());
        return val;
    }
    case BinOpCode::DivAssign: {
        LoadInst *loadInst = llvm::dyn_cast<LoadInst>(left);
        assert(loadInst);
        llvm::Value *val = irBuilder.CreateSDiv(left, right);
        irBuilder.CreateStore(val, loadInst->getPointerOperand());
        return val;
    }
    case BinOpCode::ModAssign: {
        LoadInst *loadInst = llvm::dyn_cast<LoadInst>(left);
        assert(loadInst);
        llvm::Value *val = irBuilder.CreateSRem(left, right);
        irBuilder.CreateStore(val, loadInst->getPointerOperand());
        return val;
    }
    case BinOpCode::OrAssign: {
        LoadInst *loadInst = llvm::dyn_cast<LoadInst>(left);
        assert(loadInst);
        llvm::Value *val = irBuilder.CreateOr(left, right);
        irBuilder.CreateStore(val, loadInst->getPointerOperand());
        return val;
    }
    case BinOpCode::AndAssign: {
        LoadInst *loadInst = llvm::dyn_cast<LoadInst>(left);
        assert(loadInst);
        llvm::Value *val = irBuilder.CreateAnd(left, right);
        irBuilder.CreateStore(val, loadInst->getPointerOperand());
        return val;
    }
    case BinOpCode::XorAssign: {
        LoadInst *loadInst = llvm::dyn_cast<LoadInst>(left);
        assert(loadInst);
        llvm::Value *val = irBuilder.CreateXor(left, right);
        irBuilder.CreateStore(val, loadInst->getPointerOperand());
        return val;
    }
    case BinOpCode::LeftShiftAssign: {
        LoadInst *loadInst = llvm::dyn_cast<LoadInst>(left);
        assert(loadInst);
        llvm::Value *val = irBuilder.CreateShl(left, right);
        irBuilder.CreateStore(val, loadInst->getPointerOperand());
        return val;
    }
    case BinOpCode::RightShiftAssign: {
        LoadInst *loadInst = llvm::dyn_cast<LoadInst>(left);
        assert(loadInst);
        llvm::Value *val = irBuilder.CreateAShr(left, right);
        irBuilder.CreateStore(val, loadInst->getPointerOperand());
        return val;
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
    llvm::Type *ty = variableDecl->cType->AcceptVisitor(this);
    llvm::StringRef name(variableDecl->token.ptr, variableDecl->token.length);
    llvm::Value *value = irBuilder.CreateAlloca(ty, nullptr, name);
    varAddrTypeMap.insert({name, {value, ty}});

    if (variableDecl->initNode) {
        llvm::Value *initVal = variableDecl->initNode->AcceptVisitor(this);
        irBuilder.CreateStore(initVal, value);
    }
    return irBuilder.CreateLoad(ty, value);
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

    llvm::BasicBlock *deathBB = llvm::BasicBlock::Create(llvmContext, "for.continue.death", currFunc);
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

llvm::Value *CodeGen::VisitSizeofExpr(SizeofExpr *expr) {
    llvm::Type *ty = nullptr;
    if (expr->expr) {
        ty = expr->expr->cType->AcceptVisitor(this);
    } else {
        ty = expr->sizeofTY->AcceptVisitor(this);
    }
    if (ty->isPointerTy()) {
        return irBuilder.getInt32(8);
    } else if (ty->isIntegerTy()) {
        return irBuilder.getInt32(4);
    } else {
        assert(0);
        return nullptr;
    }
}

llvm::Value *CodeGen::VisitUnaryExpr(UnaryExpr *unaryExpr) {
    llvm::Value *val = unaryExpr->expr->AcceptVisitor(this);
    llvm::Type *ty   = unaryExpr->expr->cType->AcceptVisitor(this);

    switch (unaryExpr->op) {
    case UnaryOpCode::Positive: {
        return val;
    }
    case UnaryOpCode::Negative: {
        return irBuilder.CreateNeg(val);
    }
    case UnaryOpCode::Deref: {
        // *p
        llvm::Type *nodeTy = unaryExpr->cType->AcceptVisitor(this);
        return irBuilder.CreateLoad(nodeTy, val);
    }
    case UnaryOpCode::Addr: {
        // &p
        return llvm::dyn_cast<LoadInst>(val)->getPointerOperand();
    }
    case UnaryOpCode::Inc: {
        // ++p
        if (ty->isPointerTy()) {
            llvm::Value *newVal = irBuilder.CreateInBoundsGEP(ty, val, {irBuilder.getInt32(1)});
            irBuilder.CreateStore(newVal, llvm::dyn_cast<LoadInst>(val)->getPointerOperand());
            return newVal;
        } else if (ty->isIntegerTy()) {
            llvm::Value *newVal = irBuilder.CreateAdd(val, irBuilder.getInt32(1));
            irBuilder.CreateStore(newVal, llvm::dyn_cast<LoadInst>(val)->getPointerOperand());
            return newVal;
        } else {
            assert(0);
            return nullptr;
        }
    }
    case UnaryOpCode::Dec: {
        // --p
        if (ty->isPointerTy()) {
            llvm::Value *newVal = irBuilder.CreateInBoundsGEP(ty, val, {irBuilder.getInt32(-1)});
            irBuilder.CreateStore(newVal, llvm::dyn_cast<LoadInst>(val)->getPointerOperand());
            return newVal;
        } else if (ty->isIntegerTy()) {
            llvm::Value *newVal = irBuilder.CreateSub(val, irBuilder.getInt32(1));
            irBuilder.CreateStore(newVal, llvm::dyn_cast<LoadInst>(val)->getPointerOperand());
            return newVal;
        } else {
            assert(0);
            return nullptr;
        }
        break;
    }
    case UnaryOpCode::LogicNot: {
        llvm::Value *condRet = irBuilder.CreateICmpNE(val, irBuilder.getInt32(0));
        return irBuilder.CreateZExt(irBuilder.CreateNot(condRet), irBuilder.getInt32Ty());
    }
    case UnaryOpCode::BitNot: {
        return irBuilder.CreateNot(val);
    }
    }

    return nullptr;
}

llvm::Value *CodeGen::VisitThreeExpr(ThreeExpr *threeExpr) {
    llvm::Value *condVal = threeExpr->condExpr->AcceptVisitor(this);
    llvm::Value *condRet = irBuilder.CreateICmpNE(condVal, irBuilder.getInt32(0));

    llvm::BasicBlock *trueBB  = llvm::BasicBlock::Create(llvmContext, "then", currFunc);
    llvm::BasicBlock *falseBB = llvm::BasicBlock::Create(llvmContext, "els");
    llvm::BasicBlock *lastBB  = llvm::BasicBlock::Create(llvmContext, "merge");
    irBuilder.CreateCondBr(condRet, trueBB, falseBB);

    irBuilder.SetInsertPoint(trueBB);
    llvm::Value *trueVal = threeExpr->trueExpr->AcceptVisitor(this);
    trueBB               = irBuilder.GetInsertBlock();
    irBuilder.CreateBr(lastBB);

    falseBB->insertInto(currFunc);
    irBuilder.SetInsertPoint(falseBB);
    llvm::Value *falseVal = threeExpr->falseExpr->AcceptVisitor(this);
    falseBB               = irBuilder.GetInsertBlock();
    irBuilder.CreateBr(lastBB);

    lastBB->insertInto(currFunc);
    irBuilder.SetInsertPoint(lastBB);

    llvm::PHINode *phi = irBuilder.CreatePHI(threeExpr->trueExpr->cType->AcceptVisitor(this), 2);
    phi->addIncoming(trueVal, trueBB);
    phi->addIncoming(falseVal, falseBB);
    return phi;
}

llvm::Value *CodeGen::VisitPostIncExpr(PostIncExpr *postIncExpr) {
    llvm::Type *ty      = postIncExpr->leftNode->cType->AcceptVisitor(this);
    llvm::Value *oldVal = postIncExpr->leftNode->AcceptVisitor(this);
    if (ty->isPointerTy()) {
        llvm::Value *newVal = irBuilder.CreateInBoundsGEP(ty, oldVal, {irBuilder.getInt32(1)});
        irBuilder.CreateStore(newVal, llvm::dyn_cast<LoadInst>(oldVal)->getPointerOperand());
        return oldVal;
    } else if (ty->isIntegerTy()) {
        llvm::Value *newVal = irBuilder.CreateAdd(oldVal, irBuilder.getInt32(1));
        irBuilder.CreateStore(newVal, llvm::dyn_cast<LoadInst>(oldVal)->getPointerOperand());
        return oldVal;
    } else {
        assert(0);
        return nullptr;
    }
}

llvm::Value *CodeGen::VisitPostDecExpr(PostDecExpr *postDecExpr) {
    llvm::Type *ty      = postDecExpr->leftNode->cType->AcceptVisitor(this);
    llvm::Value *oldVal = postDecExpr->leftNode->AcceptVisitor(this);
    if (ty->isPointerTy()) {
        llvm::Value *newVal = irBuilder.CreateInBoundsGEP(ty, oldVal, {irBuilder.getInt32(-1)});
        irBuilder.CreateStore(newVal, llvm::dyn_cast<LoadInst>(oldVal)->getPointerOperand());
        return oldVal;
    } else if (ty->isIntegerTy()) {
        llvm::Value *newVal = irBuilder.CreateSub(oldVal, irBuilder.getInt32(1));
        irBuilder.CreateStore(newVal, llvm::dyn_cast<LoadInst>(oldVal)->getPointerOperand());
        return oldVal;
    } else {
        assert(0);
        return nullptr;
    }
}

llvm::Value *CodeGen::VisitPostSubscriptExpr(PostSubscriptExpr *postSubscriptExpr) {
    llvm::Type *elemTy  = postSubscriptExpr->cType->AcceptVisitor(this);
    llvm::Value *val    = postSubscriptExpr->leftNode->AcceptVisitor(this);
    llvm::Value *offest = postSubscriptExpr->node->AcceptVisitor(this);

    llvm::Value *addr = irBuilder.CreateInBoundsGEP(elemTy, llvm::dyn_cast<LoadInst>(val)->getPointerOperand(), {offest});
    return irBuilder.CreateLoad(elemTy, addr);
}

llvm::Type *CodeGen::VisitCPrimaryType(CPrimaryType *ty) {
    if (ty->GetTypeKind() == CType::CTypeKind::TY_Int) {
        return irBuilder.getInt32Ty();
    }
    assert(0);
    return nullptr;
}

llvm::Type *CodeGen::VisitCPointType(CPointType *ty) {
    llvm::Type *baseTy = ty->GetBaseType()->AcceptVisitor(this);
    return llvm::PointerType::getUnqual(baseTy);
}

llvm::Type *CodeGen::VisitCArrayType(CArrayType *ty) {
    llvm::Type *elementTy = ty->GetElementType()->AcceptVisitor(this);
    return llvm::ArrayType::get(elementTy, ty->GetElementCount());
}
