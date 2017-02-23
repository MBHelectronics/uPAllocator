#ifndef DEBUG_MSG_H_INCLUDED
#define DEBUG_MSG_H_INCLUDED

#include <iostream>

#ifdef DEBUG
#define DEBUG_MSG(str)             \
  do {                             \
    std::cout << str << std::endl; \
  } while (false)
#else
#define DEBUG_MSG(str) \
  do {                 \
  } while (false)
#endif

#endif  // DEBUG_MSG_H_INCLUDED
