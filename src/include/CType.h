#pragma once
#ifndef _CTYPE_H_
#define _CTYPE_H_

#include <llvm/IR/Type.h>
#include <memory>

class CPrimaryType;
class CPointType;
class CArrayType;
class CRecordType;

class TypeVisitor {
  public:
    ~TypeVisitor() {
    }
    virtual llvm::Type *VisitCPrimaryType(CPrimaryType *ty) = 0;
    virtual llvm::Type *VisitCPointType(CPointType *ty)     = 0;
    virtual llvm::Type *VisitCArrayType(CArrayType *ty)     = 0;
    virtual llvm::Type *VisitCRecordType(CRecordType *ty)   = 0;
};

/// @brief Represents a data type in the C language.
/// @details This class is used to describe C language data types, including their size,
/// alignment requirements, and kind (e.g., integer types). It provides utilities for defining
/// specific types such as `int`.
class CType {
  public:
    enum class CTypeKind { TY_Int = 0, TY_Point, TY_Array, TY_Record };

  public:
    static std::shared_ptr<CType> IntType;

  public:
    CType(int size, int align, CTypeKind kind) : size(size), align(align), kind(kind) {
    }

    virtual ~CType() {
    }

    virtual llvm::Type *AcceptVisitor(TypeVisitor *v) {
        return nullptr;
    }

    const CTypeKind GetTypeKind() const {
        return kind;
    }

    const int GetSize() const {
        return size;
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

    llvm::Type *AcceptVisitor(TypeVisitor *v) override {
        return v->VisitCPrimaryType(this);
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

    llvm::Type *AcceptVisitor(TypeVisitor *v) override {
        return v->VisitCPointType(this);
    }

    static bool classof(const CType *ty) {
        return ty->GetTypeKind() == CTypeKind::TY_Point;
    }

  private:
    std::shared_ptr<CType> baseType;
};

class CArrayType : public CType {
  public:
    CArrayType(std::shared_ptr<CType> elementType, int elementCount)
        : CType(elementType->GetSize() * elementCount, elementType->GetSize(), CTypeKind::TY_Array), elementType(elementType),
          elementCount(elementCount) {
    }

    std::shared_ptr<CType> GetElementType() {
        return elementType;
    }

    const int GetElementCount() const {
        return elementCount;
    }

    llvm::Type *AcceptVisitor(TypeVisitor *v) override {
        return v->VisitCArrayType(this);
    }

    static bool classof(const CType *ty) {
        return ty->GetTypeKind() == CTypeKind::TY_Array;
    }

  private:
    std::shared_ptr<CType> elementType;
    int elementCount;
};

enum TagKind {
    kSturct = 0,
    kUnion,
};

struct Member {
    std::shared_ptr<CType> cType;
    llvm::StringRef name;
};

class CRecordType : public CType {
  public:
    CRecordType(llvm::StringRef name, std::vector<Member> members, TagKind tagKind)
        : CType(0, 0, CTypeKind::TY_Record), name(name), members(members), tagKind(tagKind) {
    }

    llvm::StringRef GetName() {
        return name;
    }

    std::vector<Member> GetMerbers() {
        return members;
    }

    TagKind GetTagKind() {
        return tagKind;
    }

    llvm::Type *AcceptVisitor(TypeVisitor *v) override {
        return v->VisitCRecordType(this);
    }

    static bool classof(const CType *ty) {
        return ty->GetTypeKind() == CTypeKind::TY_Record;
    }

  private:
    llvm::StringRef name;
    std::vector<Member> members;
    TagKind tagKind;
};

#endif //_CTYPE_H_