#ifndef STRIDED_INTERVAL_H_
#define STRIDED_INTERVAL_H_
#include "AbstractDomain.h"
#include "Util.h"
#include "BoundedSet.h"
#include "llvm/ADT/APInt.h"
#include <functional>
#include <iostream>
#include <set>

namespace pcpo {
using llvm::APInt;

class StridedInterval : public AbstractDomain {

public:

  /// Constructors
  explicit StridedInterval(APInt value);
  explicit StridedInterval(bool isTop, unsigned bitWidth);
  StridedInterval();
  StridedInterval(APInt begin, APInt end, APInt stride);
  StridedInterval(unsigned numBits, std::initializer_list<uint64_t> vals);
  StridedInterval(unsigned bitWidth, uint64_t begin, uint64_t end, uint64_t stride);
  StridedInterval(BoundedSet &set);

  /// Copy constructor
  StridedInterval(const StridedInterval& other);
  /// Copy assignment
  StridedInterval& operator=(const StridedInterval& other) = default;

  /// Comparison Operators
  bool operator==(const StridedInterval &other);
  bool operator!=(const StridedInterval &other);

  /// Member functions
  unsigned getBitWidth() const;
  bool isTop() const;
  bool isBottom() const;
  bool contains(APInt &value) const;

  std::shared_ptr<AbstractDomain> normalize();
  bool isNormal();
  std::set<APInt, Comparator> gamma();

  /// |gamma(this)| ???
  size_t size() const;

  /// Member functions for API TODO
  APInt getValueAt(uint64_t i) const { return APInt(); }
  APInt getUMin() const { return APInt(); }
  APSInt getSMin() const { return APSInt(); }
  APInt getUMax() const { return APInt(); }
  APSInt getSMax() const { return APSInt(); }
  /// same as prev?
  APInt umax();
  APInt umin();
  APInt smax();
  APInt smin();

  /// Print
  friend std::ostream &operator<<(std::ostream &os, const StridedInterval &bs);
  virtual llvm::raw_ostream &print(llvm::raw_ostream &os);
  void printOut() const;

  /// Bottom SI
  static shared_ptr<AbstractDomain> create_bottom(unsigned bitWidth) {
    return std::shared_ptr<AbstractDomain>(new StridedInterval(false, bitWidth));
  }

  /// Top SI
  static shared_ptr<AbstractDomain> create_top(unsigned bitWidth) {
    return std::shared_ptr<AbstractDomain>(new StridedInterval(true, bitWidth));
  }

  /// Binary Arithmetic Operations
  shared_ptr<AbstractDomain> add(unsigned numBits, AbstractDomain &other, bool nuw, bool nsw);
  shared_ptr<AbstractDomain> sub(unsigned numBits, AbstractDomain &other, bool nuw, bool nsw);
  shared_ptr<AbstractDomain> mul(unsigned numBits, AbstractDomain &other, bool nuw, bool nsw);
  shared_ptr<AbstractDomain> udiv(unsigned numBits, AbstractDomain &other);
  shared_ptr<AbstractDomain> sdiv(unsigned numBits, AbstractDomain &other);
  shared_ptr<AbstractDomain> urem(unsigned numBits, AbstractDomain &other);
  shared_ptr<AbstractDomain> srem(unsigned numBits, AbstractDomain &other);

  /// Binary Bitwise Operations
  shared_ptr<AbstractDomain> shl(unsigned numBits, AbstractDomain &other, bool nuw, bool nsw);
  shared_ptr<AbstractDomain> lshr(unsigned numBits, AbstractDomain &other);
  shared_ptr<AbstractDomain> ashr(unsigned numBits, AbstractDomain &other);
  shared_ptr<AbstractDomain> and_(unsigned numBits, AbstractDomain &other);
  shared_ptr<AbstractDomain> or_(unsigned numBits, AbstractDomain &other);
  shared_ptr<AbstractDomain> xor_(unsigned numBits, AbstractDomain &other);

  /// Other operations:

  /// Subsets for predicate
  std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>
  subsetsForPredicate(
          AbstractDomain &other,
          std::function<bool(const APInt &, const APInt &)> comparision,
          CmpInst::Predicate pred);
  /// subsets for predicate ULE
  std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>
  subsetsForPredicateULE(StridedInterval &A, StridedInterval &B);

  /// icmp
  std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>
  icmp(CmpInst::Predicate pred, unsigned numBits, AbstractDomain &other);

  shared_ptr<AbstractDomain> leastUpperBound(AbstractDomain &other);
  bool lessOrEqual(AbstractDomain &other);

private:
    unsigned bitWidth;
    APInt begin;
    APInt end;
    APInt stride;
    bool isBot;
};

} // namespace pcpo
#endif
