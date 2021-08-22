#ifndef LUMIERE_CLI
#define LUMIERE_CLI

/**
 * @file cli.hpp
 * @author Xavier Maruff (xavier.maruff@outlook.com)
 * @brief CLI declarations
 * @version 0.1
 * @date 2021-08-22
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <string>
#include <vector>

/**
 * @brief Struct containing parsed command line options
 * 
 */
typedef struct argparse_ret {
    bool verbose;
    bool repl_mode;
    bool no_output;
    bool suppress_warnings;
    std::vector<std::string> source_filenames;
    std::string output_filename;

    argparse_ret();
    ~argparse_ret();

} argparse_ret;

/**
 * @brief Parse command line args
 * 
 * @param argc Number of args
 * @param argv Array of args
 * @return argparse_ret 
 */
argparse_ret get_args(int argc, char* argv[]);

#endif