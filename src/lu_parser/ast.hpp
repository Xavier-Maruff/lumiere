#ifndef LUMIERE_PARSER_AST
#define LUMIERE_PARSER_AST

#include <string>

class ast_node{
    public:
    ast_node();
    virtual ~ast_node() = 0;

};

class ast_expr: public ast_node{
    public:
    ast_expr();
    virtual ~ast_expr();
};

class ast_var_expr: public ast_expr{
    public:
    std::string name;

    ast_var_expr();
    ast_var_expr(std::string name_);
    virtual ~ast_var_expr();
};

class ast_flt_expr: public ast_expr{
    public:
    double value;

    ast_flt_expr();
    ast_flt_expr(double value_);
    virtual ~ast_flt_expr();
};

/*
class ast_int_expr: public ast_expr{
    public:
    ast_int_expr();
    virtual ~ast_int_expr();
};

class ast_string_expr: public ast_expr{
    public:
    ast_string_expr();
    virtual ~ast_string_expr();
};

class ast_bin_expr: public ast_expr{
    public:
    ast_bin_expr();
    virtual ~ast_bin_expr();
};

class ast_func_call_expr: public ast_expr{
    public:
    ast_func_call_expr();
    virtual ~ast_func_call_expr();
};

class ast_func_proto_expr: public ast_expr{
    public:
    ast_func_proto_expr();
    virtual ~ast_func_proto_expr();
};

class ast_func_def_expr: public ast_expr{
    public:
    ast_func_def_expr();
    virtual ~ast_func_def_expr();
};
*/

#endif