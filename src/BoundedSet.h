#ifndef BOUNDED_SET_H_
#define BOUNDED_SET_H_
#include "AbstractDomain.h"
#include "llvm/ADT/APInt.h"
#include <set>
namespace pcpo {
using llvm::APInt;

struct Comparator {
  bool operator()(const APInt &left, const APInt &right) {
    return left.ule(right);
  }
};

class BoundedSet : public AbstractDomain {
private:
  std::set<APInt, Comparator> values;
  bool isTop{false};

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
  BoundedSet(APInt value);

  void printOut();
};
} // namespace pcpo
#endif