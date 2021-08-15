#include "log.hpp"
#include "colours.h"

std::ostream& logger::operator()(){
    std::clog << ANSI_CYAN << "INFO: " << ANSI_RESET;
    return std::clog;
}

std::ostream& logger::err(){
    std::cerr << ANSI_RED << "ERROR: " << ANSI_RESET;
    return std::cerr;
}

logger::logger(){
    //
}

logger::~logger(){
    //
}

logger stdlog;