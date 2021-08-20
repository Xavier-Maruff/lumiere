#include "llvm_inst.hpp"

//instantiate the llvm globals
std::unique_ptr<llvm::LLVMContext> llvm_context = std::make_unique<llvm::LLVMContext>();
std::unique_ptr<llvm::Module> llvm_module = std::make_unique<llvm::Module>("lumiere", *llvm_context);
std::unique_ptr<llvm::IRBuilder<>> llvm_irbuilder = std::make_unique<llvm::IRBuilder<>>(*llvm_context);

#include "cmp_time_extensible.hpp"
#include "log.hpp"
#include "err_codes.h"

//get the function to transform lumiere types to llvm types
type_transform_func* get_llvm_type(std::string type_name) {
    auto type_map_iter = type_map.find(type_name);
    if (type_map_iter == type_map.end()) {
        //stdlog.err() << "Type \"" << type_name << "\" is not recognised" << std::endl;
        return nullptr;
    }
    else return &type_map_iter->second;
}

//insert an alloca at block entry
//function overload
llvm::AllocaInst* insert_alloca_at_entry(llvm::Function* f_to_insert, const std::string var_name, const std::string type_name) {
    type_transform_func* var_type_transform = get_llvm_type(type_name);

    if (var_type_transform == nullptr) {
        stdlog.err() << "Variable \"" << var_name << "\" is of an unrecognised type (\"" << type_name << "\")" << std::endl;
        //THROW?
        return nullptr;
    }
    llvm::IRBuilder<> temp_block(&f_to_insert->getEntryBlock(), f_to_insert->getEntryBlock().begin());
    return temp_block.CreateAlloca(var_type_transform->operator()(*llvm_context), 0, var_name.c_str());
}

//block overload
llvm::AllocaInst* insert_alloca_at_entry(llvm::BasicBlock* b_to_insert, const std::string var_name, const std::string type_name) {
    type_transform_func* var_type_transform = get_llvm_type(type_name);

    if (var_type_transform == nullptr) {
        stdlog.err() << "Variable \"" << var_name << "\" is of an unrecognised type (\"" << type_name << "\")" << std::endl;
        return nullptr;
    }
    llvm::IRBuilder<> temp_block(b_to_insert, b_to_insert->begin());
    return temp_block.CreateAlloca(var_type_transform->operator()(*llvm_context), 0, var_name.c_str());
}



//insert an alloca at the current position
//block overload
llvm::AllocaInst* insert_alloca(llvm::BasicBlock* b_to_insert, const std::string var_name, const std::string type_name) {
    type_transform_func* var_type_transform = get_llvm_type(type_name);

    if (var_type_transform == nullptr) {
        stdlog.err() << "Variable \"" << var_name << "\" is of an unrecognised type (\"" << type_name << "\")" << std::endl;
        return nullptr;
    }
    llvm::IRBuilder<> temp_block(b_to_insert);
    return temp_block.CreateAlloca(var_type_transform->operator()(*llvm_context), 0, var_name.c_str());
}




//create a global variable
llvm::GlobalVariable* create_global_var(std::string var_name, std::string type_name, llvm::Constant* init_val) {
    type_transform_func* var_type_transform = get_llvm_type(type_name);
    if (var_type_transform == nullptr) {
        stdlog.err() << "Global variable \"" << var_name << "\" declaration of unrecognised type \"" << type_name << "\"" << std::endl;
        return nullptr;
    }

    llvm_module->getOrInsertGlobal(var_name, var_type_transform->operator()(*llvm_context));
    llvm::GlobalVariable* global_var = llvm_module->getNamedGlobal(var_name);
    //using common linkage makes it fucky and i don't know why
    //global_var->setLinkage(llvm::GlobalValue::CommonLinkage);
    if(init_val != nullptr) global_var->setInitializer(init_val);
    //TODO: this needs to be auto generated based on type - maybe extend type map?
    global_var->setAlignment(llvm::MaybeAlign());
    return global_var;
}