#ifndef LUMIERE_PARSER_CMP_TIME_EXTENSIBLE
#define LUMIERE_PARSER_CMP_TIME_EXTENSIBLE

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

#include <map>
#include <functional>

#include "ast_enums.h"
//reduce a pair of value pointers to one value pointer
typedef std::function<llvm::Value*(llvm::Value*, llvm::Value*)> reduce_value_func;
//reduce a single value pointer to a single value pointer
typedef std::function<llvm::Value*(llvm::Value*)> reduce_unary_value_func;

//wrapper over reduce_value_func with return type information
typedef struct llvm_reduce_value_func {
    reduce_value_func reduce_func;
    std::string return_type;

    llvm::Value* operator()(llvm::Value*, llvm::Value*);
    llvm_reduce_value_func(reduce_value_func reduce_func_, std::string return_type_);

} llvm_reduce_value_func;

//wrapper over reduce_unary_value_func with return type information
typedef struct llvm_reduce_unary_value_func {
    reduce_unary_value_func reduce_func;
    std::string return_type;

    llvm::Value* operator()(llvm::Value*);
    llvm_reduce_unary_value_func(reduce_unary_value_func reduce_func_, std::string return_type_);

} llvm_reduce_unary_value_func;

//redice value func + return type
//map binary expression type -> value pair reduce function
typedef std::map<std::tuple<std::string, bin_oper, std::string>, llvm_reduce_value_func> bin_oper_reduce_func_map_type;
//map unary expression type -> value reduce function
typedef std::map<std::pair<unary_oper, std::string>, llvm_reduce_unary_value_func> unary_oper_reduce_func_map_type;

//the actual binary operation function map
extern bin_oper_reduce_func_map_type bin_oper_reduce_func_map;
//acutal unary operation function map
extern unary_oper_reduce_func_map_type unary_oper_reduce_func_map;

//compile time types
typedef std::function<llvm::Type*(llvm::LLVMContext&)> type_transform_func;
typedef std::map<std::string, type_transform_func> type_map_type;
//actual compile time type map
extern type_map_type type_map;

extern std::map<std::string, std::string> symbol_type_map;
extern std::map<std::string, std::vector<std::string>> func_args_type_map;


#define TYPE_MAP_ELEM(type_name, func_body) \
{type_name, [](llvm::LLVMContext& ctx) -> llvm::Type* func_body}


//named values
extern std::map<std::string, llvm::Value*> value_map;
extern std::map<std::string, llvm::Value*> value_map_buffer;

//TODO: Adding mutable variables YOU LEFT OFF DOING THIS
//static llvm::AllocaIns

#endif