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
#include "cmp_time_extensible.hpp"

//base ast node ~ ABSTRACT
class ast_node{
    public:
    std::string name;
    std::string cmp_node_type;

    ast_node();
    virtual ~ast_node() = 0;

    //generate the IR
    virtual llvm::Value* gen_code();

};

//base ast expression
class ast_expr: public ast_node{
    public:
    static const expr_node_type base_expr_type;

    ast_expr();
    virtual ~ast_expr();

    //get the expression type (see "expr_node_type" enum)
    virtual std::string get_expr_type();
};

//code block - not yet implement in parser
class ast_block: public ast_node{
    public:
    //this should probably be a vector of node pointers, not expr pointers
    std::vector<std::unique_ptr<ast_node>> children;

    ast_block(std::vector<std::unique_ptr<ast_node>>& children_);
    ast_block();
    virtual ~ast_block();

    virtual llvm::Value* gen_code() override;
};

class ast_func_block: public ast_block{
    public:
    std::unique_ptr<ast_expr> return_expr;

    ast_func_block(std::vector<std::unique_ptr<ast_node>>& children_, std::unique_ptr<ast_expr>& return_expr_);
    ast_func_block(std::unique_ptr<ast_expr>& return_expr_);
    ~ast_func_block();

    llvm::Value* gen_code() override;
};

//baked variable expression - TODO: mutable
class ast_var_expr: public ast_expr{
    public:
    static const expr_node_type var_expr_type;

    ast_var_expr();
    ast_var_expr(std::string cmp_node_type_, std::string name_);
    virtual ~ast_var_expr();

    llvm::Value* gen_code() override;
    std::string get_expr_type() override;
    //TODO: will be deprecated after mutable vars are added
    void assign_val(llvm::Value* val);
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
    std::string get_expr_type() override;
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
    std::string get_expr_type() override;
};

//string expression
//represented as an i8*
class ast_string_expr: public ast_expr{
    public:
    static const expr_node_type string_expr_type;
    //expression string value
    std::string value;

    ast_string_expr();
    ast_string_expr(std::string value_);
    virtual ~ast_string_expr();
    
    llvm::Value* gen_code() override;
    std::string get_expr_type() override;
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
    //binary reduction function (derived from types and opcode using binary reduction function map)
    llvm_reduce_value_func* code_gen_func;

    ast_bin_expr();
    ast_bin_expr(bin_oper opcode_, std::unique_ptr<ast_expr>& lhs_, std::unique_ptr<ast_expr>& rhs_);
    virtual ~ast_bin_expr();

    llvm::Value* gen_code() override;
    std::string get_expr_type() override;
};

//unary expression
class ast_unary_expr: public ast_expr{
    public:
    //unary operator
    unary_oper opcode;
    //pointer to adjacent node
    std::unique_ptr<ast_expr> target;
    llvm_reduce_unary_value_func* code_gen_func;

    ast_unary_expr();
    ast_unary_expr(unary_oper opcode_, std::unique_ptr<ast_expr>& target_);
    virtual ~ast_unary_expr();

    llvm::Value* gen_code() override;
    std::string get_expr_type() override;
};

//TODO: CODEGEN

class ast_func_call_expr: public ast_expr{
    public:
    static const expr_node_type func_call_expr_type;
    //the function name being called
    std::string callee;
    //function args
    std::vector<std::unique_ptr<ast_expr>> args;

    ast_func_call_expr(std::string callee_, std::vector<std::unique_ptr<ast_expr>>& args_);
    virtual ~ast_func_call_expr();

    llvm::Value* gen_code() override;
    std::string get_expr_type() override;
};

class ast_func_proto: public ast_node{
    public:
    //args at dec
    std::vector<std::unique_ptr<ast_node>> args;
    //function return type
    std::string return_type;

    ast_func_proto(std::string name_, std::vector<std::unique_ptr<ast_node>>& args_, std::string return_type_);
    ast_func_proto(std::string name_, std::string return_type_);
    virtual ~ast_func_proto();

    llvm::Function* gen_code();
};

class ast_func_def: public ast_node{
    public:
    //function protype - contains name and return type info
    std::unique_ptr<ast_func_proto> func_proto;
    //the function body - expression or func block
    std::unique_ptr<ast_node> func_body;

    ast_func_def(std::unique_ptr<ast_func_proto>& func_proto_, std::unique_ptr<ast_node>& func_body_);
    ast_func_def(std::unique_ptr<ast_func_proto>& func_proto_, std::unique_ptr<ast_func_block>& func_body_);
    //ast_func_def(std::unique_ptr<ast_func_proto>& func_proto_);
    virtual ~ast_func_def();

    llvm::Function* gen_code();
};


extern std::unique_ptr<ast_func_proto> main_func_proto;
extern std::unique_ptr<ast_func_block> main_func_block;
extern std::unique_ptr<ast_expr> main_func_ret;

#endif