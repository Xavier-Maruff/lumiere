#include "cmp_time_extensible.hpp"
#include "llvm_inst.hpp"
#include "log.hpp"

#include <llvm/Transforms/Utils/IntegerDivision.h>

//binary value reduction
llvm_reduce_value_func::llvm_reduce_value_func(reduce_value_func reduce_func_, std::string return_type_):
reduce_func(reduce_func_), return_type(return_type_){
    //
}

llvm::Value* llvm_reduce_value_func::operator()(llvm::Value* lhs, llvm::Value* rhs){
    return reduce_func(lhs, rhs);
}


//unary value reduction
llvm_reduce_unary_value_func::llvm_reduce_unary_value_func(reduce_unary_value_func reduce_func_, std::string return_type_):
reduce_func(reduce_func_), return_type(return_type_){
    //
}

llvm::Value* llvm_reduce_unary_value_func::operator()(llvm::Value* target){
    return reduce_func(target);
}

//contains the compile time types
type_map_type type_map = {
    TYPE_MAP_ELEM("float", {return llvm::Type::getDoubleTy(ctx);}),
    TYPE_MAP_ELEM("int", {return llvm::Type::getInt64Ty(ctx);}),
    TYPE_MAP_ELEM("string", {return llvm::Type::getInt8PtrTy(ctx);})
};


//contains the binary operation mappings
bin_oper_reduce_func_map_type bin_oper_reduce_func_map = {
    //LHS + RHS
    {{"float", OPER_ADD, "int"}, {[](llvm::Value* lhs, llvm::Value* rhs) -> llvm::Value* {
        return llvm_irbuilder->CreateFAdd(lhs, llvm_irbuilder->CreateSIToFP(rhs, type_map["float"](*llvm_context)), "addtmp");
    }, "float"}},
    {{"int", OPER_ADD, "float"}, {[](llvm::Value* lhs, llvm::Value* rhs) -> llvm::Value* {
        return llvm_irbuilder->CreateFAdd(llvm_irbuilder->CreateSIToFP(lhs, type_map["float"](*llvm_context)), rhs, "addtmp");
    }, "float"}},
    {{"int", OPER_ADD, "int"}, {[](llvm::Value* lhs, llvm::Value* rhs) -> llvm::Value* {
        return llvm_irbuilder->CreateAdd(lhs, rhs, "addtmp");
    }, "int"}},
    {{"float", OPER_ADD, "float"}, {[](llvm::Value* lhs, llvm::Value* rhs) -> llvm::Value* {
        return llvm_irbuilder->CreateFAdd(lhs, rhs, "addtmp");
    }, "float"}},

    //LHS - RHS
    {{"float", OPER_SUB, "int"}, {[](llvm::Value* lhs, llvm::Value* rhs) -> llvm::Value* {
        return llvm_irbuilder->CreateFSub(lhs, llvm_irbuilder->CreateSIToFP(rhs, type_map["float"](*llvm_context)), "addtmp");
    }, "float"}},
    {{"int", OPER_SUB, "float"}, {[](llvm::Value* lhs, llvm::Value* rhs) -> llvm::Value* {
        return llvm_irbuilder->CreateFSub(llvm_irbuilder->CreateSIToFP(lhs, type_map["float"](*llvm_context)), rhs, "addtmp");
    }, "float"}},
    {{"int", OPER_SUB, "int"}, {[](llvm::Value* lhs, llvm::Value* rhs) -> llvm::Value* {
        return llvm_irbuilder->CreateSub(lhs, rhs, "addtmp");
    }, "int"}},
    {{"float", OPER_SUB, "float"}, {[](llvm::Value* lhs, llvm::Value* rhs) -> llvm::Value* {
        return llvm_irbuilder->CreateFSub(lhs, rhs, "addtmp");
    }, "float"}},

    //LHS * RHS
    {{"float", OPER_MULT, "int"}, {[](llvm::Value* lhs, llvm::Value* rhs) -> llvm::Value* {
        return llvm_irbuilder->CreateFMul(lhs, llvm_irbuilder->CreateSIToFP(rhs, type_map["float"](*llvm_context)), "addtmp");
    }, "float"}},
    {{"int", OPER_MULT, "float"}, {[](llvm::Value* lhs, llvm::Value* rhs) -> llvm::Value* {
        return llvm_irbuilder->CreateFMul(llvm_irbuilder->CreateSIToFP(lhs, type_map["float"](*llvm_context)), rhs, "addtmp");
    }, "float"}},
    {{"int", OPER_MULT, "int"}, {[](llvm::Value* lhs, llvm::Value* rhs) -> llvm::Value* {
        return llvm_irbuilder->CreateMul(lhs, rhs, "addtmp");
    }, "int"}},
    {{"float", OPER_MULT, "float"}, {[](llvm::Value* lhs, llvm::Value* rhs) -> llvm::Value* {
        return llvm_irbuilder->CreateFMul(lhs, rhs, "addtmp");
    }, "float"}},

    //LHS / RHS
    {{"float", OPER_DIV, "int"}, {[](llvm::Value* lhs, llvm::Value* rhs) -> llvm::Value* {
        return llvm_irbuilder->CreateFDiv(lhs, llvm_irbuilder->CreateSIToFP(rhs, type_map["float"](*llvm_context)), "addtmp");
    }, "float"}},
    {{"int", OPER_DIV, "float"}, {[](llvm::Value* lhs, llvm::Value* rhs) -> llvm::Value* {
        return llvm_irbuilder->CreateFDiv(llvm_irbuilder->CreateSIToFP(lhs, type_map["float"](*llvm_context)), rhs, "addtmp");
    }, "float"}},
    {{"int", OPER_DIV, "int"}, {[](llvm::Value* lhs, llvm::Value* rhs) -> llvm::Value* {
        return llvm_irbuilder->CreateSDiv(lhs, rhs, "addtmp");
    }, "int"}},
    {{"float", OPER_DIV, "float"}, {[](llvm::Value* lhs, llvm::Value* rhs) -> llvm::Value* {
        return llvm_irbuilder->CreateFDiv(lhs, rhs, "addtmp");
    }, "float"}},

    //LHS % RHS TODO:
    /*
    {{"float", OPER_MOD, "int"}, {[](llvm::Value* lhs, llvm::Value* rhs) -> llvm::Value* {
        return llvm_irbuilder->CreateFDiv(lhs, llvm_irbuilder->CreateSIToFP(rhs, type_map["float"](*llvm_context)), "addtmp");
    }, "float"}},
    {{"int", OPER_MOD, "float"}, {[](llvm::Value* lhs, llvm::Value* rhs) -> llvm::Value* {
        return llvm_irbuilder->CreateFDiv(llvm_irbuilder->CreateSIToFP(lhs, type_map["float"](*llvm_context)), rhs, "addtmp");
    }, "float"}},
    {{"int", OPER_MOD, "int"}, {[](llvm::Value* lhs, llvm::Value* rhs) -> llvm::Value* {
        //return llvm_irbuilder->CreateSDiv(lhs, rhs, "addtmp");
        //return llvm::expandRemainderUpTo64Bits()
    }, "int"}},
    {{"float", OPER_MOD, "float"}, {[](llvm::Value* lhs, llvm::Value* rhs) -> llvm::Value* {
        return llvm_irbuilder->CreateFDiv(lhs, rhs, "addtmp");
    }, "float"}}
    */

   {{"float", OPER_ASSIGN, "int"}, {[](llvm::Value* lhs, llvm::Value* rhs) -> llvm::Value* {
        return llvm_irbuilder->CreateStore(llvm_irbuilder->CreateSIToFP(rhs, type_map["float"](*llvm_context)), lhs);
    }, "float"}},
    {{"int", OPER_ASSIGN, "float"}, {[](llvm::Value* lhs, llvm::Value* rhs) -> llvm::Value* {
        return llvm_irbuilder->CreateStore(llvm_irbuilder->CreateFPToSI(rhs, type_map["int"](*llvm_context)), lhs);
    }, "int"}},
    {{"int", OPER_ASSIGN, "int"}, {[](llvm::Value* lhs, llvm::Value* rhs) -> llvm::Value* {
        return llvm_irbuilder->CreateStore(rhs, lhs);
    }, "int"}},
    {{"float", OPER_ASSIGN, "float"}, {[](llvm::Value* lhs, llvm::Value* rhs) -> llvm::Value* {
        return llvm_irbuilder->CreateStore(rhs, lhs);
    }, "float"}},
    {{"string", OPER_ASSIGN, "string"}, {[](llvm::Value* lhs, llvm::Value* rhs)-> llvm::Value* {
        return llvm_irbuilder->CreateStore(rhs, lhs);
    }, "string"}}
};

unary_oper_reduce_func_map_type unary_oper_reduce_func_map = {
    {{U_OPER_NEG, "float"}, {[](llvm::Value* target) -> llvm::Value* {
        return llvm_irbuilder->CreateFNeg(target);
    }, "float"}},
    {{U_OPER_NEG, "int"}, {[](llvm::Value* target) -> llvm::Value* {
        return llvm_irbuilder->CreateNeg(target);
    }, "int"}}
};

std::map<std::string, std::string> symbol_type_map = {};
std::map<std::string, llvm::Value*> value_map = {};
std::map<std::string, llvm::Value*> value_map_buffer = {};
std::map<std::string, std::vector<std::string>> func_args_type_map = {};
std::vector<std::string> global_symbols = {};
std::vector<std::string> defined_symbols = {};
std::vector<std::string> declared_symbols = {};