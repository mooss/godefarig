#ifndef MOOSS_DEBUG_H
#define MOOSS_DEBUG_H

#include <iostream>

#ifdef DEBUG_MODE
#define DEBUG(x) do { std::cerr << "DBG:" << x << std::endl; } while (0)
#else
#define DEBUG(x) do {} while (0)
#endif

#endif//MOOSS_DEBUG_H
