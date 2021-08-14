#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/APInt.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/GlobalValue.h>
//#include <llvm/IR/Verifier.h>

#include <unordered_map>

#include "ast.hpp"
#include "log.hpp"

static std::unique_ptr<llvm::LLVMContext> llvm_context = std::make_unique<llvm::LLVMContext>();
static std::unique_ptr<llvm::Module> llvm_module = std::make_unique<llvm::Module>("lumiere", *llvm_context);
static std::unique_ptr<llvm::IRBuilder<>> llvm_irbuilder = std::make_unique<llvm::IRBuilder<>>(*llvm_context);
static std::map<std::string, llvm::Value*> value_map = {};

//contains the binary operation mappings
bin_oper_reduce_func_map_type bin_oper_reduce_func_map = {
    {{FLT_EXPR_NODE, OPER_ADD, INT_EXPR_NODE}, [](llvm::Value* lhs, llvm::Value* rhs) -> llvm::Value* {
        return llvm_irbuilder->CreateAdd(lhs, rhs, "addtmp");
    }},
    {{INT_EXPR_NODE, OPER_ADD, FLT_EXPR_NODE}, [](llvm::Value* lhs, llvm::Value* rhs) -> llvm::Value* {
        return llvm_irbuilder->CreateFAdd(lhs, rhs, "addtmp");
    }},
    {{INT_EXPR_NODE, OPER_ADD, INT_EXPR_NODE}, [](llvm::Value* lhs, llvm::Value* rhs) -> llvm::Value* {
        return llvm_irbuilder->CreateAdd(lhs, rhs, "addtmp");
    }},
    {{FLT_EXPR_NODE, OPER_ADD, FLT_EXPR_NODE}, [](llvm::Value* lhs, llvm::Value* rhs) -> llvm::Value* {
        return llvm_irbuilder->CreateFAdd(lhs, rhs, "addtmp");
    }}
};

std::string get_expr_node_type_string(expr_node_type expr_node){
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
std::string get_bin_oper_string(bin_oper opcode){
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

        default:
        return "INVALID_OPCODE";
        break;
    }
    return "INVALID_OPCODE";
}

//abstract ast node base
ast_node::ast_node(){
    //
}

ast_node::~ast_node(){
    //
}

//ast expression base
ast_expr::ast_expr():
ast_node(){
    //
}

ast_expr::~ast_expr(){
    //
}

llvm::Value* ast_expr::gen_code(){
    return nullptr;
}

const expr_node_type ast_expr::expr_type = BASE_EXPR_NODE;


//ast variable expression
ast_var_expr::ast_var_expr():
ast_expr(), name(){
    //
}

ast_var_expr::ast_var_expr(std::string name_):
ast_expr(), name(name_){
    //
}

ast_var_expr::~ast_var_expr(){
    //
}

llvm::Value* ast_var_expr::gen_code(){
    auto iter = value_map.find(name);
    if(iter != value_map.end()) return iter->second;
    else{
        ERR_LOG("Could not find "+name+" in constant map");
        return nullptr;
    }
}

const expr_node_type ast_var_expr::expr_type = VAR_EXPR_NODE;

//ast float expression (but its a double)
ast_flt_expr::ast_flt_expr():
ast_expr(), value(0){
    //
}

ast_flt_expr::ast_flt_expr(double value_):
ast_expr(), value(value_){
    //
}

ast_flt_expr::~ast_flt_expr(){
    //
}

llvm::Value* ast_flt_expr::gen_code(){
    return llvm::ConstantFP::get(*llvm_context, llvm::APFloat(value));
}

const expr_node_type ast_flt_expr::expr_type = FLT_EXPR_NODE;

//ast integer expression
ast_int_expr::ast_int_expr():
ast_expr(), value(0){
    //
}

ast_int_expr::ast_int_expr(int64_t value_):
ast_expr(), value(value_){
    //
}

ast_int_expr::~ast_int_expr(){
    //
}

llvm::Value* ast_int_expr::gen_code(){
                                                // args: numbits, value, isSigned
    return llvm::ConstantInt::get(*llvm_context, llvm::APInt(64, value, true));
}

const expr_node_type ast_int_expr::expr_type = INT_EXPR_NODE;

//ast statement block
ast_block::ast_block():
ast_node(), children(){
    //
}

ast_block::~ast_block(){
    //
}

//ast string expr
ast_string_expr::ast_string_expr():
ast_expr(), value(){
    //
}

ast_string_expr::ast_string_expr(std::string value_):
ast_expr(), value(value_){
    //
}

ast_string_expr::~ast_string_expr(){
    //
}

llvm::Value* ast_string_expr::gen_code(){
    return llvm_irbuilder->CreateGlobalStringPtr(value);
}

const expr_node_type ast_string_expr::expr_type = STRING_EXPR_NODE;


//ast binary expression
ast_bin_expr::ast_bin_expr():
ast_expr(), opcode(OPER_ADD), lhs(nullptr), rhs(nullptr){
    //
}

ast_bin_expr::ast_bin_expr(bin_oper opcode_, std::unique_ptr<ast_expr> lhs_, std::unique_ptr<ast_expr> rhs_):
ast_expr(), opcode(opcode_), lhs(std::move(lhs_)), rhs(std::move(rhs_)){
    //
}

ast_bin_expr::~ast_bin_expr(){
    //
}

llvm::Value* ast_bin_expr::gen_code(){
    llvm::Value* lhs_val = lhs->gen_code();
    llvm::Value* rhs_val = rhs->gen_code();
    if(lhs_val == nullptr || rhs_val == nullptr) return nullptr;
    auto code_gen_func_iter = bin_oper_reduce_func_map.find({lhs->expr_type, opcode, rhs->expr_type});
    if(code_gen_func_iter == bin_oper_reduce_func_map.end()) {
        ERR_LOG("No "+get_bin_oper_string(opcode)+" operation for nodes "+get_expr_node_type_string(lhs->expr_type)\
        +" and "+get_expr_node_type_string(rhs->expr_type));
        return nullptr;
    }
    return code_gen_func_iter->second(lhs_val, rhs_val);
}

const expr_node_type ast_bin_expr::expr_type = BIN_EXPR_NODE;