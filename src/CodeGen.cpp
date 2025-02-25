#include "include/CodeGen.h"
#include "llvm/IR/Verifier.h"

using namespace llvm;

CodeGen::CodeGen(std::shared_ptr<Program> program) {
    llvmModule = std::make_unique<Module>("Literal Expr", llvmContext);
    VisitProgram(program.get());
}

llvm::Value *CodeGen::VisitProgram(Program *program) {
    for (auto &decl : program->externDecls) {
        decl->AcceptVisitor(this);
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

    // All variable declarations in the function are placed at the beginning of the function.
    llvm::IRBuilder<> tmp(&currFunc->getEntryBlock(), currFunc->getEntryBlock().begin());
    llvm::Value *value = tmp.CreateAlloca(ty, nullptr, name);
    localVarAddrTypeMap.insert({name, {value, ty}});

    if (variableDecl->initValues.size() > 0) {
        if (variableDecl->initValues.size() == 1) {
            llvm::Value *initVal = variableDecl->initValues[0]->value->AcceptVisitor(this);
            irBuilder.CreateStore(initVal, value);
        } else {
            if (llvm::ArrayType *arrTy = llvm::dyn_cast<llvm::ArrayType>(ty)) {
                for (const auto &node : variableDecl->initValues) {
                    llvm::SmallVector<llvm::Value *> IdxVec;
                    for (const auto &nodeSubIdx : node->offsetList) {
                        IdxVec.push_back(irBuilder.getInt32(nodeSubIdx));
                    }
                    auto nodePtr   = irBuilder.CreateInBoundsGEP(ty, value, IdxVec);
                    auto nodeValue = node->value->AcceptVisitor(this);
                    irBuilder.CreateStore(nodeValue, nodePtr);
                }
            } else if (llvm::StructType *arrTy = llvm::dyn_cast<llvm::StructType>(ty)) {
                CRecordType *recordTy = llvm::dyn_cast<CRecordType>(variableDecl->cType.get());
                if (recordTy->GetTagKind() == TagKind::kSturct) {
                    for (const auto &node : variableDecl->initValues) {
                        llvm::SmallVector<llvm::Value *> IdxVec;
                        for (const auto &nodeSubIdx : node->offsetList) {
                            IdxVec.push_back(irBuilder.getInt32(nodeSubIdx));
                        }
                        auto nodePtr   = irBuilder.CreateInBoundsGEP(ty, value, IdxVec);
                        auto nodeValue = node->value->AcceptVisitor(this);
                        irBuilder.CreateStore(nodeValue, nodePtr);
                    }
                } else {
                    assert(variableDecl->initValues.size() == 1);
                    auto node = variableDecl->initValues[0];
                    llvm::SmallVector<llvm::Value *> IdxVec;
                    for (const auto &nodeSubIdx : node->offsetList) {
                        IdxVec.push_back(irBuilder.getInt32(nodeSubIdx));
                    }
                    auto nodePtr   = irBuilder.CreateInBoundsGEP(ty, value, IdxVec);
                    auto nodeValue = node->value->AcceptVisitor(this);
                    irBuilder.CreateStore(nodeValue, nodePtr);
                }
            } else {
                assert(0);
            }
        }
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
    std::pair<llvm::Value *, llvm::Type *> pair = localVarAddrTypeMap[name];
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

llvm::Value *CodeGen::VisitPostMemberDotExpr(PostMemberDotExpr *postMemberDotExpr) {
    // a.b;
    llvm::Type *leftTy        = postMemberDotExpr->leftNode->cType->AcceptVisitor(this);
    llvm::Value *baseValue    = postMemberDotExpr->leftNode->AcceptVisitor(this);
    LoadInst *loadInst        = llvm::dyn_cast<LoadInst>(baseValue);
    llvm::Value *baseValuePtr = loadInst->getPointerOperand();
    CRecordType *recordTy     = llvm::dyn_cast<CRecordType>(postMemberDotExpr->leftNode->cType.get());

    llvm::Type *memberType = postMemberDotExpr->member.cType->AcceptVisitor(this);
    if (recordTy->GetTagKind() == TagKind::kSturct) {
        // baseValuePtr is a pointer to a structure,
        // and it can also be viewed as the beginning of an array of structures (like `Struct[1]`).
        // Therefore, you can access the address of a member by using Struct[0].field.
        llvm::Value *memberAddr = irBuilder.CreateInBoundsGEP(
            leftTy, baseValuePtr, {irBuilder.getInt32(0), irBuilder.getInt32(postMemberDotExpr->member.memberIdx)});

        return irBuilder.CreateLoad(memberType, memberAddr);
    } else {
        llvm::Value *memberAddr =
            irBuilder.CreateInBoundsGEP(leftTy, baseValuePtr, {irBuilder.getInt32(0), irBuilder.getInt32(0)});
        llvm::Value *cast = irBuilder.CreateBitCast(memberAddr, llvm::PointerType::getUnqual(memberType));
        return irBuilder.CreateLoad(memberType, cast);
    }
    return nullptr;
}

llvm::Value *CodeGen::VisitPostMemberArrowExpr(PostMemberArrowExpr *postMemberArrowExpr) {
    // a->b;
    CPointType *lefPointerTy = llvm::dyn_cast<CPointType>(postMemberArrowExpr->leftNode->cType.get());
    llvm::Type *leftTy       = lefPointerTy->GetBaseType()->AcceptVisitor(this);

    llvm::Value *baseValuePtr = postMemberArrowExpr->leftNode->AcceptVisitor(this);
    CRecordType *recordTy     = llvm::dyn_cast<CRecordType>(lefPointerTy->GetBaseType().get());

    llvm::Type *memberType = postMemberArrowExpr->member.cType->AcceptVisitor(this);
    if (recordTy->GetTagKind() == TagKind::kSturct) {
        // baseValuePtr is a pointer to a structure,
        // and it can also be viewed as the beginning of an array of structures (like `Struct[1]`).
        // Therefore, you can access the address of a member by using Struct[0].field.
        llvm::Value *memberAddr = irBuilder.CreateInBoundsGEP(
            leftTy, baseValuePtr, {irBuilder.getInt32(0), irBuilder.getInt32(postMemberArrowExpr->member.memberIdx)});

        return irBuilder.CreateLoad(memberType, memberAddr);
    } else {
        llvm::Value *memberAddr =
            irBuilder.CreateInBoundsGEP(leftTy, baseValuePtr, {irBuilder.getInt32(0), irBuilder.getInt32(0)});
        llvm::Value *cast = irBuilder.CreateBitCast(memberAddr, llvm::PointerType::getUnqual(memberType));
        return irBuilder.CreateLoad(memberType, cast);
    }
    return nullptr;
}

llvm::Value *CodeGen::VisitFuncDeclStmt(FuncDeclStmt *funcDeclStmt) {
    CFuncType *cFuncTy         = llvm::dyn_cast<CFuncType>(funcDeclStmt->cType.get());
    llvm::FunctionType *funcTy = llvm::dyn_cast<llvm::FunctionType>(cFuncTy->AcceptVisitor(this));
    Function *thisFunc =
        Function::Create(funcTy, GlobalValue::LinkageTypes::ExternalLinkage, cFuncTy->GetName(), llvmModule.get());
    currFunc = thisFunc;

    AddGlobalVarToMap(cFuncTy->GetName(), thisFunc, funcTy);

    int i              = 0;
    const auto &params = cFuncTy->GetParams();
    for (auto &arg : thisFunc->args()) {
        arg.setName(params[i++].name);
    }

    if (!funcDeclStmt->blockStmt) {
        return nullptr;
    }

    PushScope();
    // alloc for local var
    i = 0;
    for (auto &arg : thisFunc->args()) {
        auto alloc = irBuilder.CreateAlloca(arg.getType(), nullptr, arg.getName());
        alloc->setAlignment(llvm::Align(params[i++].ty->GetAlign()));
        irBuilder.CreateStore(&arg, alloc);
        AddLocalVarToMap(arg.getName(), alloc, arg.getType());
    }

    BasicBlock *entryBB = BasicBlock::Create(llvmContext, "entry", thisFunc);
    irBuilder.SetInsertPoint(entryBB);
    funcDeclStmt->blockStmt->AcceptVisitor(this);
    PopScope();

    verifyFunction(*thisFunc);
    if (verifyModule(*llvmModule, &llvm::outs())) {
        llvmModule->print(llvm::outs(), nullptr);
    }
    return nullptr;
}

llvm::Value *CodeGen::VisitReturnStmt(ReturnStmt *returnStmt) {
    if (returnStmt->expr) {
        llvm::Value *retVal = returnStmt->expr->AcceptVisitor(this);
        return irBuilder.CreateRet(retVal);
    }
    return irBuilder.CreateRetVoid();
}

llvm::Value *CodeGen::VisitPostFuncCallExpr(PostFuncCallExpr *postFuncCallExpr) {
    llvm::Value *funcAddr      = postFuncCallExpr->leftNode->AcceptVisitor(this);
    llvm::FunctionType *funcTy = llvm::dyn_cast<llvm::FunctionType>(postFuncCallExpr->leftNode->cType->AcceptVisitor(this));

    llvm::SmallVector<llvm::Value *> args;
    for (auto &arg : postFuncCallExpr->args) {
        args.push_back(arg->AcceptVisitor(this));
    }
    return irBuilder.CreateCall(funcTy, funcAddr, args);
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

llvm::Type *CodeGen::VisitCRecordType(CRecordType *ty) {
    llvm::StructType *structType = llvm::StructType::getTypeByName(llvmContext, ty->GetName());
    if (structType) {
        return structType;
    }
    structType = llvm::StructType::create(llvmContext, ty->GetName());
    llvm::SmallVector<llvm::Type *> vec;
    if (ty->GetTagKind() == TagKind::kSturct) {
        for (auto &m : ty->GetMerbers()) {
            vec.push_back(m.cType->AcceptVisitor(this));
        }
        structType->setBody(vec);
    } else {
        auto members = ty->GetMerbers();
        int idx      = ty->GetMaxElemSizeidx();
        vec.push_back(members[idx].cType->AcceptVisitor(this));
        structType->setBody(vec);
    }

    return structType;
}

llvm::Type *CodeGen::VisitCFuncType(CFuncType *ty) {
    llvm::Type *retTy = ty->GetRetTy()->AcceptVisitor(this);
    llvm::SmallVector<llvm::Type *> args;
    for (auto &arg : ty->GetParams()) {
        args.push_back(arg.ty->AcceptVisitor(this));
    }
    return llvm::FunctionType::get(retTy, args, false);
}

void CodeGen::PushScope() {
    localVarAddrTypeMap.emplace_back();
}

void CodeGen::PopScope() {
    localVarAddrTypeMap.pop_back();
}

void CodeGen::ClearVarScope() {
    localVarAddrTypeMap.clear();
}

void CodeGen::AddLocalVarToMap(llvm::StringRef name, llvm::Value *addr, llvm::Type *ty) {
    localVarAddrTypeMap.back().insert({name, {addr, ty}});
}

void CodeGen::AddGlobalVarToMap(llvm::StringRef name, llvm::Value *addr, llvm::Type *ty) {
    globalVarAddrTypeMap.insert({name, {addr, ty}});
}

std::pair<llvm::Value *, llvm::Type *> CodeGen::GetLocalVarByName(llvm::StringRef name) {
    for (auto it = localVarAddrTypeMap.rbegin(); it != localVarAddrTypeMap.rend(); it++) {
        if (it->find(name) != it->end()) {
            return (*it)[name];
        }
    }
    assert(globalVarAddrTypeMap.find(name) != globalVarAddrTypeMap.end());
    return GetGlobalVarByName(name);
}

std::pair<llvm::Value *, llvm::Type *> CodeGen::GetGlobalVarByName(llvm::StringRef name) {
    if (globalVarAddrTypeMap.find(name) != globalVarAddrTypeMap.end()) {
        return globalVarAddrTypeMap[name];
    }
    return {};
}