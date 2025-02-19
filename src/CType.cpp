#include "include/CType.h"

int RoundUp(int x, int align) {
    return (x + align - 1) & ~(align - 1);
}

std::shared_ptr<CType> CType::IntType = std::make_shared<CPrimaryType>(4, 4, CTypeKind::TY_Int);

CRecordType::CRecordType(llvm::StringRef name, std::vector<Member> members, TagKind tagKind)
    : CType(0, 0, CTypeKind::TY_Record), name(name), members(members), tagKind(tagKind) {
    if (tagKind == TagKind::kSturct) {
        UpdateStructOffest();
    } else {
        UpdateUnionOffest();
    }
}

void CRecordType::UpdateStructOffest() {
    int offset = 0, maxAlign = 0, maxSize = 0, maxSizeIdx = 0, idx = 0;
    int totalSize = 0;
    for (auto &m : members) {
        offset      = RoundUp(offset, m.cType->GetAlign());
        m.memberIdx = idx;
        m.offset    = offset;

        if (maxAlign < m.cType->GetAlign()) {
            maxAlign = m.cType->GetAlign();
        }

        if (maxSize < m.cType->GetSize()) {
            maxSize    = m.cType->GetSize();
            maxSizeIdx = idx;
        }
        idx++;

        // the next elem's start offset
        offset += m.cType->GetSize();
    }
    align          = maxAlign;
    size           = RoundUp(offset, maxAlign);
    maxElemSizeidx = maxSizeIdx;
}

void CRecordType::UpdateUnionOffest() {
    int offset = 0, maxAlign = 0, maxSize = 0, maxSizeIdx = 0, idx = 0;
    int totalSize = 0;

    for (auto &m : members) {
        m.memberIdx = idx;
        m.offset    = 0;

        if (maxAlign < m.cType->GetAlign()) {
            maxAlign = m.cType->GetAlign();
        }

        if (maxSize < m.cType->GetSize()) {
            maxSize    = m.cType->GetSize();
            maxSizeIdx = idx;
        }
        idx++;
        // the next elem's start offset
        offset += m.cType->GetSize();
    }

    size           = maxSize;
    align          = maxAlign;
    maxElemSizeidx = maxSizeIdx;
}
