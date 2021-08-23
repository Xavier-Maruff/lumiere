#ifndef LUMIERE_PARSER_CMP_TIME_EXTENSIBLE
#define LUMIERE_PARSER_CMP_TIME_EXTENSIBLE

/**
 * @file cmp_time_extensible.hpp
 * @author Xavier Maruff (xavier.maruff@outlook.com)
 * @brief Contains extern declarations of structures containing lumiere compile time info, i.e. type info, symbol tables, etc.
 * @version 0.1
 * @date 2021-08-23
 * 
 * @copyright Copyright (c) 2021
 * 
 */

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

/**
 * @brief Node id counter
 * 
 */
extern size_t current_node_id;

/**
 * @brief Node location in the source file
 * 
 */
class error_loc_info {
    public:

    int filename_index;
    int begin_line;
    int begin_column;
    int end_line;
    int end_column;

    /**
     * @brief The source file filenames
     * 
     */
    static std::vector<std::string> source_filenames;

    /**
     * @brief Get the index of a filename in the source_filenames vector, or insert into the source_filenames vector if not present
     * 
     * @param filename The filename to get the index of
     * @return int The index of the filename
     */
    static int get_filename_index(const std::string filename);

    error_loc_info(const std::string filename_, int begin_line_, int begin_column_, int end_line_, int end_column_);
    error_loc_info();
    ~error_loc_info();
    
    /**
     * @brief Get the filename corresponding to the filename index
     * 
     * @return std::string Filename, or "unknown" if not found
     */
    std::string filename();

    /**
     * @brief Formats the error_loc_info into an ostream
     * 
     * @param os 
     * @param err_info 
     * @return std::ostream& output stream
     */
    friend std::ostream& operator<<(std::ostream& os, error_loc_info& err_info);
};

/**
 * @brief Maps node IDs to error_loc_info instances
 * 
 */
extern std::map<size_t, error_loc_info> node_id_source_info_map;

/**
 * @brief Function to reduce a binary expression to a single llvm::Value*
 * 
 */
typedef std::function<llvm::Value*(llvm::Value*, llvm::Value*)> reduce_binary_value_func;

/**
 * @brief Function to reduce a unary expression to a single llvm::Value*
 * 
 */
typedef std::function<llvm::Value*(llvm::Value*)> reduce_unary_value_func;

/**
 * @brief A wrapper over reduce_binary_value_func which includes the return type
 * 
 */
typedef struct reduce_binary_value {
    /**
     * @brief The functions it is a wrapper over
     * 
     */
    reduce_binary_value_func reduce_func;

    /**
     * @brief The function return type
     * 
     */
    std::string return_type;

    /**
     * @brief Runs the reduce_binary_value_func it holds
     * 
     * @param lhs The LHS value*
     * @param rhs The RHS value*
     * @return llvm::Value* The value* resultant from the binary operation
     */
    llvm::Value* operator()(llvm::Value* lhs, llvm::Value* rhs);
    reduce_binary_value(reduce_binary_value_func reduce_func_, std::string return_type_);

} reduce_binary_value;

/**
 * @brief A wrapper over reduce_unary_value_func which includes the return type
 * 
 */
typedef struct reduce_unary_value {
    /**
     * @brief The function it is a wrapper over
     * 
     */
    reduce_unary_value_func reduce_func;
    /**
     * @brief The function return type
     * 
     */
    std::string return_type;

    /**
     * @brief Runs the reduce_unary_value_func it holds
     * 
     * @return llvm::Value* 
     */
    llvm::Value* operator()(llvm::Value*);
    reduce_unary_value(reduce_unary_value_func reduce_func_, std::string return_type_);

} reduce_unary_value;

//redice value func + return type
//map binary expression type -> value pair reduce function

/**
 * @brief Map {lhs type, opcode, rhs type} to a binary operation wrapper
 * 
 */
typedef std::map<std::tuple<std::string, bin_oper, std::string>, reduce_binary_value> bin_oper_reduce_map_type;

/**
 * @brief Map {opcode, target type} to a unary expression wrapper
 * 
 */
typedef std::map<std::pair<unary_oper, std::string>, reduce_unary_value> unary_oper_reduce_map_type;

/**
 * @brief Extensible bin_oper_reduce_map instance, contains default binexpr operations
 * 
 */
extern bin_oper_reduce_map_type bin_oper_reduce_map;

/**
 * @brief Extensible unary_oper_reduce_map instance, contains default unary expr operations
 * 
 */
extern unary_oper_reduce_map_type unary_oper_reduce_map;

/**
 * @brief Returns a pointer to the llvm type
 * 
 */
typedef std::function<llvm::Type*(llvm::LLVMContext&)> type_transform_func;

/**
 * @brief Maps lumiere typenames to a function that returns a pointer to the LLVM type
 * 
 */
typedef std::map<std::string, type_transform_func> type_map_type;

/**
 * @brief Extensible type_map_type map instance
 * 
 */
extern type_map_type type_map;

/**
 * @brief Maps symbol names to lumiere types
 * 
 */
extern std::map<std::string, std::string> symbol_type_map;

/**
 * @brief Maps function names to a vector of function argument types
 * 
 */
extern std::map<std::string, std::vector<std::string>> func_args_type_map;


#define TYPE_MAP_ELEM(type_name, func_body) \
{type_name, [](llvm::LLVMContext& ctx) -> llvm::Type* func_body}

/**
 * @brief Not yet implemented, will be a list of type constructors
 * 
 */
extern std::map<std::string, std::function<llvm::Value*(llvm::Value*)>> primitive_type_constructors;


/**
 * @brief Named values
 * 
 */
extern std::map<std::string, llvm::Value*> value_map;
extern std::map<std::string, llvm::Value*> value_map_buffer;

/**
 * @brief Global symbol names
 * 
 */
extern std::set<std::string> global_symbols;

/**
 * @brief Global named values
 * 
 */
extern std::map<std::string, llvm::Value*> global_symbol_map;

/**
 * @brief Global named value type map
 * 
 */
extern std::map<std::string, std::string> global_symbol_type_map;

/**
 * @brief Set of symbols with a definition
 * 
 */
extern std::set<std::string> defined_symbols;

/**
 * @brief Set of symbols with a declaration
 * 
 */
extern std::set<std::string> declared_symbols;

//buffers for dec and def
extern std::set<std::string> defined_symbols_buffer;
extern std::set<std::string> declared_symbols_buffer;

/**
 * @brief Store symbol tables to their respective buffers, used when enetering a deeper scope
 * 
 */
void store_tables_to_buffer();

/**
 * @brief Load symbol tables back from their respective buffers, used when exiting a deeper scope
 * 
 */
void load_tables_from_buffer();

/**
 * @brief Store the symbol type map to its buffer
 * 
 */
void store_symbol_type_map_to_buffer();

/**
 * @brief Load the symbol type map from its buffer
 * 
 */
void load_symbol_type_map_from_buffer();

#endif