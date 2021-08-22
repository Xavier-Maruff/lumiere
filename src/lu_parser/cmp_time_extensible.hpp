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
#include <vector>
#include <set>
#include <iostream>

#include "ast_enums.h"

//node id counter
extern size_t current_node_id;

//node source location information
class error_loc_info {
    public:

    int filename_index;
    int begin_line;
    int begin_column;
    int end_line;
    int end_column;

    //stores the source filenames
    static std::vector<std::string> source_filenames;
    //returns the index of a filename in the source_filenames vect, if not found adds filename to vect, returns new index
    static int get_filename_index(const std::string filename);

    error_loc_info(const std::string filename_, int begin_line_, int begin_column_, int end_line_, int end_column_);
    error_loc_info();
    ~error_loc_info();
    
    //returns the filename referenced to by the filename_index
    std::string filename();

    friend std::ostream& operator<<(std::ostream& os, error_loc_info& err_info);
};

//node location map
extern std::map<size_t, error_loc_info> node_id_source_info_map;

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
//type function map (typename -> function returning correct type)
typedef std::map<std::string, type_transform_func> type_map_type;
//actual compile time type map
extern type_map_type type_map;

//symbol types (name -> typename)
extern std::map<std::string, std::string> symbol_type_map;
//function arg types (name -> {arg typenames})
extern std::map<std::string, std::vector<std::string>> func_args_type_map;


#define TYPE_MAP_ELEM(type_name, func_body) \
{type_name, [](llvm::LLVMContext& ctx) -> llvm::Type* func_body}

//primitive type constructors
extern std::map<std::string, std::function<llvm::Value*(llvm::Value*)>> primitive_type_constructors;


//named values
extern std::map<std::string, llvm::Value*> value_map;
extern std::map<std::string, llvm::Value*> value_map_buffer;

//globals
extern std::set<std::string> global_symbols;
extern std::map<std::string, llvm::Value*> global_symbol_map;
extern std::map<std::string, std::string> global_symbol_type_map;

//declarations and definitions
extern std::set<std::string> defined_symbols;
extern std::set<std::string> declared_symbols;

//buffers for dec and def
extern std::set<std::string> defined_symbols_buffer;
extern std::set<std::string> declared_symbols_buffer;

void store_tables_to_buffer();
void load_tables_from_buffer();

//to be called when traversing scopes at ast codegen time
void store_symbol_type_map_to_buffer();
void load_symbol_type_map_from_buffer();

#endif