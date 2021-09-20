#ifndef LUMIERE_PARSER_DRIVER
#define LUMIERE_PARSER_DRIVER

/**
 * @file driver.hpp
 * @author Xavier Maruff (xavier.maruff@outlook.com)
 * @brief Contains the lumiere driver declaration
 * @version 0.1
 * @date 2021-08-23
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <string>

#include "bison.hpp"

#define YY_DECL yy::parser::symbol_type yylex(parser_driver& lu_driver)
YY_DECL;

/**
 * @brief Drives the lumiere parser
 * 
 */
class parser_driver{
    public:
    /**
     * @brief Current location in the source file
     * 
     */
    yy::location loc;
    /**
     * @brief Source file filename
     * 
     */
    std::string filename;
    /**
     * @brief Logging verbosity
     * 
     */
    bool is_verbose;
    /**
     * @brief If true, input will be read from the stdin
     * 
     */
    bool is_repl;

    parser_driver(bool is_repl_ = false, bool is_verbose_ = false);
    virtual ~parser_driver();

    /**
     * @brief Load a file to parse
     * 
     * @param filename_ The path to the file to parse
     */
    void load_file(std::string filename_);
   
    /**
     * @brief Parse the loaded file
     * 
     */
    void parse();
    
    /**
     * @brief Get the LLVM IR as a string
     * 
     * @return std::string The IR
     */
    std::string get_ir();
    
    /**
     * @brief Write the LLVM IR to the output file
     * 
     * @param output_filename The path to the output file
     */
    void write_ir(std::string output_filename);;

    private:
    /**
     * @brief Initialize the llvm globals
     * 
     */
    void init_llvm(std::string module_name);

};

#endif