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


    /**
     * @brief Set the expression init val - mostly used for global variables
     * 
     * @param start_expr uptr to the initial value expression
     */
    virtual void set_init_val(std::unique_ptr<ast_expr>& start_expr);
};

/**
 * @brief Code block node
 * 
 */
class ast_block: public ast_node{
    public:
    std::vector<std::unique_ptr<ast_node>> children;

    ast_block(std::vector<std::unique_ptr<ast_node>>& children_);
    ast_block();
    virtual ~ast_block();

    virtual llvm::Value* gen_code() override;
};

/**
 * @brief Function code block
 * 
 */
class ast_func_block: public ast_block{
    public:
    std::unique_ptr<ast_expr> return_expr;

    /**
     * @brief Construct a new ast func block object
     * 
     * @param children_ Discrete expressions in the block
     * @param return_expr_ Return expression
     */
    ast_func_block(std::vector<std::unique_ptr<ast_node>>& children_, std::unique_ptr<ast_expr>& return_expr_);

    /**
     * @brief No return expression - used for void functions
     */
    ast_func_block(std::vector<std::unique_ptr<ast_node>>& children_);

    /**
     * @brief Just a return expression - used for single expression functions
     */
    ast_func_block(std::unique_ptr<ast_expr>& return_expr_);

    /**
     * @brief Empty void function block
     */
    ast_func_block();
    ~ast_func_block();

    llvm::Value* gen_code() override;
};

/**
 * @brief Variable expression node
 * 
 */
class ast_var_expr: public ast_expr{
    public:
    /**
     * @brief Initival value - does not need to be set for non-global variables
     * 
     */
    std::unique_ptr<ast_expr> init_val;
    bool is_global;

    ast_var_expr();
    /**
     * @brief Constructor - used for variables being declared
     * 
     * @param cmp_node_type_ Variable type
     * @param name_ Variable name
     * @param is_global_ Not used currently
     */
    ast_var_expr(std::string cmp_node_type_, std::string name_, bool is_global_ = false);

    /**
     * @brief Constructor used for variables already declared (type not apparent from parse)
     * 
     * @param name_ Variable name
     * @param is_global_ Not used currently
     */
    ast_var_expr(std::string name_, bool is_global_ = false);
    virtual ~ast_var_expr();

    llvm::Value* gen_code() override;

    /**
     * @brief Set the initial value of the variable - mostly used for globals
     * 
     * @param start_expr Uptr to the initial value expression
     */
    void set_init_val(std::unique_ptr<ast_expr>& start_expr) override;
    llvm::Value* gen_global_def();
};

/**
 * @brief Float literal node - actually a double
 * 
 */
class ast_flt_expr: public ast_expr{
    public:
    /**
     * @brief The double value of the expression
     * 
     */
    double value;

    ast_flt_expr();
    /**
     * @brief Constructor with value
     * 
     * @param value_ the double value
     */
    ast_flt_expr(double value_);
    virtual ~ast_flt_expr();

    llvm::Value* gen_code() override;

};

/**
 * @brief Integer (int_64t) expression
 * 
 */
class ast_int_expr: public ast_expr{
    public:
    /**
     * @brief Expression integer value
     * 
     */
    int64_t value;

    ast_int_expr();
    /**
     * @brief Constructor with new int value
     * 
     * @param value_ the integer value of the expression
     */
    ast_int_expr(int64_t value_);
    virtual ~ast_int_expr();
    
    llvm::Value* gen_code() override;

};

/**
 * @brief String expression node
 * 
 */
class ast_string_expr: public ast_expr{
    public:
    /**
     * @brief Value of the node as std::string
     * 
     */
    std::string value;

    ast_string_expr();
    /**
     * @brief Constructor with std::string expression value
     * 
     * @param value_ 
     */
    ast_string_expr(std::string value_);
    virtual ~ast_string_expr();

    /**
     * @brief Replaces string special characters with the correct characters (ie \\n to newline)
     * 
     * @param value_ 
     */
    static void replace_special_chars(std::string& value_);
    llvm::Value* gen_code() override;

};

/**
 * @brief Binary expression node
 * 
 */
class ast_bin_expr: public ast_expr{
    public:
    /**
     * @brief Binary operation opcode
     * 
     */
    bin_oper opcode;
    /**
     * @brief Uptr to left hand side node
     * 
     */
    std::unique_ptr<ast_expr> lhs;
    /**
     * @brief Uptr to right hand side node
     * 
     */
    std::unique_ptr<ast_expr> rhs;
    /**
     * @brief Pointer to function that reduces the binary expression to one llvm::Value*
     * 
     */
    reduce_binary_value* code_gen_func;

    ast_bin_expr();
    /**
     * @brief Constructor
     * 
     * @param opcode_ Operation code
     * @param lhs_ Uptr to LHS expression
     * @param rhs_ Uptr to RHS expression
     */
    ast_bin_expr(bin_oper opcode_, std::unique_ptr<ast_expr>& lhs_, std::unique_ptr<ast_expr>& rhs_);
    virtual ~ast_bin_expr();

    llvm::Value* gen_code() override;

};

/**
 * @brief Unary expression node
 * 
 */
class ast_unary_expr: public ast_expr{
    public:
    /**
     * @brief Unary operation code
     * 
     */
    unary_oper opcode;
    /**
     * @brief Node that is operated upon
     * 
     */
    std::unique_ptr<ast_expr> target;
    /**
     * @brief Pointer to function that transforms the target
     * 
     */
    reduce_unary_value* code_gen_func;

    ast_unary_expr();
    /**
     * @brief Constructor
     * 
     * @param opcode_ The operation code
     * @param target_ Uptr to the target node
     */
    ast_unary_expr(unary_oper opcode_, std::unique_ptr<ast_expr>& target_);
    virtual ~ast_unary_expr();

    llvm::Value* gen_code() override;

};

/**
 * @brief Function call node
 * 
 */
class ast_func_call_expr: public ast_expr{
    public:
    /**
     * @brief Name of the function being called
     * 
     */
    std::string callee;
    /**
     * @brief Vector of uptrs to the function arg expressions
     * 
     */
    std::vector<std::unique_ptr<ast_expr>> args;

    /**
     * @brief Constructor
     * 
     * @param callee_ Name of the function being called
     * @param args_ Vector of uptrs to the function args nodes
     */
    ast_func_call_expr(std::string callee_, std::vector<std::unique_ptr<ast_expr>>& args_);
    virtual ~ast_func_call_expr();

    llvm::Value* gen_code() override;

};

/**
 * @brief Function prototype node
 * 
 */
class ast_func_proto: public ast_node{
    public:
    /**
     * @brief Vector of uptrs to the function args
     * 
     */
    std::vector<std::unique_ptr<ast_node>> args;
    /**
     * @brief Whether the function has variadic arguments or not
     * 
     */
    bool variadic;
    /**
     * @brief Function return type
     * 
     */
    std::string return_type;

    ast_func_proto(std::string name_, std::vector<std::unique_ptr<ast_node>>& args_, std::string return_type_);
    ast_func_proto(std::string name_, std::string return_type_);
    virtual ~ast_func_proto();

    llvm::Function* gen_code();
};

/**
 * @brief Function definition node
 * 
 */
class ast_func_def: public ast_node{
    public:
    /**
    * @brief Uptr to the function prototype
    * 
    */
    std::unique_ptr<ast_func_proto> func_proto;
    /**
     * @brief Uptr to the function body
     * 
     */
    std::unique_ptr<ast_node> func_body;

    /**
     * @brief Constructor
     * 
     * @param func_proto_ Uptr to the function prototype
     * @param func_body_ Uptr to the function body as a node
     */
    ast_func_def(std::unique_ptr<ast_func_proto>& func_proto_, std::unique_ptr<ast_node>& func_body_);

    /**
     * @brief Constructor
     * 
     * @param func_proto_ Uptr to the function prototype
     * @param func_body_ Uptr to the function body as a function block
     */
    ast_func_def(std::unique_ptr<ast_func_proto>& func_proto_, std::unique_ptr<ast_func_block>& func_body_);
    virtual ~ast_func_def();

    llvm::Function* gen_code();
};


#endif