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
  bool operator()(const APInt &left, const APInt &right) const {
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
  shared_ptr<AbstractDomain> sub(AbstractDomain &other);
  shared_ptr<AbstractDomain> mul(AbstractDomain &other);
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