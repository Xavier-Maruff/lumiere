#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/APInt.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/ADT/StringRef.h>
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
#include "get_string.hpp"
#include "llvm_inst.hpp"

//named values
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

expr_node_type ast_expr::get_expr_type(){
    return base_expr_type;
}

const expr_node_type ast_expr::base_expr_type = BASE_EXPR_NODE;


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
    //TODO:
    return nullptr;
}

expr_node_type ast_var_expr::get_expr_type(){
    return var_expr_type;
}

const expr_node_type ast_var_expr::var_expr_type = VAR_EXPR_NODE;

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

expr_node_type ast_flt_expr::get_expr_type(){
    return flt_expr_type;
}

const expr_node_type ast_flt_expr::flt_expr_type = FLT_EXPR_NODE;

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

expr_node_type ast_int_expr::get_expr_type(){
    return int_expr_type;
}

const expr_node_type ast_int_expr::int_expr_type = INT_EXPR_NODE;

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

//get ref to string value
//not entirely sure that this is right
llvm::Value* ast_string_expr::gen_code(){
    return llvm::ConstantDataArray::getString(*llvm_context, value.c_str(), true);
}

expr_node_type ast_string_expr::get_expr_type(){
    return string_expr_type;
}

const expr_node_type ast_string_expr::string_expr_type = STRING_EXPR_NODE;


//ast binary expression
ast_bin_expr::ast_bin_expr():
ast_expr(), opcode(OPER_ADD), lhs(nullptr), rhs(nullptr){
    //
}

//actual constructor
ast_bin_expr::ast_bin_expr(bin_oper opcode_, std::unique_ptr<ast_expr>& lhs_, std::unique_ptr<ast_expr>& rhs_):
ast_expr(), opcode(opcode_), lhs(std::move(lhs_)), rhs(std::move(rhs_)){
    //
}

ast_bin_expr::~ast_bin_expr(){
    //
}

llvm::Value* ast_bin_expr::gen_code(){
    //get the value of the two adjacent nodes
    llvm::Value* lhs_val = lhs->gen_code();
    llvm::Value* rhs_val = rhs->gen_code();
    if(lhs_val == nullptr || rhs_val == nullptr) return nullptr;
    //get the iterator pointing to the function that will produce the correct value pointer
    auto code_gen_func_iter = bin_oper_reduce_func_map.find({lhs->get_expr_type(), opcode, rhs->get_expr_type()});
    //check if the operation is valid
    if(code_gen_func_iter == bin_oper_reduce_func_map.end()) {
        //No valid operation for the two adjacent nodes
        stdlog.err() << "No " << get_string_bin_oper(opcode) << " operation for nodes "
        << get_string_expr_node_type(lhs->get_expr_type())
        << " and " << get_string_expr_node_type(rhs->get_expr_type()) << std::endl;
        return nullptr;
    }
    //generate the value pointer
    return code_gen_func_iter->second(lhs_val, rhs_val);
}

expr_node_type ast_bin_expr::get_expr_type(){
    return bin_expr_type;
}

const expr_node_type ast_bin_expr::bin_expr_type = BIN_EXPR_NODE;

//TODO: This is where you left off, writing out the funciton class methods
//function call expression
ast_func_call_expr::ast_func_call_expr(std::string callee_, std::vector<std::unique_ptr<ast_expr>> args_):
ast_expr(), callee(callee_), args(std::move(args_)){
    //
}

ast_func_call_expr::~ast_func_call_expr(){
    //
}



