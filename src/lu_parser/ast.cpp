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
#include <llvm/IR/Verifier.h>

#include <unordered_map>

#include "ast.hpp"
#include "log.hpp"
#include "get_string.hpp"
#include "llvm_inst.hpp"

//abstract ast node base
ast_node::ast_node(){
    cmp_node_type = "node";
}

ast_node::~ast_node(){
    //
}

llvm::Value* ast_node::gen_code(){
    return nullptr;
}

//ast expression base
ast_expr::ast_expr():
ast_node(){
    cmp_node_type = "expr";
}

ast_expr::~ast_expr(){
    //
}

//TODO: remove all the get_expr_type methods, theyre DUMB and REDUNDANT and EW and WHY
std::string ast_expr::get_expr_type(){
    return "expr";
}

const expr_node_type ast_expr::base_expr_type = BASE_EXPR_NODE;


//ast variable expression
ast_var_expr::ast_var_expr():
ast_expr(){
    //
}

ast_var_expr::ast_var_expr(std::string cmp_node_type_, std::string name_):
ast_expr(){
    name = name_;
    cmp_node_type = cmp_node_type_;
}

ast_var_expr::~ast_var_expr(){
    //
}

llvm::Value* ast_var_expr::gen_code(){
    //TODO: mutable variables
    llvm::Value* val = value_map[name];
    if(val == nullptr){
        stdlog.err() << "Unrecognised variable \"" << name << "\"" << std::endl;
        return nullptr;
    }
    return val;
}

std::string ast_var_expr::get_expr_type(){
    return "var";
}

const expr_node_type ast_var_expr::var_expr_type = VAR_EXPR_NODE;

//ast float expression (but its a double)
ast_flt_expr::ast_flt_expr():
ast_expr(), value(0){
    cmp_node_type = "float";
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

std::string ast_flt_expr::get_expr_type(){
    return "float";
}

const expr_node_type ast_flt_expr::flt_expr_type = FLT_EXPR_NODE;

//ast integer expression
ast_int_expr::ast_int_expr():
ast_expr(), value(0){
    cmp_node_type = "int";
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

std::string ast_int_expr::get_expr_type(){
    return "int";
}

const expr_node_type ast_int_expr::int_expr_type = INT_EXPR_NODE;

//ast statement block
ast_block::ast_block(std::vector<std::unique_ptr<ast_node>>& children_):
ast_node(), children(std::move(children_)){
    cmp_node_type = "block";
}

ast_block::ast_block(){
    //
}

ast_block::~ast_block(){
    //
}

llvm::Value* ast_block::gen_code(){
    for(std::unique_ptr<ast_node>& child: children){
        child->gen_code();
    }
    return nullptr;
}

//ast string expr
ast_string_expr::ast_string_expr():
ast_expr(), value(){
    cmp_node_type = "string";
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

std::string ast_string_expr::get_expr_type(){
    return "string";
}

const expr_node_type ast_string_expr::string_expr_type = STRING_EXPR_NODE;


//ast binary expression
ast_bin_expr::ast_bin_expr():
ast_expr(), opcode(OPER_ADD), lhs(nullptr), rhs(nullptr){
    cmp_node_type = "binexpr";
}

//actual constructor
ast_bin_expr::ast_bin_expr(bin_oper opcode_, std::unique_ptr<ast_expr>& lhs_, std::unique_ptr<ast_expr>& rhs_):
ast_expr(), opcode(opcode_), lhs(std::move(lhs_)), rhs(std::move(rhs_)){
    //get the iterator pointing to the function that will produce the correct value pointer
    auto code_gen_func_iter = bin_oper_reduce_func_map.find({lhs->cmp_node_type, opcode, rhs->cmp_node_type});
    //check if the operation is valid
    if(code_gen_func_iter == bin_oper_reduce_func_map.end()) {
        //No valid operation for the two adjacent nodes
        stdlog.err() << "No " << get_string_bin_oper(opcode) << " operation for nodes "
        << lhs->cmp_node_type
        << " and " << rhs->cmp_node_type<< std::endl;
        //throw err
        throw PARSE_ERR;
    }
    else {
        code_gen_func = &code_gen_func_iter->second;
        cmp_node_type = code_gen_func_iter->second.return_type;
    }
}

ast_bin_expr::~ast_bin_expr(){
    //
}

llvm::Value* ast_bin_expr::gen_code(){
    //get the value of the two adjacent nodes
    llvm::Value* lhs_val = lhs->gen_code();
    llvm::Value* rhs_val = rhs->gen_code();
    if(lhs_val == nullptr || rhs_val == nullptr) return nullptr;
    //generate the value pointer
    return code_gen_func->operator()(lhs_val, rhs_val);
}

std::string ast_bin_expr::get_expr_type(){
    return "binary";
}

const expr_node_type ast_bin_expr::bin_expr_type = BIN_EXPR_NODE;

//function call expression
ast_func_call_expr::ast_func_call_expr(std::string callee_, std::vector<std::unique_ptr<ast_expr>>& args_):
ast_expr(), callee(callee_), args(std::move(args_)){
    cmp_node_type = "fcall";
}

ast_func_call_expr::~ast_func_call_expr(){
    //
}

//TODO:
llvm::Value* ast_func_call_expr::gen_code(){
   llvm::Function* f_callee = llvm_module->getFunction(callee);
   if(f_callee == nullptr) {
       //TODO: add line no from yy::parser
       stdlog.err() << "Function \"" << callee << "\" not recognised (line " << std::endl;
       return nullptr;
   }
   //todo: also check arg type
   if(f_callee->arg_size() != args.size()) {
       stdlog.err() << "Invalid arguments supplied to function \"" << callee << "\" (line " << std::endl;
   }
   std::vector<llvm::Value*> arg_values;
   for(uint index = 0; index < args.size(); index++){
       arg_values.push_back(args[index]->gen_code());
       if(arg_values.back() == nullptr) return nullptr;
   }

   return llvm_irbuilder->CreateCall(f_callee, arg_values, "calltmp");
}

std::string ast_func_call_expr::get_expr_type(){
    std::string proto_return_type = symbol_type_map[callee];
    if(proto_return_type == std::string()) return "unknown";
    else return proto_return_type;
}


const expr_node_type ast_func_call_expr::func_call_expr_type = FUNC_CALL_EXPR_NODE;

//function prototype
ast_func_proto::ast_func_proto(std::string name_, std::vector<std::unique_ptr<ast_node>>& args_, std::string return_type_):
ast_node(), args(std::move(args_)), return_type(return_type_){
    name = name_;
    symbol_type_map[name] = return_type;
    cmp_node_type = return_type;
}

ast_func_proto::ast_func_proto(std::string name_, std::string return_type_):
ast_node(), return_type(return_type_){
    name = name_;
    symbol_type_map[name] = return_type;
    cmp_node_type = return_type;
}

ast_func_proto::~ast_func_proto(){
    //
}

//generate ir
llvm::Function* ast_func_proto::gen_code(){
    //return nullptr;
    std::vector<llvm::Type*> arg_llvm_types;
    for(std::unique_ptr<ast_node>& arg: args){
        //get the arg type as an llvm::Type*
        arg_llvm_types.push_back(type_map[arg->cmp_node_type](*llvm_context));
    }
    //generate function type                                                                                      might need this as true, TODO:
    llvm::FunctionType* f_llvm_type = llvm::FunctionType::get(type_map[return_type](*llvm_context), arg_llvm_types, false);
    llvm::Function* f_llvm = llvm::Function::Create(f_llvm_type, llvm::Function::ExternalLinkage, name, llvm_module.get());

    uint arg_index = 0;
    for(auto& arg: f_llvm->args()){
        arg.setName(args[arg_index++]->name);
    }

    return f_llvm;
}

//function definition
ast_func_def::ast_func_def(std::unique_ptr<ast_func_proto>& proto_, std::unique_ptr<ast_node>& func_body_):
ast_node(), func_proto(std::move(proto_)), func_body(std::move(func_body_)){
    cmp_node_type = func_proto->cmp_node_type;
}

ast_func_def::~ast_func_def(){
    //
}

//TODO: check this
llvm::Function* ast_func_def::gen_code(){

    llvm::Function* llvm_f = llvm_module->getFunction(func_proto->name);
    if(llvm_f == nullptr) llvm_f = func_proto->gen_code();
    if(llvm_f == nullptr) {
        stdlog.err() << "Function prototype code gen returned nullptr" << std::endl;
        return nullptr;
    }

    llvm::BasicBlock* block_f = llvm::BasicBlock::Create(*llvm_context, "entry", llvm_f);
    llvm_irbuilder->SetInsertPoint(block_f);

    value_map.clear();
    for(auto& arg: llvm_f->args()){
        value_map[std::string(arg.getName())] = &arg;
    }

    //maybe should throw error
    if(func_body->cmp_node_type != func_proto->return_type) {
        stdlog.warn() << "Function " << func_proto->name << " is declared to return " << func_proto->return_type
        << ",  but returns " << func_body->cmp_node_type << std::endl;
    }

    llvm::Value* return_value = func_body->gen_code();
    if(return_value != nullptr) {
        llvm_irbuilder->CreateRet(return_value);
        llvm::verifyFunction(*llvm_f);
        return llvm_f;
    }

    stdlog.err() << "Func body gen code returned nullptr" << std::endl;
    llvm_f->eraseFromParent();
    return nullptr;

}

//ast function body block - same as block, but with return expression
ast_func_block::ast_func_block(std::vector<std::unique_ptr<ast_node>>& children_, std::unique_ptr<ast_expr>& return_expr_):
ast_block(children_), return_expr(std::move(return_expr_)){
    cmp_node_type = return_expr->cmp_node_type;
}

ast_func_block::ast_func_block(std::unique_ptr<ast_expr>& return_expr_):
ast_block(), return_expr(std::move(return_expr_)){
    cmp_node_type = return_expr->cmp_node_type;
}

ast_func_block::~ast_func_block(){
    //
}

llvm::Value* ast_func_block::gen_code(){
    for(std::unique_ptr<ast_node>& child: children){
        child->gen_code();
    }
    return return_expr->gen_code();
}





