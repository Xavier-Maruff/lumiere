#ifndef LUMIERE_CLI
#define LUMIERE_CLI

#include <string>
#include <vector>

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

argparse_ret get_args(int argc, char* argv[]);

#endif