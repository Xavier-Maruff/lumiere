#ifndef LUMIERE_COM_GET_STRING
#define LUMIERE_COM_GET_STRING

#include <string>
#include "ast_enums.h"
#include "err_codes.h"

std::string get_string_expr_node_type(expr_node_type expr_node);
std::string get_string_bin_oper(bin_oper opcode);
std::string get_string_err_code(err_codes err_code);

#endif