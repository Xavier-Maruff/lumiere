#ifndef LUMIERE_PARSER_AST_ENUMS
#define LUMIERE_PARSER_AST_ENUMS

//binary expression operator types
enum bin_oper {
    OPER_ADD,
    OPER_MULT,
    OPER_SUB,
    OPER_DIV,
    OPER_MOD
};

//unary expression operator types
enum unary_oper {
    U_OPER_NEG
};

//expression ast node types
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