#ifndef STRIDED_INTERVAL_H_
#define STRIDED_INTERVAL_H_
#include "AbstractDomain.h"
#include "BoundedSet.h"
#include "llvm/ADT/APInt.h"
#include <functional>
#include <iostream>
#include <set>

namespace pcpo {
using llvm::APInt;

class StridedInterval : public AbstractDomain {
private:
  unsigned bitWidth;
  APInt begin;
  APInt end;
  APInt stride;
  bool isBot;

public:
  std::shared_ptr<AbstractDomain> normalize();

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
  size_t size() const;

  shared_ptr<AbstractDomain> leastUpperBound(AbstractDomain &other);
  bool lessOrEqual(AbstractDomain &other);

  bool operator==(const StridedInterval &other);
  bool operator!=(const StridedInterval &other);

  explicit StridedInterval(APInt value);
  friend std::ostream &operator<<(std::ostream &os, const StridedInterval &bs);

  explicit StridedInterval(bool isTop);
  StridedInterval();
  StridedInterval(APInt begin, APInt end, APInt stride);
  StridedInterval(unsigned numBits, std::initializer_list<uint64_t> vals);
  StridedInterval(unsigned bitWidth, uint64_t begin, uint64_t end,
                  uint64_t stride);
  StridedInterval(BoundedSet &set);
  unsigned getBitWidth() const;
  bool isTop() const;
  bool isBottom() const;
  bool contains(APInt &value) const;
  void printOut() const;

  virtual llvm::raw_ostream &print(llvm::raw_ostream &os);

  static shared_ptr<AbstractDomain> create_bottom() {
    return std::shared_ptr<AbstractDomain>(new StridedInterval(false));
  }

  static shared_ptr<AbstractDomain> create_top() {
    return std::shared_ptr<AbstractDomain>(new StridedInterval(true));
  }

  // static shared_ptr<AbstractDomain> create_bottom() {
  //   return std::shared_ptr<AbstractDomain>(new StridedInterval());
  // }

  // static shared_ptr<AbstractDomain> create_top() {
  //   return std::shared_ptr<AbstractDomain>(new StridedInterval());
  // }
};

} // namespace pcpo
#endif
