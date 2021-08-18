#include <iostream>
#include <boost/program_options.hpp>

#include "cli.hpp"
#include "err_codes.h"
#include "log.hpp"

namespace po = boost::program_options;

argparse_ret::argparse_ret():
output_filename("out.ll"), repl_mode(false), no_output(false), suppress_warnings(false), verbose(false){
    //
}

argparse_ret::~argparse_ret(){
    //
}

argparse_ret get_args(int argc, char* argv[]){
    argparse_ret retval;
    //options
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "show this help message")
        ("verbose,v", "increase verbosity")
        ("repl,r", "run the REPL")
        ("no-output", "print LLVM IR to the console")
        ("suppress-warnings", "Don't show any warnings")
        ("output,o", po::value<std::string>(), "output file path")
        ("sources", po::value<std::vector<std::string>>()->multitoken(), "source file paths")
    ;

    //positional options
    po::positional_options_description pos_desc;
    pos_desc.add("sources", -1);

    //parse the cmd args, create the arg map
    po::variables_map vm;
    try{
        po::store(po::command_line_parser(argc, argv).options(desc).positional(pos_desc).run(), vm);
        po::notify(vm);
    } catch(po::error& e){
        stdlog.err() << "Invalid command line arguments - " << e.what() << std::endl;
        throw CLI_INVALID_ARGS_ERR;
    }

    //verify args existence
    if(vm.size() == 0){
        stdlog.err() << "Invalid arguments" << std::endl;
        std::cout << desc << std::endl;
        throw CLI_INVALID_ARGS_ERR;
    }

    //show help msg
    if (vm.count("help")){
        std::cout << desc << std::endl;
        throw CLI_INVALID_ARGS_ERR;
    }

    if(vm.count("repl")) retval.repl_mode = true;
    if(vm.count("verbose")) retval.verbose = true;
    if(vm.count("no-output")) retval.no_output = true;
    if(vm.count("suppress-warnings")) retval.suppress_warnings = true;

    if(vm.count("sources")){
        retval.source_filenames = vm["sources"].as<std::vector<std::string>>();
        retval.repl_mode = false;
    }
    else if(!vm.count("repl")){
        stdlog.err() << "At least one source file must be given" << std::endl;
        throw CLI_INVALID_ARGS_ERR;
    }

    if(vm.count("output")){
        retval.output_filename = vm["output"].as<std::string>();
    }

    return retval;
}
