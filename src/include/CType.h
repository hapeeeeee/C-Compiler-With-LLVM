#pragma once
#ifndef _CTYPE_H_
#define _CTYPE_H_

enum class CTypeKind {
    Int = 0,
};

/// @brief Represents a data type in the C language.
/// @details This class is used to describe C language data types, including their size,
/// alignment requirements, and kind (e.g., integer types). It provides utilities for defining
/// specific types such as `int`.
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