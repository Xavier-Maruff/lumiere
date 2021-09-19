#ifndef LU_PARSER_module_context
#define LU_PARSER_module_context

#include <llvm/IR/Value.h>
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

#include <vector>
#include <memory>

#include "llvm_inst.hpp"
#include "cmp_time_extensible.hpp"

class module_context {
    public:

    module_context();
    ~module_context();

    llvm::Value* gen_code();
    
};

#endif