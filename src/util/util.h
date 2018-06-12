#ifndef UTIL
#define UTIL

#include "llvm/Support/raw_ostream.h"

/// settings

/// should debug output enabled?
//#define DEBUG

/// type of abstract domain
#define AD_TYPE BoundedSet

/// do not to touch anything beneath here (useful functions)

#ifdef DEBUG
#define DEBUG_OUTPUT(text) errs() << text << "\n"
#else
#define DEBUG_OUTPUT(text)
#endif

#define STD_OUTPUT(text) errs() << text << "\n"

#endif