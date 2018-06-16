#ifndef BOUNDED_SET_H_
#define BOUNDED_SET_H_
#include "AbstractDomain.h"
#include "Util.h"
#include <functional>
#include <iostream>
#include <set>
namespace pcpo {
using llvm::APInt;

const int SET_LIMIT = 40;

// struct Comparator {
//   bool operator()(const APInt &left, const APInt &right) const {
//     return left.ult(right);
//   }
// };

class BoundedSet : public AbstractDomain {
private:
  unsigned bitWidth;
  std::set<APInt, Comparator> values;
  bool top{false};
  shared_ptr<AbstractDomain>
  compute(AbstractDomain &other,
          std::function<BoundedSet(const APInt &, const APInt &)> op);
  std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>
  subsetsForPredicate(
      AbstractDomain &other,
      std::function<bool(const APInt &, const APInt &)> comparision,
      CmpInst::Predicate pred);

  shared_ptr<AbstractDomain> createBoundedSetPointer(unsigned bitWidth, bool top);
  std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>
  createBoundedSetPointerPair(unsigned bitWidth, bool firstTop, bool secondTop);
  bool containsValue(unsigned numBits, uint64_t n) const;
  void warnIfContainsZero(unsigned numBits);
  void warnIfDivisionOverflowPossible(unsigned numBits, BoundedSet &other);

public:
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

  bool contains(APInt &value) const;

  shared_ptr<AbstractDomain> leastUpperBound(AbstractDomain &other);
  bool lessOrEqual(AbstractDomain &other);

  bool operator==(const BoundedSet &other);
  bool operator!=(const BoundedSet &other);

  BoundedSet(unsigned ,std::set<APInt, Comparator> values);
  explicit BoundedSet(APInt value);

  BoundedSet(const BoundedSet & b);
  explicit BoundedSet(unsigned numBits, bool isTop);
  BoundedSet(unsigned numBits, std::initializer_list<APInt> vals);
  BoundedSet(unsigned numBits, std::initializer_list<uint64_t> vals);
  unsigned getBitWidth() const;
  std::set<APInt, Comparator> getValues() const;
  bool isTop() const;
  bool isBottom() const;

  APInt getValueAt(uint64_t i) const;
  APInt getUMin() const;
  APSInt getSMin() const;
  APInt getUMax() const;
  APSInt getSMax() const;

  void printOut() const;

  virtual llvm::raw_ostream &print(llvm::raw_ostream &os);

  static shared_ptr<AbstractDomain> create_bottom(unsigned bitWidth) {
    return std::shared_ptr<AbstractDomain>(new BoundedSet(bitWidth, false));
  }

  static shared_ptr<AbstractDomain> create_top(unsigned bitWidth) {
    return std::shared_ptr<AbstractDomain>(new BoundedSet(bitWidth, true));
  }
};
} // namespace pcpo
#endif
