#ifndef BOUNDED_SET_H_
#define BOUNDED_SET_H_
#include "AbstractDomain.h"
#include "llvm/ADT/APInt.h"
#include <functional>
#include <set>
namespace pcpo {
using llvm::APInt;

const int SET_LIMIT = 10;

struct Comparator {
  bool operator()(const APInt &left, const APInt &right) {
    return left.ule(right);
  }
};

class BoundedSet : public AbstractDomain {
private:
  std::set<APInt, Comparator> values;
  bool top{false};
  shared_ptr<AbstractDomain> compute(AbstractDomain &other,
                                     std::function<APInt(APInt, APInt)> op);

public:
  shared_ptr<AbstractDomain> add(AbstractDomain &other);
  shared_ptr<AbstractDomain> subtract(AbstractDomain &other);
  shared_ptr<AbstractDomain> multiply(AbstractDomain &other);
  shared_ptr<AbstractDomain> unaryMinus();
  shared_ptr<AbstractDomain> increment();
  shared_ptr<AbstractDomain> decrement();
  shared_ptr<AbstractDomain> leastUpperBound(AbstractDomain &other);
  bool lessOrEqual(AbstractDomain &other);

  BoundedSet(std::set<APInt, Comparator> values);
  BoundedSet(APInt value);
  BoundedSet(bool isTop);
  bool isTop();
  void printOut();
};
} // namespace pcpo
#endif