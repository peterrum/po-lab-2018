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
  // Binary Arithmetic Operations
  shared_ptr<AbstractDomain> add(unsigned numBits, AbstractDomain &other,
    bool nuw=false, bool nsw=false);
  shared_ptr<AbstractDomain> sub(unsigned numBits, AbstractDomain &other,
    bool nuw=false, bool nsw=false);
  shared_ptr<AbstractDomain> mul(unsigned numBits, AbstractDomain &other,
    bool nuw=false, bool nsw=false);
  shared_ptr<AbstractDomain> udiv(unsigned numBits, AbstractDomain &other,
    bool nuw=false, bool nsw=false);
  shared_ptr<AbstractDomain> sdiv(unsigned numBits, AbstractDomain &other,
    bool nuw=false, bool nsw=false);
  shared_ptr<AbstractDomain> urem(unsigned numBits, AbstractDomain &other,
    bool nuw=false, bool nsw=false);
  shared_ptr<AbstractDomain> srem(unsigned numBits, AbstractDomain &other,
    bool nuw=false, bool nsw=false);

  // Binary Bitwise Operations
  shared_ptr<AbstractDomain> shl(unsigned numBits, AbstractDomain &other,
    bool nuw=false, bool nsw=false);
  shared_ptr<AbstractDomain> shlr(unsigned numBits, AbstractDomain &other,
    bool nuw=false, bool nsw=false);
  shared_ptr<AbstractDomain> ashr(unsigned numBits, AbstractDomain &other,
    bool nuw=false, bool nsw=false);
  shared_ptr<AbstractDomain> and_(unsigned numBits, AbstractDomain &other,
    bool nuw=false, bool nsw=false);
  shared_ptr<AbstractDomain> or_(unsigned numBits, AbstractDomain &other,
    bool nuw=false, bool nsw=false);
  shared_ptr<AbstractDomain> xor_(unsigned numBits, AbstractDomain &other,
    bool nuw=false, bool nsw=false);

  // Other operations
  std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>
    icmp(CmpInst::Predicate pred, unsigned numBits, AbstractDomain &other);


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