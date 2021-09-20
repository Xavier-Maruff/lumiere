#ifndef LUMIERE_PARSER_LLVM_INST
#define LUMIERE_PARSER_LLVM_INST

/**
 * @file llvm_inst.hpp
 * @author Xavier Maruff (xavier.maruff@outlook.com)
 * @brief Contains the extern declarations of the LLVM instance, plus some helper functions
 * @version 0.1
 * @date 2021-08-23
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <memory>

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Utils.h"
#include "llvm/IR/LegacyPassManager.h"
#include <algorithm>
#include <string>

/**
 * @brief Uptr to the LLVM context
 * 
 */
extern std::unique_ptr<llvm::LLVMContext> llvm_context;

/**
 * @brief Uptr to the llvm module
 * 
 */
extern std::unique_ptr<llvm::Module> llvm_module;

/**
 * @brief Uptr to the llvm IR builder
 * 
 */
extern std::unique_ptr<llvm::IRBuilder<>> llvm_irbuilder;

/**
 * @brief Uptr to the llvm pass manager - TODO: migrate to updated pass manager
 * 
 */
extern std::unique_ptr<llvm::legacy::FunctionPassManager> llvm_func_pass_man;

/**
 * @brief Get the llvm type from a lumiere type
 * 
 * @param type_name lumiere typename
 * @return std::function<llvm::Type*(llvm::LLVMContext&)>* 
 */
std::function<llvm::Type*(llvm::LLVMContext&)>* get_llvm_type(std::string type_name);

/**
 * @brief Insert an alloca at the entry of a function
 * 
 * @param f_to_insert Pointer to function the alloca will be inserted into
 * @param var_name The name of the alloca
 * @param type_nme The lumiere type of the alloca
 * @return llvm::AllocaInst* The alloca instance
 */
llvm::AllocaInst* insert_alloca_at_entry(llvm::Function* f_to_insert, const std::string var_name, const std::string type_name);

/**
 * @brief Insert an alloca at the entry of a block
 * 
 * @param b_to_insert Pointer to block the alloca will be inserted into
 * @param var_name The name of the alloca
 * @param type_nme The lumiere type of the alloca
 * @return llvm::AllocaInst* The alloca instance
 */
llvm::AllocaInst* insert_alloca_at_entry(llvm::BasicBlock* b_to_insert, const std::string var_name, const std::string type_name);

/**
 * @brief Insert an alloca at the current cursor position in the block
 * 
 * @param b_to_insert The block to insert the alloca into
 * @param var_name The name of the alloca
 * @param type_name The lumiere type of the alloca
 * @return llvm::AllocaInst* Pointer to the alloca instance
 */
llvm::AllocaInst* insert_alloca(llvm::BasicBlock* b_to_insert, const std::string var_name, const std::string type_name);

/**
 * @brief Create a global variable
 * 
 * @param var_name Name of the global variable
 * @param var_type Lumiere type of the global variable
 * @param init_val Initial value of the global variable
 * @return llvm::GlobalVariable* Pointer to the global variable
 */
llvm::GlobalVariable* create_global_var(std::string var_name, std::string var_type, llvm::Constant* init_val = nullptr);

#endif