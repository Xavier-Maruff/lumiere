#ifndef LUMIERE_PARSER_AST
#define LUMIERE_PARSER_AST

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <llvm/IR/Value.h>

#include <tuple>
#include <functional>

#include "ast_enums.h"

//reduce a pair of value pointers to one value pointer
typedef std::function<llvm::Value*(llvm::Value*, llvm::Value*)> llvm_reduce_value_func;
//map binary expression type -> value pair reduce function
typedef std::map<std::tuple<expr_node_type, bin_oper, expr_node_type>, llvm_reduce_value_func> bin_oper_reduce_func_map_type;
//the actual binary operation function map
extern bin_oper_reduce_func_map_type bin_oper_reduce_func_map;

//base ast node ~ ABSTRACT
class ast_node{
    public:
    ast_node();
    virtual ~ast_node() = 0;

};

//code block - not yet implement in parser
class ast_block: public ast_node{
    public:
    std::vector<ast_node*> children;

    ast_block();
    ~ast_block();

};

//base ast expression
class ast_expr: public ast_node{
    public:
    static const expr_node_type base_expr_type;

    ast_expr();
    virtual ~ast_expr();

    //generate the IR
    virtual llvm::Value* gen_code();
    //get the expression type (see "expr_node_type" enum)
    virtual expr_node_type get_expr_type();
};

//baked variable expression
class ast_var_expr: public ast_expr{
    public:
    static const expr_node_type var_expr_type;
    expr_node_type var_type;
    //variable name
    std::string name;

    ast_var_expr();
    ast_var_expr(std::string name_);
    virtual ~ast_var_expr();

    llvm::Value* gen_code() override;
    expr_node_type get_expr_type() override;
};

//float expression
class ast_flt_expr: public ast_expr{
    public:
    static const expr_node_type flt_expr_type;
    //expression double value
    double value;

    ast_flt_expr();
    ast_flt_expr(double value_);
    virtual ~ast_flt_expr();

    llvm::Value* gen_code() override;
    expr_node_type get_expr_type() override;
};

//integer expression
class ast_int_expr: public ast_expr{
    public:
    static const expr_node_type int_expr_type;
    //expression int64 value
    int64_t value;

    ast_int_expr();
    ast_int_expr(int64_t value_);
    virtual ~ast_int_expr();
    
    llvm::Value* gen_code() override;
    expr_node_type get_expr_type() override;
};

//string expression
class ast_string_expr: public ast_expr{
    public:
    static const expr_node_type string_expr_type;
    //expression string value
    std::string value;

    ast_string_expr();
    ast_string_expr(std::string value_);
    virtual ~ast_string_expr();
    
    llvm::Value* gen_code() override;
    expr_node_type get_expr_type() override;
};

//binary expression
class ast_bin_expr: public ast_expr{
    public:
    static const expr_node_type bin_expr_type;
    //operator - see "bin_oper" enum
    bin_oper opcode;
    //pointers to adjacent nodes
    std::unique_ptr<ast_expr> lhs;
    std::unique_ptr<ast_expr> rhs;

    ast_bin_expr();
    ast_bin_expr(bin_oper opcode_, std::unique_ptr<ast_expr>& lhs_, std::unique_ptr<ast_expr>& rhs_);
    virtual ~ast_bin_expr();

    llvm::Value* gen_code() override;
    expr_node_type get_expr_type() override;
};


class ast_func_call_expr: public ast_expr{
    public:
    static const expr_node_type func_call_expr_type;
    std::string callee;
    std::vector<std::unique_ptr<ast_expr>> args;

    ast_func_call_expr();
    virtual ~ast_func_call_expr();
};

class ast_func_proto{
    public:
    std::string name;
    std::vector<std::string> args;

    ast_func_proto();
    virtual ~ast_func_proto();
};

class ast_func_def{
    public:
    std::unique_ptr<ast_func_proto> func_proto;
    std::unique_ptr<ast_expr> func_body;

    ast_func_def();
    virtual ~ast_func_def();
};

#endif