#include "CompositeDomain.h"
#include <functional>
#include <memory>

namespace pcpo {
using std::function;
using std::shared_ptr;
// Binary Arithmetic Operations

shared_ptr<AbstractDomain> CompositeDomain::add(unsigned numBits,
                                                AbstractDomain &other, bool nuw,
                                                bool nsw) {
  CompositeDomain &otherD = *static_cast<CompositeDomain *>(&other);
  if (otherD.getDelegateType() == boundedSet &&
      getDelegateType() == boundedSet) {

    BoundedSet &otherDelegate =
        *static_cast<BoundedSet *>(otherD.delegate.get());
    return this->delegate->add(numBits, otherDelegate, nuw, nsw);
  } else if (this->delegateType == boundedSet) {
    StridedInterval lhsDelegate{this->delegate.get()};
    return lhsDelegate.add(numBits, *otherD.delegate.get(), nuw, nsw);
  } else if (otherD.delegateType == boundedSet) {
    StridedInterval lhsDelegate{this->delegate.get()};
    StridedInterval &rhsDelegate =
        *static_cast<StridedInterval *>(otherD.delegate.get());
    return lhsDelegate.add(numBits, rhsDelegate, nuw, nsw);
  } else {
    // both are strided intervals
    return delegate->add(numBits, *otherD.delegate.get(), nuw, nsw);
  }
}
shared_ptr<AbstractDomain> CompositeDomain::sub(unsigned numBits,
                                                AbstractDomain &other, bool nuw,
                                                bool nsw) {
  return nullptr;
}
shared_ptr<AbstractDomain> CompositeDomain::mul(unsigned numBits,
                                                AbstractDomain &other, bool nuw,
                                                bool nsw) {
  return nullptr;
}
shared_ptr<AbstractDomain> CompositeDomain::udiv(unsigned numBits,
                                                 AbstractDomain &other,
                                                 bool nuw, bool nsw) {
  return nullptr;
}
shared_ptr<AbstractDomain> CompositeDomain::sdiv(unsigned numBits,
                                                 AbstractDomain &other,
                                                 bool nuw, bool nsw) {
  return nullptr;
}
shared_ptr<AbstractDomain> CompositeDomain::urem(unsigned numBits,
                                                 AbstractDomain &other,
                                                 bool nuw, bool nsw) {
  return nullptr;
}
shared_ptr<AbstractDomain> CompositeDomain::srem(unsigned numBits,
                                                 AbstractDomain &other,
                                                 bool nuw, bool nsw) {
  return nullptr;
}

// Binary Bitwise Operations
shared_ptr<AbstractDomain> CompositeDomain::shl(unsigned numBits,
                                                AbstractDomain &other, bool nuw,
                                                bool nsw) {
  return nullptr;
}
shared_ptr<AbstractDomain> CompositeDomain::lshr(unsigned numBits,
                                                 AbstractDomain &other,
                                                 bool nuw, bool nsw) {
  return nullptr;
}
shared_ptr<AbstractDomain> CompositeDomain::ashr(unsigned numBits,
                                                 AbstractDomain &other,
                                                 bool nuw, bool nsw) {
  return nullptr;
}
shared_ptr<AbstractDomain> CompositeDomain::and_(unsigned numBits,
                                                 AbstractDomain &other,
                                                 bool nuw, bool nsw) {
  return nullptr;
}
shared_ptr<AbstractDomain> CompositeDomain::or_(unsigned numBits,
                                                AbstractDomain &other, bool nuw,
                                                bool nsw) {
  return nullptr;
}
shared_ptr<AbstractDomain> CompositeDomain::xor_(unsigned numBits,
                                                 AbstractDomain &other,
                                                 bool nuw, bool nsw) {
  return nullptr;
}

// Conversion Operations (TODO?)

// Other operations
std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>
CompositeDomain::icmp(CmpInst::Predicate pred, unsigned numBits,
                      AbstractDomain &other) {
  return std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>{
      nullptr, nullptr};
}

llvm::raw_ostream &CompositeDomain::print(llvm::raw_ostream &os) { return os; }

// Lattice interface
shared_ptr<AbstractDomain>
CompositeDomain::leastUpperBound(AbstractDomain &other) {
  return nullptr;
}
bool CompositeDomain::lessOrEqual(AbstractDomain &other) { return false; }

// |gamma(this)|
size_t CompositeDomain::size() const { return 0; }

bool CompositeDomain::isTop() const { return false; }
bool CompositeDomain::isBottom() const { return false; }

// Debugging methodes
void CompositeDomain::printOut() const {}

DelegateType CompositeDomain::getDelegateType() { return delegateType; }
} // namespace pcpo