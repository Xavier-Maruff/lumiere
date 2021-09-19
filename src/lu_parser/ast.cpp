/**
 * @file ast.cpp
 * @author Xavier Maruff (xavier.maruff@outlook.com)
 * @brief Contains the AST definitions
 * @version 0.1
 * @date 2021-08-23
 * 
 * @copyright Copyright (c) 2021
 * 
 */

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
#include <boost/algorithm/string/replace.hpp>

#include "ast.hpp"
#include "log.hpp"
#include "get_string.hpp"
#include "llvm_inst.hpp"

//abstract ast node base
ast_node::ast_node() {
    cmp_node_type = "node";
    node_id = current_node_id++;
}

ast_node::~ast_node() {
    //
}

std::ostream& ast_node::log_err(){
    return stdlog.err() << node_id_source_info_map[node_id];
}

std::ostream& ast_node::log_warn(){
    return stdlog.warn() << node_id_source_info_map[node_id];
}

std::ostream& ast_node::log_info(){
    return stdlog() << node_id_source_info_map[node_id];
}

llvm::Value* ast_node::gen_code() {
    return nullptr;
}

//ast expression base
ast_expr::ast_expr() :
ast_node() {
    cmp_node_type = "expr";
}

void ast_expr::set_init_val(std::unique_ptr<ast_expr>& start_expr){
    //dummy
}

ast_expr::~ast_expr() {
    //
}

//ast variable expression
ast_var_expr::ast_var_expr() :
    ast_expr() {
    //
}

ast_var_expr::ast_var_expr(std::string cmp_node_type_, std::string name_, bool is_global_) :
    ast_expr(), is_global(is_global_) {
    name = name_;
    cmp_node_type = cmp_node_type_;

    //TODO: this does not work
    //auto type_iter = symbol_type_map.find(name);
    //if(type_iter == symbol_type_map.end()) symbol_type_map[name] = cmp_node_type;

    
    //this might cause issue when travelling up through scope - unless buffering of symbol type map is implemented
    symbol_type_map[name] = cmp_node_type;
}

ast_var_expr::ast_var_expr(std::string name_, bool is_global_) :
    ast_expr(), is_global(is_global_) {
    name = name_;

    //this might cause issue when travelling up through scope
    cmp_node_type = symbol_type_map[name];
}

ast_var_expr::~ast_var_expr() {
    //
}

void ast_var_expr::set_init_val(std::unique_ptr<ast_expr>& start_expr){
    if(start_expr == nullptr){
        log_warn()  << "Nullptr initial value for var " << name << std::endl;
    }
    init_val = std::move(start_expr);
}

//create the global definition
llvm::Value* ast_var_expr::gen_global_def(){
    llvm::Value* ret_val = nullptr;
    llvm::Value* init_val_gen = init_val->gen_code();
    if(init_val_gen == nullptr){
        log_err()  << "Initial value for var " << name << " return nullptr at codegen" << std::endl;
        throw PARSE_ERR;
    }
    //cast to a constant
    llvm::Constant* constant_expr = llvm::dyn_cast<llvm::Constant>(init_val_gen);
    if(constant_expr == nullptr) {
        log_err()  << "Nullptr returned casting init val to llvm::Constant for global " << name << std::endl;
        throw PARSE_ERR;
    }
    //create the global variable
    llvm::Value* global_var = create_global_var(name, cmp_node_type, constant_expr);
    if(global_var == nullptr){
        log_err()  << "Nullptr returned creating global var " << name << std::endl;
        throw PARSE_ERR;
    }
    //record the global var in the symbol table
    value_map[name] = global_var;
    symbol_type_map[name] = cmp_node_type;
    global_symbol_map[name] = global_var;
    global_symbol_type_map[name] = cmp_node_type;

    ret_val = global_var;

    declared_symbols.insert(name);
    defined_symbols.insert(name);
    return ret_val;
}

llvm::Value* ast_var_expr::gen_code(){

    if(cmp_node_type == std::string()){
        log_err() << "Use of undeclared variable " << name << std::endl;
        throw PARSE_ERR;
    }

    llvm::Value* ret_val = nullptr;
    llvm::BasicBlock* parent_b = llvm_irbuilder->GetInsertBlock();

    //var needs to be declared
    if(declared_symbols.find(name) == declared_symbols.end()){
        //create the var declaration
        //block is nullptr or global explicitly stated, must be a global var
        //llvm say globals vars MUST be assigned at declaration
        if(parent_b == nullptr || global_symbols.find(name) != global_symbols.end()){
            is_global = true;
            global_symbols.insert(name);
            if(init_val == nullptr){
                log_warn()  << "No initial value provided for global var " << name << ", init as default" << std::endl;
                return nullptr;
                //TODO: default type primitives
            }
            else {
                //declare and define global var
                //get the initial value
                ret_val = gen_global_def();
                //maybe return retval here? TODO:
            }
        }
        else {
            //declare a local variable
            //llvm::AllocaInst* var_alloca = insert_alloca_at_entry(parent_b, name, cmp_node_type);
            llvm::AllocaInst* var_alloca = insert_alloca(parent_b, name, cmp_node_type);

            if(var_alloca == nullptr) {
                log_err()  << "Variable " << name << " allocation failed" << std::endl;
                throw PARSE_ERR;
            }

            value_map[name] = var_alloca;
            ret_val = var_alloca;

            declared_symbols.insert(name);

            //maybe not, test
            return ret_val;
        }
    }

    //check that there is a valid value
    if(global_symbols.find(name) != global_symbols.end()) ret_val = global_symbol_map[name];
    else ret_val = value_map[name];

    if(ret_val == nullptr){
        log_err()  << "Nullptr alloca value found for var " << name << std::endl;
        throw PARSE_ERR;
    }

    //var has already been declared, check if it has been defined
    if(defined_symbols.find(name) == defined_symbols.end() && !is_global) {
        //if not defined, store the initial value
        if(init_val != nullptr){
            llvm::Value* init_val_gen = init_val->gen_code();
            if(init_val_gen == nullptr) {
                log_err()  << "Nullptr initial value after codegen for var " << name << std::endl;
                throw PARSE_ERR;
            }
            llvm_irbuilder->CreateStore(init_val_gen, ret_val);
            defined_symbols.insert(name);
        }
        //else log_warn()  << "No init val provided for var " << name << std::endl;
    }

    //get the variable type information
    type_transform_func* type_ptr = get_llvm_type(cmp_node_type);
    if(type_ptr == nullptr) {
        log_err()  << "Variable " << name << " type (" << cmp_node_type << ") not recognised" << std::endl;
        throw PARSE_ERR;
    }
    llvm::Type* var_type = type_ptr->operator()(*llvm_context);
    if(var_type == nullptr){
        log_err()  << "Error retrieving variable " << name << " type (" << cmp_node_type << ")" << std::endl;
        throw PARSE_ERR;
    }

    //return the load instruction
    return llvm_irbuilder->CreateLoad(var_type, ret_val, name.c_str());
}




//ast float expression (but its a double)
ast_flt_expr::ast_flt_expr() :
    ast_expr(), value(0) {
    cmp_node_type = "float";
}

ast_flt_expr::ast_flt_expr(double value_) :
    ast_expr(), value(value_) {
    //
}

ast_flt_expr::~ast_flt_expr() {
    //
}

llvm::Value* ast_flt_expr::gen_code() {
    return llvm::ConstantFP::get(*llvm_context, llvm::APFloat(value));
}




//ast integer expression
ast_int_expr::ast_int_expr() :
    ast_expr(), value(0) {
    cmp_node_type = "int";
}

ast_int_expr::ast_int_expr(int64_t value_) :
    ast_expr(), value(value_) {
    //
}

ast_int_expr::~ast_int_expr() {
    //
}

llvm::Value* ast_int_expr::gen_code() {
    return llvm::ConstantInt::get(*llvm_context, llvm::APInt(64, value, true));
}




//ast statement block
ast_block::ast_block(std::vector<std::unique_ptr<ast_node>>& children_) :
    ast_node(), children(std::move(children_)) {
    cmp_node_type = "int";
}

ast_block::ast_block() {
    //
}

ast_block::~ast_block() {
    //
}

llvm::Value* ast_block::gen_code() {
    for (std::unique_ptr<ast_node>& child : children) {
        child->gen_code();
    }
    //default to return 1
    return nullptr;
}

//ast string expr
ast_string_expr::ast_string_expr() :
    ast_expr(), value() {
    cmp_node_type = "string";
}

ast_string_expr::ast_string_expr(std::string value_) :
    ast_expr(), value(value_) {
    replace_special_chars(value);
}

ast_string_expr::~ast_string_expr() {
    //
}

void ast_string_expr::replace_special_chars(std::string& value_){
    //TODO: this is a REALLY bad way of doing this, rewrite it 
    boost::replace_all(value_, "\\n", "\n");
    boost::replace_all(value_, "\\r", "\r");
}

//get ref to string value
//TODO: this is segfaulting in global insert
llvm::Value* ast_string_expr::gen_code() {
    llvm::BasicBlock* parent_b = llvm_irbuilder->GetInsertBlock();
    if(parent_b == nullptr){
        log_warn()  << "Creating global strings not yet implemented" << std::endl;
        //return llvm_irbuilder->
        return nullptr;
    }
    return llvm_irbuilder->CreateGlobalStringPtr(llvm::StringRef(value));
}





//ast binary expression
ast_bin_expr::ast_bin_expr() :
    ast_expr(), opcode(OPER_ADD), lhs(nullptr), rhs(nullptr) {
    cmp_node_type = "binexpr";
}

//actual constructor
ast_bin_expr::ast_bin_expr(bin_oper opcode_, std::unique_ptr<ast_expr>& lhs_, std::unique_ptr<ast_expr>& rhs_) :
    ast_expr(), opcode(opcode_), lhs(std::move(lhs_)), rhs(std::move(rhs_)) {
    //
}

ast_bin_expr::~ast_bin_expr() {
    //
}

llvm::Value* ast_bin_expr::gen_code() {
    
    llvm::Value* lhs_val;
    llvm::Value* rhs_val;

    //special case for assignment
    if (opcode == OPER_ASSIGN) {
        // Assignment requires the LHS to be an identifier.
        ast_var_expr *lhs_raw = dynamic_cast<ast_var_expr*>(lhs.get());
        if(lhs_raw == nullptr){
            log_err()  << "Only variables can be assigned to" << std::endl;
            throw PARSE_ERR;
        }
        lhs_val = value_map[lhs_raw->name];
        if(declared_symbols.find(lhs_raw->name) == declared_symbols.end() || lhs_val == nullptr){
            log_err()  << "Assignment to undeclared variable " << lhs_raw->name << std::endl;
            throw PARSE_ERR;
        }
    }
    else lhs_val = lhs->gen_code();
    
    rhs_val = rhs->gen_code();

    if (lhs_val == nullptr || rhs_val == nullptr) {
        log_err()  << "Nullptr node in binexpr" << std::endl;
        throw PARSE_ERR;
    }

    //get the iterator pointing to the function that will produce the correct value pointer
    auto code_gen_func_iter = bin_oper_reduce_map.find({ lhs->cmp_node_type, opcode, rhs->cmp_node_type });
    //check if the operation is valid
    if (code_gen_func_iter == bin_oper_reduce_map.end()) {
        //No valid operation for the two adjacent nodes
        log_err()  << "No " << get_string_bin_oper(opcode) << " operation for nodes "
            << lhs->cmp_node_type << " (" << lhs->name << "), "
            << " and " << rhs->cmp_node_type << " (" << rhs->name << ")" << std::endl;
        //throw err
        throw PARSE_ERR;
    }
    code_gen_func = &code_gen_func_iter->second;
    cmp_node_type = code_gen_func_iter->second.return_type;
    
    if(code_gen_func == nullptr){
        log_err()  << "Code gen func for bin expr retreived as nullptr" << std::endl;
        throw PARSE_ERR; 
    }
    //generate the value pointer
    return code_gen_func->operator()(lhs_val, rhs_val);
}




ast_unary_expr::ast_unary_expr(unary_oper opcode_, std::unique_ptr<ast_expr>& target_):
ast_expr(), opcode(opcode_), target(std::move(target_)){
    //
}

ast_unary_expr::~ast_unary_expr(){
    //
}



llvm::Value* ast_unary_expr::gen_code() {
    //get the value of the two adjacent nodes
    llvm::Value* target_val = target->gen_code();
    if (target_val == nullptr) return nullptr;

    //get the iterator pointing to the function that will produce the correct value pointer
    auto code_gen_func_iter = unary_oper_reduce_map.find({opcode, target->cmp_node_type});
    //check if the operation is valid
    if (code_gen_func_iter == unary_oper_reduce_map.end()) {
        //No valid operation for the two adjacent nodes
        log_err()  << "No " << get_string_unary_oper(opcode) << " operation for node "
            << target->cmp_node_type << std::endl;
        //throw err
        throw PARSE_ERR;
    }
    else {
        code_gen_func = &code_gen_func_iter->second;
        cmp_node_type = code_gen_func_iter->second.return_type;
    }


    //generate the value pointer
    return code_gen_func->operator()(target_val);
}

//function call expression
ast_func_call_expr::ast_func_call_expr(std::string callee_, std::vector<std::unique_ptr<ast_expr>>& args_) :
    ast_expr(), callee(callee_), args(std::move(args_)) {
    cmp_node_type = symbol_type_map[callee];
}

ast_func_call_expr::~ast_func_call_expr() {
    //
}

//TODO:
llvm::Value* ast_func_call_expr::gen_code() {
    llvm::Function* f_callee = llvm_module->getFunction(callee);
    if (f_callee == nullptr) {
        log_err()  << "Function \"" << callee << "\" not recognised" << std::endl;
        throw PARSE_ERR;
    }
    //todo: also check arg type
    if (f_callee->arg_size() > args.size()) {
        log_err()  << "Invalid arguments supplied to function \"" << callee << std::endl;
        throw PARSE_ERR;
    }
    std::vector<llvm::Value*> arg_values;
    int func_args_size = func_args_type_map[callee].size();
    for(uint index = 0; index < args.size(); index++) {
        llvm::Value* arg_code_gen = args[index]->gen_code();
        if(arg_code_gen == nullptr) return nullptr;
        if (index < func_args_size && args[index]->cmp_node_type != func_args_type_map[callee][index]) {
            log_err()  << callee << " argument " << index << " is of type " << args[index]->cmp_node_type
                << ", was expecting " << func_args_type_map[callee][index] << std::endl;
                throw PARSE_ERR;
        }
        arg_values.push_back(arg_code_gen);
    }

    //return llvm_irbuilder->CreateCall(f_callee, arg_values, "calltmp");
    if(symbol_type_map[callee] == "void") return llvm_irbuilder->CreateCall(f_callee, arg_values);
    else return llvm_irbuilder->CreateCall(f_callee, arg_values, "calltmp");
}


//function prototype
ast_func_proto::ast_func_proto(std::string name_, std::vector<std::unique_ptr<ast_node>>& args_, std::string return_type_) :
    ast_node(), args(std::move(args_)), return_type(return_type_), variadic(false) {
    name = name_;
    symbol_type_map[name] = return_type;
    func_args_type_map[name] = {};
    for (std::unique_ptr<ast_node>& arg : args) {
        func_args_type_map[name].push_back(arg->cmp_node_type);
    }
    cmp_node_type = return_type;
}

ast_func_proto::ast_func_proto(std::string name_, std::string return_type_) :
ast_node(), return_type(return_type_), variadic(false) {
    name = name_;
    symbol_type_map[name] = return_type;
    func_args_type_map[name] = {};
    cmp_node_type = return_type;
}

ast_func_proto::~ast_func_proto() {
    //
}

//generate ir
llvm::Function* ast_func_proto::gen_code() {
    //return nullptr;
    std::vector<llvm::Type*> arg_llvm_types;
    for (std::unique_ptr<ast_node>& arg : args) {
        //get the arg type as an llvm::Type*
        arg_llvm_types.push_back(type_map[arg->cmp_node_type](*llvm_context));
    }
    //generate function type                                                                                   //TODO: varargs functions
    llvm::FunctionType* f_llvm_type = llvm::FunctionType::get(type_map[return_type](*llvm_context), arg_llvm_types, variadic);
    llvm::Function* f_llvm = llvm::Function::Create(f_llvm_type, llvm::Function::ExternalLinkage, name, llvm_module.get());

    uint arg_index = 0;
    for (auto& arg : f_llvm->args()) {
        arg.setName(args[arg_index++]->name);
    }

    return f_llvm;
}

//function definition
ast_func_def::ast_func_def(std::unique_ptr<ast_func_proto>& proto_, std::unique_ptr<ast_node>& func_body_) :
    ast_node(), func_proto(std::move(proto_)), func_body(std::move(func_body_)) {
    cmp_node_type = func_proto->cmp_node_type;
}

ast_func_def::ast_func_def(std::unique_ptr<ast_func_proto>& proto_, std::unique_ptr<ast_func_block>& func_body_) :
    ast_node(), func_proto(std::move(proto_)), func_body(std::move(func_body_)) {
    cmp_node_type = func_proto->cmp_node_type;
}

/*ast_func_def::ast_func_def(std::unique_ptr<ast_func_proto>& proto_):
ast_node(), func_proto(std::move(proto_)){
    cmp_node_type = func_proto->cmp_node_type;
}*/

ast_func_def::~ast_func_def() {
    //
}

//TODO: check this
llvm::Function* ast_func_def::gen_code() {

    llvm::Function* llvm_f = llvm_module->getFunction(func_proto->name);
    if (llvm_f == nullptr) llvm_f = func_proto->gen_code();
    if (llvm_f == nullptr) {
        log_err()  << "Function prototype code gen returned nullptr" << std::endl;
        return nullptr;
    }

    llvm::BasicBlock* block_f = llvm::BasicBlock::Create(*llvm_context, "entry", llvm_f);
    llvm_irbuilder->SetInsertPoint(block_f);

    //might do an outer join, then revert after
    //value_map_buffer.clear();
    //value_map_buffer = value_map;
    store_tables_to_buffer();
    std::vector<std::string> arg_types = func_args_type_map[func_proto->name];
    size_t arg_index = 0;
    for (auto& arg : llvm_f->args()) {
        llvm::AllocaInst* arg_alloca = insert_alloca_at_entry(llvm_f, arg.getName(), arg_types[arg_index]);
        llvm_irbuilder->CreateStore(&arg, arg_alloca);
        std::string arg_name(arg.getName());
        value_map[arg_name] = arg_alloca;
        declared_symbols.insert(arg_name);
        defined_symbols.insert(arg_name);
        arg_index++;
    }

    llvm::Value* return_value = func_body->gen_code();

    //maybe should throw error
    if (func_body->cmp_node_type != func_proto->return_type) {
        log_warn()  << "Function " << func_proto->name << " is declared to return " << func_proto->return_type
            << ",  but returns " << func_body->cmp_node_type << std::endl;
    }

    if (return_value != nullptr) {
        llvm_irbuilder->CreateRet(return_value);
        llvm::verifyFunction(*llvm_f);
        load_tables_from_buffer();
        //value_map = value_map_buffer;
        //value_map_buffer.clear();
        return llvm_f;
    }
    else if(func_body->cmp_node_type == "void"){
        llvm_irbuilder->CreateRet(nullptr);
        llvm::verifyFunction(*llvm_f);
        load_tables_from_buffer();
        return llvm_f;
    }

    log_err()  << "Func body gen code returned nullptr" << std::endl;
    llvm_f->eraseFromParent();
    load_tables_from_buffer();
    //value_map = value_map_buffer;
    //value_map_buffer.clear();
    return nullptr;

}

//ast function body block - same as block, but with return expression
ast_func_block::ast_func_block(std::vector<std::unique_ptr<ast_node>>& children_, std::unique_ptr<ast_expr>& return_expr_) :
    ast_block(children_), return_expr(std::move(return_expr_)) {
    cmp_node_type = return_expr->cmp_node_type;
}

ast_func_block::ast_func_block(std::vector<std::unique_ptr<ast_node>>& children_) :
    ast_block(children_) {
    cmp_node_type = "void";
    return_expr = nullptr;
}

ast_func_block::ast_func_block(std::unique_ptr<ast_expr>& return_expr_) :
    ast_block(), return_expr(std::move(return_expr_)) {
    cmp_node_type = return_expr->cmp_node_type;
}

ast_func_block::ast_func_block():
ast_block(){
    cmp_node_type = "void";
    return_expr = nullptr;
}

ast_func_block::~ast_func_block() {
    //
}

llvm::Value* ast_func_block::gen_code() {
    for (std::unique_ptr<ast_node>& child : children) {
        if(child != nullptr) child->gen_code();
        else log_warn()  << "Null child" << std::endl;
    }
    if(return_expr == nullptr) return nullptr;

    llvm::Value* ret_val = return_expr->gen_code();
    cmp_node_type = return_expr->cmp_node_type;
    if(ret_val == nullptr) log_warn()  << "Null return from function " << std::endl;
    return ret_val;
}