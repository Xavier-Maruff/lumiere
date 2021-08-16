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

std::ostream& logger::warn(){
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