#ifndef ECMAPP_DEBUG_
#define ECMAPP_DEBUG_

#ifdef DEBUG
#include "colours.h"
#include <iostream>
#define DEBUG_LOG(MSG) std::cout << ANSI_GREEN << "DEBUG: " << ANSI_RESET << (MSG) << std::endl
#else
#define DEBUG_LOG(MSG) (void)0
#endif

#endif