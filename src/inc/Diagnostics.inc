#ifndef DIAG
#define DIAG(ID, KIND, MSG)
#endif

/// Lexer
DIAG(error_unknown_char, Error, "unknown char '{0}'")

/// Parser
DIAG(error_except, Error, "except '{0}', but found '{1}'")
DIAG(error_break_not_in_loop, Error, "'break' not in loop or switch stmt")
DIAG(error_continue_not_in_loop, Error, "'continue' not in loop stmt")

/// Sema
DIAG(error_redefined, Error, "redefined symbol '{0}'")
DIAG(error_undefined, Error, "undefined symbol '{0}'")
DIAG(error_lvalue, Error, "Required lvalue on the assign operation left side")

#undef DIAG
