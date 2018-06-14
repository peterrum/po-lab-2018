#ifndef COMPOSITE_DOMAIN_H_
#define COMPOSITE_DOMAIN_H_
#include "AbstractDomain.h"
#include "BoundedSet.h"
#include "StridedInterval.h"
#include "llvm/ADT/APInt.h"
#include <functional>
#include <iostream>
#include <set>
namespace pcpo {
using llvm::APInt;
using std::function;
using std::shared_ptr;

enum DelegateType { stridedInterval, boundedSet };

class CompositeDomain : public AbstractDomain {
private:
  DelegateType delegateType;
  shared_ptr<AbstractDomain> delegate;
  DelegateType getDelegateType();
  shared_ptr<AbstractDomain> computeOperation(
      AbstractDomain &other,
      function<shared_ptr<AbstractDomain>(AbstractDomain &, AbstractDomain &)>
          op);

public:
  // Constructors
  CompositeDomain(APInt value);
  CompositeDomain(bool isTop);
  CompositeDomain::CompositeDomain(const CompositeDomain &old);

  // Binary Arithmetic Operations
  shared_ptr<AbstractDomain> add(unsigned numBits, AbstractDomain &other,
                                 bool nuw, bool nsw);
  shared_ptr<AbstractDomain> sub(unsigned numBits, AbstractDomain &other,
                                 bool nuw, bool nsw);
  shared_ptr<AbstractDomain> mul(unsigned numBits, AbstractDomain &other,
                                 bool nuw, bool nsw);
  shared_ptr<AbstractDomain> udiv(unsigned numBits, AbstractDomain &other);
  shared_ptr<AbstractDomain> sdiv(unsigned numBits, AbstractDomain &other);
  shared_ptr<AbstractDomain> urem(unsigned numBits, AbstractDomain &other);
  shared_ptr<AbstractDomain> srem(unsigned numBits, AbstractDomain &other);

  // Binary Bitwise Operations
  shared_ptr<AbstractDomain> shl(unsigned numBits, AbstractDomain &other,
                                 bool nuw, bool nsw);
  shared_ptr<AbstractDomain> lshr(unsigned numBits, AbstractDomain &other);
  shared_ptr<AbstractDomain> ashr(unsigned numBits, AbstractDomain &other);
  shared_ptr<AbstractDomain> and_(unsigned numBits, AbstractDomain &other);
  shared_ptr<AbstractDomain> or_(unsigned numBits, AbstractDomain &other);
  shared_ptr<AbstractDomain> xor_(unsigned numBits, AbstractDomain &other);

  // Other operations
  std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>
  icmp(CmpInst::Predicate pred, unsigned numBits, AbstractDomain &other);

  // |gamma(this)|
  size_t size() const;

  shared_ptr<AbstractDomain> leastUpperBound(AbstractDomain &other);
  bool lessOrEqual(AbstractDomain &other);

  bool operator==(const BoundedSet &other);

  friend std::ostream &operator<<(std::ostream &os, const BoundedSet &bs);
  bool isTop() const;
  bool isBottom() const;
  void printOut() const;

  virtual llvm::raw_ostream &print(llvm::raw_ostream &os);
};
} // namespace pcpo
#endif
