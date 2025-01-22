#include "include/CType.h"

std::shared_ptr<CType> CType::IntType = std::make_shared<CPrimaryType>(4, 4, CTypeKind::TY_Int);
