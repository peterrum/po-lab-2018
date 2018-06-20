#ifndef STRIDED_INTERVAL_H_
#define STRIDED_INTERVAL_H_
#include <llvm/ADT/APInt.h>
#include <functional>
#include <iostream>
#include <set>
#include "AbstractDomain.h"
#include "Util.h"
#include "BoundedSet.h"

namespace pcpo {
using llvm::APInt;

class StridedInterval : public AbstractDomain {

public:

  /// Constructor: Bottom
  StridedInterval() : isBot(true) {}
  /// Constructor: Top
  StridedInterval(bool isTop, unsigned bitWidth);
  /// Constructor: Constant
  StridedInterval(APInt value);
  /// Constructor: Interval with APInt
  StridedInterval(APInt begin, APInt end, APInt stride);
  /// Constructor: Interval with uint64_t
  StridedInterval(unsigned bitWidth, uint64_t begin, uint64_t end, uint64_t stride);
  //StridedInterval(unsigned numBits, std::initializer_list<uint64_t> vals);
  /// Constructor: From BoundedSet
  StridedInterval(BoundedSet &set);

  /// Bottom SI
  static shared_ptr<AbstractDomain> create_bottom(unsigned bitWidth)
      { return std::shared_ptr<AbstractDomain>(new StridedInterval(false, bitWidth)); }
  /// Top SI
  static shared_ptr<AbstractDomain> create_top(unsigned bitWidth)
      { return std::shared_ptr<AbstractDomain>(new StridedInterval(true, bitWidth)); }

  /// Copy constructor
  StridedInterval(const StridedInterval& other);
  /// Copy assignment
  StridedInterval& operator=(const StridedInterval& other) = default;

  /// Comparison Operators
  bool operator==(const StridedInterval &other);
  bool operator!=(const StridedInterval &other) {return !(operator==(other));}
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

  /// Other operations:
  /// Subsets for predicate
  std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>
  subsetsForPredicate(
          AbstractDomain &other,
          CmpInst::Predicate pred);

  /// Functions for icmp predicates
  std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>
  subsetsForPredicateEQ(StridedInterval &A, StridedInterval &B);
  std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>
  subsetsForPredicateSLE(StridedInterval &A, StridedInterval &B);
  std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>
  subsetsForPredicateSLT(StridedInterval &A, StridedInterval &B);
  std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>
  subsetsForPredicateULE(StridedInterval &A, StridedInterval &B);
  std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>
  subsetsForPredicateULT(StridedInterval &A, StridedInterval &B);


  /// icmp
  std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>
  icmp(CmpInst::Predicate pred, unsigned numBits, AbstractDomain &other);

  // Check whether this is a wrap around interval
  bool isWrapAround() const;

  // Conduct an overapproximated intersection of two intervals.
  shared_ptr<AbstractDomain> intersect(const StridedInterval &A, const StridedInterval &B);
  shared_ptr<AbstractDomain> intersectWithBounds(const StridedInterval &first,
                                                 const StridedInterval &second);

  shared_ptr<AbstractDomain> leastUpperBound(AbstractDomain &other);

  // Widening
  virtual shared_ptr<AbstractDomain> widen();
  virtual bool requiresWidening();


  /// Member functions
  unsigned getBitWidth() const { return bitWidth; }
  bool isTop() const;
  bool isBottom() const { return isBot; }
  bool contains(APInt &value) const;
  size_t size() const;

  /// TODO comment
  std::shared_ptr<AbstractDomain> normalize();
  bool isNormal();
  std::set<APInt, Comparator> gamma();

  /// Member functions for API
  APInt getValueAt(uint64_t i) const { return begin + stride*i; }
  APInt getUMin() const { return umin(); }
  APSInt getSMin() const { return APSInt(smin(),false); }
  APInt getUMax() const { return umax(); }
  APSInt getSMax() const { return APSInt(smax(),false); }

  /// Print
  friend std::ostream &operator<<(std::ostream &os, const StridedInterval &bs);
  virtual llvm::raw_ostream &print(llvm::raw_ostream &os);
  void printOut() const;

private:

    /// TODO: this vs get methods API
    APInt umax() const;
    APInt umin() const;
    APInt smax() const;
    APInt smin() const;
    APInt sstride() const;
    APInt ustride() const;

    unsigned bitWidth;
    APInt begin;
    APInt end;
    APInt stride;
    bool isBot;
};

} // namespace pcpo
#endif
