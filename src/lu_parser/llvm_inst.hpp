#ifndef LUMIERE_PARSER_LLVM_INST
#define LUMIERE_PARSER_LLVM_INST

#include <memory>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/GlobalValue.h>

//main context
extern std::unique_ptr<llvm::LLVMContext> llvm_context;
//main module
extern std::unique_ptr<llvm::Module> llvm_module;
//IR builder
extern std::unique_ptr<llvm::IRBuilder<>> llvm_irbuilder;

#endif