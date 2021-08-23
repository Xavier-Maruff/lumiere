/**
 * @file get_string.cpp
 * @author Xavier Maruff (xavier.maruff@outlook.com)
 * @brief get_string function definitions
 * @version 0.1
 * @date 2021-08-23
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "get_string.hpp"

std::string get_string_expr_node_type(expr_node_type expr_node){
    switch(expr_node){
        case BASE_EXPR_NODE:
        return "BASE_EXPR_NODE";
        break;

        case VAR_EXPR_NODE:
        return "VAR_EXPR_NODE";
        break;

        case FLT_EXPR_NODE:
        return "FLT_EXPR_NODE";
        break;

        case INT_EXPR_NODE:
        return "INT_EXPR_NODE";
        break;

        case STRING_EXPR_NODE:
        return "STRING_EXPR_NODE";
        break;

        case BIN_EXPR_NODE:
        return "BIN_EXPR_NODE";
        break;

        default:
        return "INVALID_EXPR_NODE";
        break;
    }
    return "INVALID_EXPR_NODE";
}
std::string get_string_bin_oper(bin_oper opcode){
    switch(opcode){
        case OPER_ADD:
        return "OPER_ADD";
        break;

        case OPER_MULT:
        return "OPER_MULT";
        break;

        case OPER_SUB:
        return "OPER_SUB";
        break;

        case OPER_DIV:
        return "OPER_DIV";
        break;

        case OPER_MOD:
        return "OPER_MOD";
        break;

        case OPER_ASSIGN:
        return "OPER_ASSIGN";
        break;

        default:
        return "INVALID_OPCODE";
        break;
    }
    return "INVALID_OPCODE";
}

std::string get_string_err_code(err_codes err_code){
    switch(err_code){
        case FILE_OPEN_ERR:
        return "FILE_OPEN_ERR";
        break;

        case FILE_READ_ERR:
        return "FILE_READ_ERR";
        break;

        case CLI_INVALID_ARGS_ERR:
        return "CLI_INVALID_ARGS_ERR";
        break;

        case SYNTAX_ERR:
        return "SYNTAX_ERR";
        break;

        case PARSE_ERR:
        return "PARSE_ERR";
        break;

        default:
        return "UNKNOWN_ERR";
        break;
    }

    return "UNKNOWN_ERR";
}

std::string get_string_unary_oper(unary_oper opcode){
    switch(opcode){
        case U_OPER_NEG:
        return "U_OPER_NEG";
        break;

        default:
        return "UNKNOWN_UNARY_OPER";
        break;
    }
    return "UNKNOWN_UNARY_OPER";
}