#ifndef LUMIERE_COM_GET_STRING
#define LUMIERE_COM_GET_STRING

/**
 * @file get_string.hpp
 * @author Xavier Maruff (xavier.maruff@outlook.com)
 * @brief Functions that convert lumiere enums to strings
 * @version 0.1
 * @date 2021-08-22
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <string>
#include "ast_enums.h"
#include "err_codes.h"

std::string get_string_expr_node_type(expr_node_type expr_node);
std::string get_string_bin_oper(bin_oper opcode);
std::string get_string_unary_oper(unary_oper opcode);
std::string get_string_err_code(err_codes err_code);

#endif