#ifndef LUMIERE_PARSER_AST
#define LUMIERE_PARSER_AST

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <llvm/IR/Value.h>

#include <tuple>
#include <functional>

enum bin_oper {
    OPER_ADD,
    OPER_MULT,
    OPER_SUB,
    OPER_DIV,
    OPER_MOD
};

enum expr_node_type {
    BASE_EXPR_NODE,
    VAR_EXPR_NODE,
    FLT_EXPR_NODE,
    INT_EXPR_NODE,
    STRING_EXPR_NODE,
    BIN_EXPR_NODE
};


typedef std::function<llvm::Value*(llvm::Value*, llvm::Value*)> llvm_reduce_value_func;
typedef std::map<std::tuple<expr_node_type, bin_oper, expr_node_type>, llvm_reduce_value_func> bin_oper_reduce_func_map_type;
extern bin_oper_reduce_func_map_type bin_oper_reduce_func_map;

std::string get_expr_node_type_string(expr_node_type expr_node);
std::string get_bin_oper_string(bin_oper opcode);

class ast_node{
    public:
    ast_node();
    virtual ~ast_node() = 0;

};

class ast_block: public ast_node{
    public:
    std::vector<ast_node*> children;

    ast_block();
    ~ast_block();

};

class ast_expr: public ast_node{
    public:
    static const expr_node_type expr_type;

    ast_expr();
    virtual ~ast_expr();
    virtual llvm::Value* gen_code();
};

class ast_var_expr: public ast_expr{
    public:
    static const expr_node_type expr_type;
    std::string name;

    ast_var_expr();
    ast_var_expr(std::string name_);
    virtual ~ast_var_expr();

    llvm::Value* gen_code() override;
};

class ast_flt_expr: public ast_expr{
    public:
    static const expr_node_type expr_type;
    double value;

    ast_flt_expr();
    ast_flt_expr(double value_);
    virtual ~ast_flt_expr();

    llvm::Value* gen_code() override;
};

class ast_int_expr: public ast_expr{
    public:
    static const expr_node_type expr_type;
    int64_t value;

    ast_int_expr();
    ast_int_expr(int64_t value_);
    virtual ~ast_int_expr();
    
    llvm::Value* gen_code() override;
};


class ast_string_expr: public ast_expr{
    public:
    static const expr_node_type expr_type;
    std::string value;

    ast_string_expr();
    ast_string_expr(std::string value_);
    virtual ~ast_string_expr();
    
    llvm::Value* gen_code() override;
};


class ast_bin_expr: public ast_expr{
    public:
    static const expr_node_type expr_type;
    bin_oper opcode;
    std::unique_ptr<ast_expr> lhs;
    std::unique_ptr<ast_expr> rhs;

    ast_bin_expr();
    ast_bin_expr(bin_oper opcode_, std::unique_ptr<ast_expr> lhs_, std::unique_ptr<ast_expr> rhs_);
    virtual ~ast_bin_expr();

    llvm::Value* gen_code() override;
};

/*
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