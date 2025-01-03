#include "include/CType.h"

CType::CType(int size, int align, CTypeKind kind) : size(size), align(align), kind(kind) {
}

CType *CType::getIntTy() {
    static CType type(4, 4, CTypeKind::Int);
    return &type;
}