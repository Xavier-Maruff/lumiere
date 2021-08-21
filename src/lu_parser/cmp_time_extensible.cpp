#include "cmp_time_extensible.hpp"
#include "llvm_inst.hpp"
#include "log.hpp"

#include <llvm/Transforms/Utils/IntegerDivision.h>

//binary expression boilerplate
llvm_reduce_value_func::llvm_reduce_value_func(reduce_value_func reduce_func_, std::string return_type_):
reduce_func(reduce_func_), return_type(return_type_){}

llvm::Value* llvm_reduce_value_func::operator()(llvm::Value* lhs, llvm::Value* rhs){return reduce_func(lhs, rhs);}

//unary expression boilerplate
llvm_reduce_unary_value_func::llvm_reduce_unary_value_func(reduce_unary_value_func reduce_func_, std::string return_type_):
reduce_func(reduce_func_), return_type(return_type_){}

llvm::Value* llvm_reduce_unary_value_func::operator()(llvm::Value* target){return reduce_func(target);}

//converts lumiere typenames to llvm types
type_map_type type_map = {
    TYPE_MAP_ELEM("float", {return llvm::Type::getDoubleTy(ctx);}),
    TYPE_MAP_ELEM("int", {return llvm::Type::getInt64Ty(ctx);}),
    TYPE_MAP_ELEM("string", {return llvm::Type::getInt8PtrTy(ctx);}),
    TYPE_MAP_ELEM("void", {return llvm::Type::getVoidTy(ctx);}),
    TYPE_MAP_ELEM("int32", {return llvm::Type::getInt32Ty(ctx);})
};


//returns a function that generates the IR for a binary expression - this should allow for easy operator overloading
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

//returns a function to generate IR for unary expression
unary_oper_reduce_func_map_type unary_oper_reduce_func_map = {
    {{U_OPER_NEG, "float"}, {[](llvm::Value* target) -> llvm::Value* {
        return llvm_irbuilder->CreateFNeg(target);
    }, "float"}},
    {{U_OPER_NEG, "int"}, {[](llvm::Value* target) -> llvm::Value* {
        return llvm_irbuilder->CreateNeg(target);
    }, "int"}}
};

//stores the lumiere type of a symbol
std::map<std::string, std::string> symbol_type_map = {};
std::map<std::string, std::string> symbol_type_map_buffer = {};

//maps symbol names to llvm::Value*
std::map<std::string, llvm::Value*> value_map = {};
std::map<std::string, llvm::Value*> value_map_buffer = {};

//maps function names to vector of argument types
std::map<std::string, std::vector<std::string>> func_args_type_map = {};
std::map<std::string, std::vector<std::string>> func_args_type_map_buffer = {};

//global symbols is not affected by scope
std::set<std::string> global_symbols = {};
std::map<std::string, llvm::Value*> global_symbol_map = {};
std::map<std::string, std::string> global_symbol_type_map = {};

//symbols that have been declared and defined
std::set<std::string> defined_symbols = {};
std::set<std::string> defined_symbols_buffer = {}; 

//symbols that have been declared
std::set<std::string> declared_symbols = {};
std::set<std::string> declared_symbols_buffer = {};


//called when entering a deeper scope, so new variables within scope are removed
void store_tables_to_buffer(){
    symbol_type_map_buffer = symbol_type_map;
    value_map_buffer = value_map;
    func_args_type_map_buffer = func_args_type_map;
    defined_symbols_buffer = defined_symbols;
    declared_symbols_buffer = declared_symbols;
}

//sister function to store_tables_to_buffer, called going to a shallower scope
void load_tables_from_buffer(){
    symbol_type_map = symbol_type_map_buffer;
    symbol_type_map.insert(global_symbol_type_map.begin(), global_symbol_type_map.end());

    value_map = value_map_buffer;
    value_map.insert(global_symbol_map.begin(), global_symbol_map.end());

    func_args_type_map = func_args_type_map_buffer;

    defined_symbols = defined_symbols_buffer;
    declared_symbols = declared_symbols_buffer;

    symbol_type_map_buffer.clear();
    value_map_buffer.clear();
    func_args_type_map_buffer.clear();
    defined_symbols_buffer.clear();
    declared_symbols_buffer.clear();
}


void store_symbol_type_map_to_buffer(){
    symbol_type_map_buffer = symbol_type_map;
}


void load_symbol_type_map_from_buffer(){
    symbol_type_map = symbol_type_map_buffer;
    symbol_type_map.insert(global_symbol_type_map.begin(), global_symbol_type_map.end());
    symbol_type_map_buffer.clear();
}