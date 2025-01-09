#include "include/Diagnostics.h"
#include "llvm/Support/Format.h"

static const char *DIAG_MSG[] = {
#define DIAG(ID, KIND, MSG) MSG,
#include "inc/Diagnostics.inc"
};

static llvm::SourceMgr::DiagKind DIAG_KIND[] = {
#define DIAG(ID, KIND, MSG) llvm::SourceMgr::DK_##KIND,
#include "inc/Diagnostics.inc"
};

llvm::SourceMgr::DiagKind Diagnostics::GetDiagKind(unsigned int id) {
    return DIAG_KIND[id];
}

const char *Diagnostics::GetDiagMsg(unsigned int id) {
    return DIAG_MSG[id];
}
