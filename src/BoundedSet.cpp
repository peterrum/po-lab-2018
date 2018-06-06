#include "BoundedSet.h"
#include "AbstractDomain.h"
#include "llvm/ADT/APInt.h"
#include "llvm/Support/raw_os_ostream.h"
#include <iostream>
#include <iterator>
#include <memory>
#include <set>

namespace pcpo {

using namespace llvm;
using std::unique_ptr;

BoundedSet::BoundedSet(std::set<APInt, Comparator> vals) : values{vals} {}
BoundedSet::BoundedSet(APInt val) : values{} { values.insert(val); }

unique_ptr<AbstractDomain> BoundedSet::add(AbstractDomain &other) {
  if (BoundedSet *otherB = static_cast<BoundedSet *>(&other)) {
    std::set<APInt, Comparator> newValues{};
    for (auto &leftVal : values) {
      for (auto &rightVal : otherB->values) {
        APInt newVal{leftVal};
        newVal += rightVal;
        newValues.insert(newVal);
      }
    }
    unique_ptr<AbstractDomain> result{new BoundedSet(newValues)};
    return result;
  }
  return nullptr;
}
unique_ptr<AbstractDomain> BoundedSet::subtract(AbstractDomain &other) {
  return nullptr;
}
unique_ptr<AbstractDomain> BoundedSet::multiply(AbstractDomain &other) {
  return nullptr;
}
unique_ptr<AbstractDomain> BoundedSet::unaryMinus() { return nullptr; }
unique_ptr<AbstractDomain> BoundedSet::increment() { return nullptr; }
unique_ptr<AbstractDomain> BoundedSet::decrement() { return nullptr; }

unique_ptr<AbstractDomain> BoundedSet::leastUpperBound(AbstractDomain &other) {
  if (BoundedSet *otherB = static_cast<BoundedSet *>(&other)) {
    std::set<APInt, Comparator> result;
    for (auto &val : values) {
      result.insert(val);
    }
    for (auto &val : otherB->values) {
      result.insert(val);
    }
    unique_ptr<BoundedSet> res{new BoundedSet{result}};
    return res;
  }
  return nullptr;
}
bool BoundedSet::lessOrEqual(AbstractDomain &other) { return false; }

void BoundedSet::printOut() {
  //   errs() << "BoundedSet@" << this << std::endl;
  for (auto &val : values) {
    errs() << val.toString(10, false) << "\n";
  }
}
} // namespace pcpo