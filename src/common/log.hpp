#ifndef LUMIERE_COM_LOG
#define LUMIERE_COM_LOG

#include "colours.h"
#include <iostream>
#include <sstream>

#ifdef DEBUG
#define DEBUG_LOG(MSG) std::cout << ANSI_GREEN << "DEBUG: " << ANSI_RESET << (MSG) << std::endl
#define DEBUG_LOGL(LOC, MSG) std::cout << ANSI_CYAN "(" << (LOC) << ")\t" << ANSI_RESET << (MSG) << std::endl
#else
#define DEBUG_LOG(MSG) (void)0
#define DEBUG_LOGL(LOC, MSG) (void)0
#endif

#define ERR_LOG(MSG) std::cerr << ANSI_RED << "ERROR: " << ANSI_RESET << (MSG) << std::endl;
#define INFO_LOG(MSG) std::cout << ANSI_CYAN << "INFO: " << ANSI_RESET << (MSG) << std::endl;

typedef struct logger {

    bool is_verbose = false;
    bool suppress_warnings = false;

    std::ostream& operator()(bool override_verbose = false);
    std::ostream& err();
    std::ostream& warn();

    logger();
    ~logger();

    private:
    std::stringstream dummy;
    
} logger;

extern logger stdlog;

#endif