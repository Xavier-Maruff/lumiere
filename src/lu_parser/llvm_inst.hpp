#ifndef LUMIERE_PARSER_LLVM_INST
#define LUMIERE_PARSER_LLVM_INST

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
#include <algorithm>
#include <string>

//main context
extern std::unique_ptr<llvm::LLVMContext> llvm_context;
//main module
extern std::unique_ptr<llvm::Module> llvm_module;
//ir builder
extern std::unique_ptr<llvm::IRBuilder<>> llvm_irbuilder;

//Get symbol type
std::function<llvm::Type*(llvm::LLVMContext&)>* get_llvm_type(std::string type_name);

//insert an alloca at function entry
llvm::AllocaInst* insert_alloca_at_entry(llvm::Function*, const std::string, const std::string);
//insert alloca at block entry
llvm::AllocaInst* insert_alloca_at_entry(llvm::BasicBlock*, const std::string, const std::string);
//insert alloca at current cursor position
llvm::AllocaInst* insert_alloca(llvm::BasicBlock* b_to_insert, const std::string var_name, const std::string type_name);

//create global variable
llvm::GlobalVariable* create_global_var(std::string var_name, std::string var_type, llvm::Constant* init_val = nullptr);

#endif