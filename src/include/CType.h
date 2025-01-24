#pragma once
#ifndef _CTYPE_H_
#define _CTYPE_H_

#include <llvm/IR/Type.h>
#include <memory>

class CPrimaryType;
class CPointType;

class TypeVisitor {
  public:
    ~TypeVisitor() {
    }
    virtual llvm::Type *VisitCPrimaryType(CPrimaryType *ty) = 0;
    virtual llvm::Type *VisitCPointType(CPointType *ty)     = 0;
};

/// @brief Represents a data type in the C language.
/// @details This class is used to describe C language data types, including their size,
/// alignment requirements, and kind (e.g., integer types). It provides utilities for defining
/// specific types such as `int`.
class CType {
  public:
    enum class CTypeKind { TY_Int = 0, TY_Point };

  public:
    static std::shared_ptr<CType> IntType;

  public:
    CType(int size, int align, CTypeKind kind) : size(size), align(align), kind(kind) {
    }

    virtual ~CType() {
    }

    virtual void AcceptVisitor(TypeVisitor *v) {
    }

    const CTypeKind GetTypeKind() const {
        return kind;
    }

  private:
    int size;
    int align;
    CTypeKind kind;
};

class CPrimaryType : public CType {
  public:
    CPrimaryType(int size, int align, CTypeKind kind) : CType(size, align, kind) {
    }

    void AcceptVisitor(TypeVisitor *v) override {
        v->VisitCPrimaryType(this);
    }

    static bool classof(const CType *ty) {
        return ty->GetTypeKind() == CTypeKind::TY_Int;
    }

  private:
    std::shared_ptr<CType> baseType;
};

class CPointType : public CType {
  public:
    CPointType(std::shared_ptr<CType> baseType) : CType(8, 8, CTypeKind::TY_Point), baseType(baseType) {
    }

    std::shared_ptr<CType> GetBaseType() {
        return baseType;
    }

    void AcceptVisitor(TypeVisitor *v) override {
        v->VisitCPointType(this);
    }

    static bool classof(const CType *ty) {
        return ty->GetTypeKind() == CTypeKind::TY_Point;
    }

  private:
    std::shared_ptr<CType> baseType;
};

#endif //_CTYPE_H_