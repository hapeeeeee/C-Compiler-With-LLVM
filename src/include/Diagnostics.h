#pragma
// Diagnostics
#ifndef _DIAGNOSTICS_H_
#define _DIAGNOSTICS_H_

#include "llvm/Support/FormatVariadic.h"
#include "llvm/Support/SourceMgr.h"

namespace diag {
enum {
#define DIAG(ID, KIND, MSG) ID,
#include "../inc/Diagnostics.inc"
};
}; // namespace diag

/// @brief A diagnostics class for reporting messages and errors.
/// @details This class is used for reporting diagnostics such as errors, warnings, and notes during
/// compilation or interpretation. It integrates with LLVM's `SourceMgr` to associate messages with
/// source code locations and manage diagnostic types and messages defined in `Diagnostics.inc`.
class Diagnostics {
  public:
    Diagnostics(llvm::SourceMgr &mgr) : mgr(mgr) {
    }

    template <typename... Args> void Report(llvm::SMLoc loc, unsigned int diagId, Args... args) {
        auto kind = GetDiagKind(diagId);
        auto msg  = GetDiagMsg(diagId);
        auto m    = llvm::formatv(msg, std::forward<Args>(args)...).str();
        mgr.PrintMessage(loc, kind, m);
        if (kind == llvm::SourceMgr::DK_Error) {
            exit(-1);
        }
    }

  private:
    llvm::SourceMgr &mgr;

  private:
    llvm::SourceMgr::DiagKind GetDiagKind(unsigned int id);
    const char *GetDiagMsg(unsigned int id);
};

#endif // _DIAGNOSTICS_H_