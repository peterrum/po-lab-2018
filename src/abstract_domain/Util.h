#ifndef AD_UTIL_H_
#define AD_UTIL_H_

#include "llvm/ADT/APInt.h"

namespace pcpo {
using llvm::APInt;

struct Comparator {
  bool operator()(const APInt &left, const APInt &right) const {
    return left.ult(right);
  }
};

}

#endif