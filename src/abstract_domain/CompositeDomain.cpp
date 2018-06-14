#include "CompositeDomain.h"
#include "llvm/Support/raw_os_ostream.h"
#include <functional>
#include <memory>

namespace pcpo {
using std::function;
using std::shared_ptr;

CompositeDomain::CompositeDomain(APInt value) : bitWidth{value.getBitWidth()}, delegateType{boundedSet} {
  delegate = shared_ptr<AbstractDomain>{new BoundedSet{value}};
}

// if isTop==true then we create a top delegate
// if isTop==false then we create a bottom delegate
CompositeDomain::CompositeDomain(unsigned bitWidth, bool isTop) : bitWidth{bitWidth}, delegateType{boundedSet} {
  delegate = shared_ptr<AbstractDomain>{new BoundedSet{bitWidth, isTop}};
}

CompositeDomain::CompositeDomain(const CompositeDomain &old)
    : bitWidth{bitWidth}, delegateType{old.delegateType} {
  if (old.delegateType == boundedSet) {
    BoundedSet *oldBs = static_cast<BoundedSet *>(old.delegate.get());
    delegate = shared_ptr<AbstractDomain>{new BoundedSet{*oldBs}};
  } else {
    StridedInterval *oldSi = static_cast<StridedInterval *>(old.delegate.get());
    delegate = shared_ptr<AbstractDomain>{new StridedInterval{*oldSi}};
  }
}

CompositeDomain::CompositeDomain(shared_ptr<AbstractDomain> del,
                                 DelegateType delType)
    : delegateType{delType}, delegate{del} {}

// computeOperation expects a CompositeDomain (CD) and a binary function to be
// evaluated on these. In case both (this and the argument) CompositeDomains
// contain a BoundedSet (BS), the function is executed on these. If this results
// in a top, both BoundedSets are converted to StridedIntervals (SI) and the
// operation is executed again. In case one of the CD contains a BS and the
// other a SI, the BS is converted to a SI. In case both CD contain a SI, the
// function is executed on the SIs.
shared_ptr<AbstractDomain> CompositeDomain::computeOperation(
    AbstractDomain &other,
    function<shared_ptr<AbstractDomain>(AbstractDomain &, AbstractDomain &)>
        op) {
  CompositeDomain &otherD = *static_cast<CompositeDomain *>(&other);

  if (otherD.getDelegateType() == boundedSet) {
    if (getDelegateType() == boundedSet) {
      // both are bounded sets
      auto resultOp = op(*delegate.get(), *otherD.delegate.get());
      // If the operation results in a top, this might be due
      // to the size limitation of the bounded set.
      // Thus, we transform them into strided intervals
      if (resultOp->isTop()) {
        BoundedSet otherBs = *static_cast<BoundedSet *>(otherD.delegate.get());
        BoundedSet thisBs = *static_cast<BoundedSet *>(this->delegate.get());
        StridedInterval otherDelegate{otherBs};
        StridedInterval thisDelegate{thisBs};
        resultOp = op(thisDelegate, otherDelegate);
        return shared_ptr<AbstractDomain>{
            new CompositeDomain{resultOp, stridedInterval}};
      } else {
        return shared_ptr<AbstractDomain>{
            new CompositeDomain{resultOp, boundedSet}};
      }
    } else {

      // other has a bounded set, we have a strided interval
      // change other to strided interval
      StridedInterval otherDelegate{otherD.delegate.get()};
      return shared_ptr<AbstractDomain>{new CompositeDomain{
          op(*delegate.get(), otherDelegate), stridedInterval}};
    }
  } else {
    // other is a strided interval
    if (getDelegateType() == boundedSet) {
      // this is a bounded set
      // change to strided interval
      StridedInterval thisDelegate{this->delegate.get()};
      return shared_ptr<AbstractDomain>{new CompositeDomain{
          op(thisDelegate, *otherD.delegate.get()), stridedInterval}};
    } else {
      // both are strided intervals already
      return shared_ptr<AbstractDomain>{new CompositeDomain{
          op(*delegate.get(), *otherD.delegate.get()), stridedInterval}};
    }
  }
}

// Binary Arithmetic Operations
shared_ptr<AbstractDomain> CompositeDomain::add(unsigned numBits,
                                                AbstractDomain &other, bool nuw,
                                                bool nsw) {
  auto operation = [numBits, nuw, nsw](AbstractDomain &lhs,
                                       AbstractDomain &rhs) {
    return lhs.add(numBits, rhs, nuw, nsw);
  };
  return computeOperation(other, operation);
}

shared_ptr<AbstractDomain> CompositeDomain::sub(unsigned numBits,
                                                AbstractDomain &other, bool nuw,
                                                bool nsw) {
  auto operation = [numBits, nuw, nsw](AbstractDomain &lhs,
                                       AbstractDomain &rhs) {
    return lhs.sub(numBits, rhs, nuw, nsw);
  };
  return computeOperation(other, operation);
}
shared_ptr<AbstractDomain> CompositeDomain::mul(unsigned numBits,
                                                AbstractDomain &other, bool nuw,
                                                bool nsw) {
  auto operation = [numBits, nuw, nsw](AbstractDomain &lhs,
                                       AbstractDomain &rhs) {
    return lhs.mul(numBits, rhs, nuw, nsw);
  };
  return computeOperation(other, operation);
}
shared_ptr<AbstractDomain> CompositeDomain::udiv(unsigned numBits,
                                                 AbstractDomain &other) {
  auto operation = [numBits](AbstractDomain &lhs,
                                       AbstractDomain &rhs) {
    return lhs.udiv(numBits, rhs);
  };
  return computeOperation(other, operation);
}
shared_ptr<AbstractDomain> CompositeDomain::sdiv(unsigned numBits,
                                                 AbstractDomain &other) {
  auto operation = [numBits](AbstractDomain &lhs,
                                       AbstractDomain &rhs) {
    return lhs.sdiv(numBits, rhs);
  };
  return computeOperation(other, operation);
}
shared_ptr<AbstractDomain> CompositeDomain::urem(unsigned numBits,
                                                 AbstractDomain &other) {
  auto operation = [numBits](AbstractDomain &lhs,
                                       AbstractDomain &rhs) {
    return lhs.urem(numBits, rhs);
  };
  return computeOperation(other, operation);
}
shared_ptr<AbstractDomain> CompositeDomain::srem(unsigned numBits,
                                                 AbstractDomain &other) {
  auto operation = [numBits](AbstractDomain &lhs,
                                       AbstractDomain &rhs) {
    return lhs.srem(numBits, rhs);
  };
  return computeOperation(other, operation);
}

// Binary Bitwise Operations
shared_ptr<AbstractDomain> CompositeDomain::shl(unsigned numBits,
                                                AbstractDomain &other, bool nuw,
                                                bool nsw) {
  auto operation = [numBits, nuw, nsw](AbstractDomain &lhs,
                                       AbstractDomain &rhs) {
    return lhs.shl(numBits, rhs, nuw, nsw);
  };
  return computeOperation(other, operation);
}
shared_ptr<AbstractDomain> CompositeDomain::lshr(unsigned numBits,
                                                 AbstractDomain &other) {
  auto operation = [numBits](AbstractDomain &lhs,
                                       AbstractDomain &rhs) {
    return lhs.lshr(numBits, rhs);
  };
  return computeOperation(other, operation);
}
shared_ptr<AbstractDomain> CompositeDomain::ashr(unsigned numBits,
                                                 AbstractDomain &other) {
  auto operation = [numBits](AbstractDomain &lhs,
                                       AbstractDomain &rhs) {
    return lhs.ashr(numBits, rhs);
  };
  return computeOperation(other, operation);
}
shared_ptr<AbstractDomain> CompositeDomain::and_(unsigned numBits,
                                                 AbstractDomain &other) {
  auto operation = [numBits](AbstractDomain &lhs,
                                       AbstractDomain &rhs) {
    return lhs.and_(numBits, rhs);
  };
  return computeOperation(other, operation);
}
shared_ptr<AbstractDomain> CompositeDomain::or_(unsigned numBits,
                                                AbstractDomain &other) {
  auto operation = [numBits](AbstractDomain &lhs,
                                       AbstractDomain &rhs) {
    return lhs.or_(numBits, rhs);
  };
  return computeOperation(other, operation);
}
shared_ptr<AbstractDomain> CompositeDomain::xor_(unsigned numBits,
                                                 AbstractDomain &other) {
  auto operation = [numBits](AbstractDomain &lhs,
                                       AbstractDomain &rhs) {
    return lhs.xor_(numBits, rhs);
  };
  return computeOperation(other, operation);
}

// Conversion Operations (TODO?)

// Other operations
std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>
CompositeDomain::icmp(CmpInst::Predicate pred, unsigned numBits,
                      AbstractDomain &other) {
  return std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>{
      nullptr, nullptr};
}

llvm::raw_ostream &CompositeDomain::print(llvm::raw_ostream &os) {
  return delegate->print(os);
}

// Lattice interface
shared_ptr<AbstractDomain>
CompositeDomain::leastUpperBound(AbstractDomain &other) {
  errs() << "in lub cd\n";
  auto operation = [](AbstractDomain &lhs, AbstractDomain &rhs) {
    return lhs.leastUpperBound(rhs);
  };
  return computeOperation(other, operation);
}

bool CompositeDomain::lessOrEqual(AbstractDomain &other) {
  CompositeDomain &otherD = *static_cast<CompositeDomain *>(&other);
  // TODO: Implement lessOrEqual for both BoundedSet and StridedInterval in
  // these classes
  return delegate->lessOrEqual(*otherD.delegate.get());
}

// |gamma(this)|
size_t CompositeDomain::size() const { return delegate->size(); }

bool CompositeDomain::isTop() const { return delegate->isTop(); }
bool CompositeDomain::isBottom() const { return delegate->isBottom(); }

bool CompositeDomain::contains(APInt& value) const{
  return false;
}

APInt CompositeDomain::getValueAt(uint64_t i) const {
  return delegate->getValueAt(i);
}

APInt CompositeDomain::getUMin() const {
   return delegate->getUMin();
}

APSInt CompositeDomain::getSMin() const { 
  return delegate->getSMin();
}

APInt CompositeDomain::getUMax() const { 
  return delegate->getUMax();
}

APSInt CompositeDomain::getSMax() const { 
  return delegate->getSMax();
}

// Debugging methodes
void CompositeDomain::printOut() const { delegate->printOut(); }

DelegateType CompositeDomain::getDelegateType() { return delegateType; }
} // namespace pcpo