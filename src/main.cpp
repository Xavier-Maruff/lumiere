#include <iostream>

#include "driver.hpp"
#include "cli.hpp"
#include "err_codes.h"
#include "log.hpp"
#include "get_string.hpp"

parser_driver drv;

int main(int argc, char* argv[]){
    try{
        argparse_ret cli_args = get_args(argc, argv);

        drv.is_repl = cli_args.repl_mode;
        drv.is_verbose = cli_args.verbose;

        stdlog.is_verbose = cli_args.verbose;
        stdlog.suppress_warnings = cli_args.suppress_warnings;

        if(!drv.is_repl){
            for(std::string source_filename: cli_args.source_filenames){
                drv.load_file(source_filename);
                drv.parse();
                
                if(cli_args.no_output){
                    stdlog() << "LLVM IR " << source_filename << ":" << std::endl;
                    std::cout << drv.get_ir() << std::endl;
                }
                else drv.write_ir(cli_args.output_filename);
            }
        }
        else drv.parse();
    } catch(err_codes err_code){
        stdlog.err() << get_string_err_code(err_code) << std::endl;
        return err_code;
    }
    return EXIT_SUCCESS;
}