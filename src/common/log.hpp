#ifndef LUMIERE_COM_LOG
#define LUMIERE_COM_LOG

/**
 * @file log.hpp
 * @author Xavier Maruff (xavier.maruff@outlook.com)
 * @brief Lumiere logger declarations
 * @version 0.1
 * @date 2021-08-22
 * 
 * @copyright Copyright (c) 2021
 * 
 */

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

/**
 * @brief Lumiere logger
 * 
 */
typedef struct logger {

    bool is_verbose = false;
    bool suppress_warnings = false;

    /**
     * @brief info level logging
     * 
     * @param override_verbose Overrides non-verbose mode to direct info level logging to stdout
     * @return std::ostream& stdout
     */
    std::ostream& operator()(bool override_verbose = false);

    /**
     * @brief error logging
     * 
     * @return std::ostream& stderr
     */
    std::ostream& err();

    /**
     * @brief warning logging
     * 
     * @return std::ostream& stdout
     */
    std::ostream& warn();

    logger();
    ~logger();

    private:
    /**
     * @brief Dummy stream that logs are diverted to depending on verbosity
     * 
     */
    std::stringstream dummy;
    
} logger;

/**
 * @brief Logger instance
 * 
 */
extern logger stdlog;

#endif