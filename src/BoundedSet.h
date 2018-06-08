#ifndef BOUNDED_SET_H_
#define BOUNDED_SET_H_
#include "AbstractDomain.h"
#include "llvm/ADT/APInt.h"
#include <functional>
#include <set>
#include <iostream>
namespace pcpo {
using llvm::APInt;

const int SET_LIMIT = 10;

struct Comparator {
  bool operator()(const APInt &left, const APInt &right) const {
    return left.ult(right);
  }
};

class BoundedSet : public AbstractDomain {
private:
  std::set<APInt, Comparator> values;
  bool top{false};
  shared_ptr<AbstractDomain> compute(AbstractDomain &other,
                                     std::function<BoundedSet(const APInt&, const APInt&)> op);
  std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>
  subsetsForPredicate(unsigned numBits, AbstractDomain &other,
       std::function<bool(const APInt &, const APInt &)> op);

  shared_ptr<AbstractDomain> createBoundedSetPointer(bool top);
  std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>
    createBoundedSetPointerPair(bool firstTop, bool secondTop);

public:
  // Binary Arithmetic Operations
  shared_ptr<AbstractDomain> add(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw);
  shared_ptr<AbstractDomain> sub(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw);
  shared_ptr<AbstractDomain> mul(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw);
  shared_ptr<AbstractDomain> udiv(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw);
  shared_ptr<AbstractDomain> sdiv(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw);
  shared_ptr<AbstractDomain> urem(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw);
  shared_ptr<AbstractDomain> srem(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw);

  // Binary Bitwise Operations
  shared_ptr<AbstractDomain> shl(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw);
  shared_ptr<AbstractDomain> lshr(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw);
  shared_ptr<AbstractDomain> ashr(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw);
  shared_ptr<AbstractDomain> and_(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw);
  shared_ptr<AbstractDomain> or_(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw);
  shared_ptr<AbstractDomain> xor_(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw);

  // Other operations
  std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>
    icmp(CmpInst::Predicate pred, unsigned numBits, AbstractDomain &other);
  
  // |gamma(this)|
  size_t size();


  shared_ptr<AbstractDomain> leastUpperBound(AbstractDomain &other);
  bool lessOrEqual(AbstractDomain &other);


  bool operator==(const BoundedSet &other);

  BoundedSet(std::set<APInt, Comparator> values);
  explicit BoundedSet(APInt value);
  friend std::ostream& operator<< (std::ostream& os, const BoundedSet& bs);

  explicit BoundedSet(bool isTop);
  BoundedSet(std::initializer_list<APInt> vals);
  BoundedSet(unsigned numBits, std::initializer_list<uint64_t> vals);
  bool isTop();
  void printOut();
  
  static shared_ptr<AbstractDomain> create_bottom(){
      return std::shared_ptr<AbstractDomain> (new BoundedSet(false));
  }
  
  static shared_ptr<AbstractDomain> create_top(){
      return std::shared_ptr<AbstractDomain> (new BoundedSet(true));
  }
  
};
} // namespace pcpo
#endif