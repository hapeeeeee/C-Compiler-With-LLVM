#pragma once
#ifndef _CTYPE_H_
#define _CTYPE_H_

enum class CTypeKind {
    Int = 0,
};

class CType {
  public:
    CType(int size, int align, CTypeKind kind);
    static CType *getIntTy();

  private:
    int size;
    int align;
    CTypeKind kind;
};

#endif //_CTYPE_H_