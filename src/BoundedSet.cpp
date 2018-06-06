#include "AbstractDomain.h"
#include "llvm/ADT/APInt.h"
#include <iterator>
#include <memory>
#include <set>

namespace pcpo {

using llvm::APInt;
using std::unique_ptr;

struct Comparator {
  bool operator()(const APInt &left, const APInt &right) {
    return left.ule(right);
  }
};

class BoundedSet : AbstractDomain {
private:
  std::set<APInt, Comparator> values;

public:
  unique_ptr<AbstractDomain> add(AbstractDomain &other);
  unique_ptr<AbstractDomain> subtract(AbstractDomain &other);
  unique_ptr<AbstractDomain> multiply(AbstractDomain &other);
  unique_ptr<AbstractDomain> unaryMinus();
  unique_ptr<AbstractDomain> increment();
  unique_ptr<AbstractDomain> decrement();
  unique_ptr<AbstractDomain> leastUpperBound(AbstractDomain &other);
  bool lessOrEqual(AbstractDomain &other);

  BoundedSet(std::set<APInt, Comparator> values);
};

BoundedSet::BoundedSet(std::set<APInt, Comparator> vals) : values{vals} {}

unique_ptr<AbstractDomain> BoundedSet::add(AbstractDomain &other) {
  if (BoundedSet *otherB = dynamic_cast<BoundedSet *>(&other)) {
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
  return nullptr;
}
bool BoundedSet::lessOrEqual(AbstractDomain &other) { return false; }

} // namespace pcpo
