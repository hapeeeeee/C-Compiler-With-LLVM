#ifndef DIAG
#define DIAG(ID, KIND, MSG)
#endif

/// Lexer
DIAG(error_unknown_char, Error, "unknown char '{0}'")

/// Parser
DIAG(error_except, Error, "except '{0}', but found '{1}'")
#undef DIAG
