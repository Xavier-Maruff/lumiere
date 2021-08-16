#include "cmp_time_extensible.hpp"
#include "llvm_inst.hpp"

llvm_reduce_value_func::llvm_reduce_value_func(reduce_value_func reduce_func_, std::string return_type_):
reduce_func(reduce_func_), return_type(return_type_){
    //
}

llvm::Value* llvm_reduce_value_func::operator()(llvm::Value* lhs, llvm::Value* rhs){
    return reduce_func(lhs, rhs);
}

//contains the compile time types
type_map_type type_map = {
    TYPE_MAP_ELEM("float", {return llvm::Type::getDoubleTy(ctx);}),
    TYPE_MAP_ELEM("int", {return llvm::Type::getInt64Ty(ctx);}),
    TYPE_MAP_ELEM("string", {return llvm::Type::getInt8PtrTy(ctx);})
};


//contains the binary operation mappings
bin_oper_reduce_func_map_type bin_oper_reduce_func_map = {
    {{"float", OPER_ADD, "int"}, {[](llvm::Value* lhs, llvm::Value* rhs) -> llvm::Value* {
        return llvm_irbuilder->CreateAdd(lhs, llvm_irbuilder->CreateSIToFP(rhs, type_map["float"](*llvm_context)), "addtmp");
    }, "float"}},
    {{"int", OPER_ADD, "float"}, {[](llvm::Value* lhs, llvm::Value* rhs) -> llvm::Value* {
        return llvm_irbuilder->CreateFAdd(llvm_irbuilder->CreateSIToFP(lhs, type_map["float"](*llvm_context)), rhs, "addtmp");
    }, "float"}},
    {{"int", OPER_ADD, "int"}, {[](llvm::Value* lhs, llvm::Value* rhs) -> llvm::Value* {
        return llvm_irbuilder->CreateAdd(lhs, rhs, "addtmp");
    }, "int"}},
    {{"float", OPER_ADD, "float"}, {[](llvm::Value* lhs, llvm::Value* rhs) -> llvm::Value* {
        return llvm_irbuilder->CreateFAdd(lhs, rhs, "addtmp");
    }, "float"}}
};

std::map<std::string, std::string> symbol_type_map = {};
std::map<std::string, llvm::Value*> value_map = {};