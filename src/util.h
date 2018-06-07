#ifndef UTIL
#define UTIL

#include "llvm/Support/raw_ostream.h"

//#define DEBUG

#ifdef DEBUG
    #define DEBUG_OUTPUT(text) errs() << text << "\n"
#else
    #define DEBUG_OUTPUT(text) 
#endif

#define STD_OUTPUT(text) errs() << text << "\n"

#endif