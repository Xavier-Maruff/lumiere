#include "llvm_inst.hpp"

//instantiate the llvm globals
std::unique_ptr<llvm::LLVMContext> llvm_context = std::make_unique<llvm::LLVMContext>();
std::unique_ptr<llvm::Module> llvm_module = std::make_unique<llvm::Module>("lumiere", *llvm_context);
std::unique_ptr<llvm::IRBuilder<>> llvm_irbuilder = std::make_unique<llvm::IRBuilder<>>(*llvm_context);