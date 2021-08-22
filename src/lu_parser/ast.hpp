#ifndef LUMIERE_PARSER_AST
#define LUMIERE_PARSER_AST

/**
 * @file ast.hpp
 * @author Xavier Maruff (xavier.maruff@outlook.com)
 * @brief Contains the AST node classes declarations
 * @version 0.1
 * @date 2021-08-22
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <llvm/IR/Value.h>

#include <tuple>
#include <functional>

#include "ast_enums.h"
#include "cmp_time_extensible.hpp"

/**
 * @brief Abstract AST base node
 * 
 */
class ast_node{
    public:
    /**
     * @brief Node name
     * 
     */
    std::string name;
    /**
     * @brief Node type
     * 
     */
    std::string cmp_node_type;
    /**
     * @brief Node ID
     * 
     */
    size_t node_id;

    ast_node();
    virtual ~ast_node() = 0;

    /**
     * @brief Generate the LLVM IR
     * 
     * @return llvm::Value* Result of codegen
     */
    virtual llvm::Value* gen_code();

    /**
     * @brief Error logging formatted with source file location info
     * 
     * @return std::ostream& stderr
     */
    inline std::ostream& log_err();

    /**
     * @brief Warning logging formatted with source file location info
     * 
     * @return std::ostream& stdout
     */
    inline std::ostream& log_warn();

    /**
     * @brief Info logging formatted with source file location info
     * 
     * @return std::ostream& stdout
     */
    inline std::ostream& log_info();
};

/**
 * @brief Expression node
 * 
 */
class ast_expr: public ast_node{
    public:
    
    ast_expr();
    virtual ~ast_expr();

    /**
     * @brief Get the expression type
     * 
     * @return std::string 
     */
    virtual std::string get_expr_type();
    virtual void set_init_val(std::unique_ptr<ast_expr>& start_expr);
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
    ast_func_block(std::vector<std::unique_ptr<ast_node>>& children_);
    ast_func_block(std::unique_ptr<ast_expr>& return_expr_);
    ast_func_block();
    ~ast_func_block();

    llvm::Value* gen_code() override;
};

//baked variable expression - TODO: mutable
class ast_var_expr: public ast_expr{
    public:
        std::unique_ptr<ast_expr> init_val;
    bool is_global;

    ast_var_expr();
    ast_var_expr(std::string cmp_node_type_, std::string name_, bool is_global_ = false);
    ast_var_expr(std::string name_, bool is_global_ = false);
    virtual ~ast_var_expr();

    llvm::Value* gen_code() override;
    std::string get_expr_type() override;
    //TODO: will be deprecated after mutable vars are added
    void set_init_val(std::unique_ptr<ast_expr>& start_expr) override;
    llvm::Value* gen_global_def();
};

//float expression
class ast_flt_expr: public ast_expr{
    public:
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

//class ast_assign_expr: public ast_expr{
    //
//};

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
    bool variadic;
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


#endif