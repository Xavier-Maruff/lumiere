/**
 * @file log.cpp
 * @author Xavier Maruff (xavier.maruff@outlook.com)
 * @brief Logging definitions
 * @version 0.1
 * @date 2021-08-23
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "log.hpp"
#include "colours.h"

std::ostream& logger::operator()(bool override_verbose){
    if(!is_verbose && !override_verbose) {
        dummy.str(std::string());
        return dummy;
    }
    std::clog << ANSI_CYAN << "INFO: " << ANSI_RESET;
    return std::clog;
    
}

std::ostream& logger::err(){
    std::cerr << ANSI_RED << "ERROR: " << ANSI_RESET;
    return std::cerr;
}

std::ostream& logger::warn(){
    if(suppress_warnings) {
        dummy.str(std::string());
        return dummy;
    }
    std::cout << ANSI_YELLOW << "WARNING: " << ANSI_RESET;
    return std::cout;
}

logger::logger(){
    //
}

logger::~logger(){
    //
}

logger stdlog;