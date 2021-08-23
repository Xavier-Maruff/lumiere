#ifndef LUMIERE_PARSER_AST_ENUMS
#define LUMIERE_PARSER_AST_ENUMS

/**
 * @file ast_enums.h
 * @author Xavier Maruff (xavier.maruff@outlook.com)
 * @brief Contains the AST enum declarations
 * @version 0.1
 * @date 2021-08-23
 * 
 * @copyright Copyright (c) 2021
 * 
 */


/**
 * @brief Binary operation code enum
 * 
 */
enum bin_oper {
    OPER_ADD,
    OPER_MULT,
    OPER_SUB,
    OPER_DIV,
    OPER_MOD,
    OPER_ASSIGN
};

/**
 * @brief Unary operation code enum
 * 
 */
enum unary_oper {
    U_OPER_NEG
};

/**
 * @brief Expression node type enum
 * 
 */
enum expr_node_type {
    BASE_EXPR_NODE,
    VAR_EXPR_NODE,
    FLT_EXPR_NODE,
    INT_EXPR_NODE,
    STRING_EXPR_NODE,
    BIN_EXPR_NODE,
    FUNC_CALL_EXPR_NODE
};


#endif