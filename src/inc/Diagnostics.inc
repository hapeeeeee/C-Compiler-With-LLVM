#ifndef DIAG
#define DIAG(ID, KIND, MSG)
#endif

/// Lexer
DIAG(error_unknown_char, Error, "unknown char '{0}'")

/// Parser
DIAG(error_except, Error, "except '{0}', but found '{1}'")
DIAG(error_break_not_in_loop, Error, "'break' not in loop or switch stmt")
DIAG(error_continue_not_in_loop, Error, "'continue' not in loop stmt")
DIAG(error_case_not_in_switch, Error, "'case' not in switch stmt")
DIAG(error_default_not_in_switch, Error, "'default' not in switch stmt")
DIAG(error_repeat_default_in_switch, Error, "switch stmt only be allowed to own one default")

/// Sema
DIAG(error_redefined, Error, "redefined symbol '{0}'")
DIAG(error_undefined, Error, "undefined symbol '{0}'")
DIAG(error_lvalue, Error, "Required lvalue on the assign operation left side")
DIAG(unexpect_typename, Error, "Unexcept type name '{0}'")
DIAG(unsame_typename, Error, "Except Same type")
DIAG(unexcept_type, Error, "Except '{0}' type")
DIAG(error_miss, Error, "Miss '{0}'")
#undef DIAG
