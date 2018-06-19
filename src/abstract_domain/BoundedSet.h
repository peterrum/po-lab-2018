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

class BoundedSet : public AbstractDomain {

public:

  /// Constructors
  BoundedSet(unsigned bitWidth,std::set<APInt, Comparator> values);
  BoundedSet(APInt value);
  BoundedSet(unsigned numBits, bool isTop);
  BoundedSet(unsigned numBits, std::initializer_list<APInt> vals);
  BoundedSet(unsigned numBits, std::initializer_list<uint64_t> vals);

  /// Bottom BS
  static shared_ptr<AbstractDomain> create_bottom(unsigned bitWidth) { return std::shared_ptr<AbstractDomain>(new BoundedSet(bitWidth, false)); }
  /// Top BS
  static shared_ptr<AbstractDomain> create_top(unsigned bitWidth) { return std::shared_ptr<AbstractDomain>(new BoundedSet(bitWidth, true)); }

  /// Copy constructor
  BoundedSet(const BoundedSet & b);
  /// Copy assignment
  //BoundedSet& operator=(const BoundedSet& other) = default;

  /// Comparison Operators
  bool operator==(const BoundedSet &other);
  bool operator!=(const BoundedSet &other);
  //bool lessOrEqual(AbstractDomain &other);
  bool operator<=(AbstractDomain &other);

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

  /// Other operations
  std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>
  icmp(CmpInst::Predicate pred, unsigned numBits, AbstractDomain &other);

  shared_ptr<AbstractDomain> leastUpperBound(AbstractDomain &other);

  /// Member functions
  bool contains(APInt &value) const;
  unsigned getBitWidth() const;
  std::set<APInt, Comparator> getValues() const;
  bool isTop() const;
  bool isBottom() const;
  size_t size() const;

  /// Member functions for API
  APInt getValueAt(uint64_t i) const;
  APInt getUMin() const;
  APSInt getSMin() const;
  APInt getUMax() const;
  APSInt getSMax() const;

  /// Print
  void printOut() const;
  virtual llvm::raw_ostream &print(llvm::raw_ostream &os);

private:
  /// TODO comment
  shared_ptr<AbstractDomain> compute(AbstractDomain &other, std::function<BoundedSet(const APInt &, const APInt &)> op);
  /// Returns two subsets of the values of this BoundedSet that that can lead to a true and a false evaluation,
  /// respectively Note that a given value may be contained in both sets of the return pair.
  std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>> subsetsForPredicate(
          AbstractDomain &other, std::function<bool(const APInt &, const APInt &)> comparision, CmpInst::Predicate pred);

  /// returns a shared pointer to a new BoundedSet if the flag is set, it returns a top, otherwise bottom
  shared_ptr<AbstractDomain> createBoundedSetPointer(unsigned bitWidth, bool top);
  std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>> createBoundedSetPointerPair(
          unsigned bitWidth, bool firstTop, bool secondTop);

  /// private convenience method: right now only used to test whether this BoundedSet contains numBits
  bool containsValue(unsigned numBits, uint64_t n) const;

  /// Warning. We don't handle these cases.
  void warnIfContainsZero(unsigned numBits);
  void warnIfDivisionOverflowPossible(unsigned numBits, BoundedSet &other);

  /// Member
  unsigned bitWidth;
  std::set<APInt, Comparator> values;
  bool top{false};
};
} // namespace pcpo
#endif
