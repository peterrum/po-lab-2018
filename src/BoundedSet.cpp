#include "BoundedSet.h"
#include "AbstractDomain.h"
#include "llvm/ADT/APInt.h"
#include "llvm/Support/raw_os_ostream.h"
#include <iostream>
#include <iterator>
#include <memory>
#include <set>
#include <initializer_list>
#include <iostream>
#include <utility>

namespace pcpo {

using namespace llvm;
using std::shared_ptr;

BoundedSet::BoundedSet(std::set<APInt, Comparator> vals) {
  values = vals;
}
BoundedSet::BoundedSet(APInt val) { values.insert(val); }
BoundedSet::BoundedSet(bool isTop) : top(isTop) {}
BoundedSet::BoundedSet(std::initializer_list<APInt> vals) {
  values = vals;
}
BoundedSet::BoundedSet(unsigned numBits, std::initializer_list<uint64_t> vals) {
  for (auto val : vals) {
    values.insert(APInt{numBits, val});
  }
}

bool BoundedSet::operator==(const BoundedSet &other) {
  if (this->top && other.top) {
    return true;
  } else {
    return this->values == other.values;
  }
}

shared_ptr<AbstractDomain>
BoundedSet::compute(AbstractDomain &other,
                    std::function<BoundedSet(const APInt&, const APInt&)> op) {
  if (BoundedSet *otherB = static_cast<BoundedSet *>(&other)) {
    shared_ptr<AbstractDomain> newValues {new BoundedSet(false)};
    if (top || otherB->top) {
      BoundedSet *res = new BoundedSet(true);
      return shared_ptr<BoundedSet>{res};
    }
    for (auto &leftVal : values) {
      for (auto &rightVal : otherB->values) {
        BoundedSet res = op(leftVal, rightVal);
        newValues = newValues->leastUpperBound(res);
        if (newValues->size() > SET_LIMIT) {
          shared_ptr<BoundedSet> topSet{new BoundedSet(true)};
          return topSet;
        }
      }
    }
    return newValues;
  }
  return nullptr;
}

shared_ptr<AbstractDomain> BoundedSet::add(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) {
  auto opPlus = [numBits, nuw, nsw](APInt left, APInt right) {
    APInt newValue{numBits, 0};
    newValue += left;
    newValue += right;
    bool overflow {false};
    overflow |= (nsw &&
      left.isNonNegative() == right.isNonNegative() &&
      left.isNonNegative() != newValue.isNonNegative());
    overflow |= (nuw &&
      newValue.ult(right));
    if (overflow) {
      return BoundedSet{true};
    } else {
      return BoundedSet{newValue};
    }
  };
  return compute(other, opPlus);
}
shared_ptr<AbstractDomain> BoundedSet::sub(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) {
  auto opPlus = [numBits, nuw, nsw](APInt left, APInt right) {
    APInt newValue{numBits, 0};
    newValue += left;
    newValue -= right;
    bool overflow {false};
    overflow |= (nsw &&
      left.isNonNegative() != right.isNonNegative() &&
      left.isNonNegative() != newValue.isNonNegative());
    overflow |= (nuw &&
      newValue.ugt(right));
    if (overflow) {
      return BoundedSet{true};
    } else {
      return BoundedSet{newValue};
    }
  };
  return compute(other, opPlus);
}
shared_ptr<AbstractDomain> BoundedSet::mul(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) {
    auto opMul = [numBits, nuw, nsw](APInt lhs, APInt rhs) {
    APInt res{numBits, 0};
    res += lhs;
    res *= rhs;
    bool overflow {false};
    if (nsw) {
      if (lhs != 0 && lhs != 0) {
        overflow |= res.udiv(rhs) != lhs || res.udiv(lhs) != rhs;
      }
    }
    if (nuw) {
      if (lhs != 0 && rhs != 0) {
        overflow |= res.udiv(rhs) != lhs || res.udiv(res) != rhs;
      }
    }
    if (overflow) {
      return BoundedSet{true};
    } else {
      return BoundedSet{res};
    }
  };
  return compute(other, opMul);
}
shared_ptr<AbstractDomain> BoundedSet::udiv(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) {
  auto opUDiv = [numBits](APInt lhs, APInt rhs) {
    APInt res{numBits, 0};
    res += lhs;
    res = res.udiv(rhs);
    return BoundedSet{res};
  };
  return compute(other, opUDiv);
}
shared_ptr<AbstractDomain> BoundedSet::sdiv(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) {
  auto opSDiv = [numBits](APInt lhs, APInt rhs) {
    APInt res{numBits, 0};
    res += lhs;
    res = res.sdiv(rhs);
    return BoundedSet{res};
  };
  return compute(other, opSDiv);
}
shared_ptr<AbstractDomain> BoundedSet::urem(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) {
  auto opURem = [numBits](APInt lhs, APInt rhs) {
    APInt res{numBits, 0};
    res += lhs;
    res = res.urem(rhs);
    return BoundedSet{res};
  };
  return compute(other, opURem);
}
shared_ptr<AbstractDomain> BoundedSet::srem(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) {
  auto opSRem = [numBits](APInt lhs, APInt rhs) {
    APInt res{numBits, 0};
    res += lhs;
    res = res.srem(rhs);
    return BoundedSet{res};
  };
  return compute(other, opSRem);
}
shared_ptr<AbstractDomain> BoundedSet::shl(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) {
    auto opShl = [numBits, nuw, nsw](APInt lhs, APInt shAmt) {
    APInt res{numBits, 0};
    res += lhs;
    res.shl(shAmt);
    bool overflow {shAmt.uge(numBits)};
    if (lhs.isNonNegative()) {
      overflow |= shAmt.uge(lhs.countLeadingZeros());
    } else {
      overflow |= shAmt.uge(lhs.countLeadingZeros());
    }
    if (overflow) {
      return BoundedSet{true};
    } else {
      return BoundedSet{res};
    }
  };
  return compute(other, opShl);
}

//lshr shifts the this object to the right, using a zero fill on the right  
shared_ptr<AbstractDomain> BoundedSet::lshr(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) {
  auto opLShr = [numBits](APInt lhs, APInt rhs) {
    APInt res{numBits, 0};
    res += lhs;
    res = res.lshr(rhs);
    return BoundedSet{res};
  };
  return compute(other, opLShr);
}

shared_ptr<AbstractDomain> BoundedSet::ashr(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) {
  auto opAShr = [numBits] (APInt lhs, APInt rhs) {
    APInt res{numBits, 0};
    res += lhs;
    res = res.ashr(rhs);
    return BoundedSet{res};
  };
  return compute(other, opAShr);
}
shared_ptr<AbstractDomain> BoundedSet::and_(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) {
  auto opAnd = [numBits] (APInt lhs, APInt rhs) {
    APInt res{numBits, 0};
    res += lhs;
    res &= rhs;
    return BoundedSet{res};
  };
  return compute(other, opAnd);
}
shared_ptr<AbstractDomain> BoundedSet::or_(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) {
  auto opOr = [numBits] (APInt lhs, APInt rhs) {
    APInt res{numBits, 0};
    res += lhs;
    res |= rhs;
    return BoundedSet{res};
  };
  return compute(other, opOr);
}
shared_ptr<AbstractDomain> BoundedSet::xor_(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) {
  auto opNot = [numBits] (APInt lhs, APInt rhs) {
    APInt res{numBits, 0};
    res += lhs;
    res ^= rhs;
    return BoundedSet{res};
  };
  return compute(other, opNot);
}

// returns a shared pointer to a new BoundedSet
// if the flag is set, it returs a top, otherwise a bottomstd::make_pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>(std::make_shared<AbstractDomain>(new BoundedSet(true)), std::make_shared<AbstractDomain>(new BoundedSet(true)));
shared_ptr<AbstractDomain> createBoundedSetPointer(bool  top){
  std::shared_ptr<AbstractDomain> resultPtr(new BoundedSet(top));
  return resultPtr;
}

std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>
createBoundedSetPointerPair(bool firstTop, bool secondTop){
  return std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>(createBoundedSetPointer(firstTop), createBoundedSetPointer(secondTop));
}

//Returns two subsets of the values of this BoundedSet that that can lead to a true and a false evaluation, respectively
//Note that a given value may be contained in both sets of the return pair.
std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>
BoundedSet::subsetsForPredicate(unsigned numBits, AbstractDomain &other, std::function<bool(const APInt&, const APInt&)> comparision){
  if (BoundedSet *otherB = static_cast<BoundedSet *>(&other)) {
    
    if(isTop()){
      // if this set is top, it will be top in both branches afterwards
      return createBoundedSetPointerPair(true, true);
    }

    if(otherB->isTop()){
      // if the other set is top; we cannot infer more details about our set in both branches afterwards
      // thus, the set stays the same
      shared_ptr<AbstractDomain> copy {new BoundedSet(*this)};
      return std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>(
          copy,
          createBoundedSetPointer(true));
    }
    std::set<APInt, Comparator> trueValues;
    std::set<APInt, Comparator> falseValues;

    for(auto &leftVal : values){
      for(auto &rightVal : otherB->values){
        if(comparision(leftVal, rightVal)){
          trueValues.insert(leftVal);
        } else {
          falseValues.insert(leftVal);
        }
      }
    }

    shared_ptr<AbstractDomain> trueSet{new BoundedSet(trueValues)};
    shared_ptr<AbstractDomain> falseSet{new BoundedSet(falseValues)};
    return std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>(
        trueSet, falseSet);
  }

  return createBoundedSetPointerPair(true, true);
}

std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>
BoundedSet::icmp(CmpInst::Predicate pred, unsigned numBits, AbstractDomain &other) {
  return std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>
    (shared_ptr<AbstractDomain>(new BoundedSet(true)), shared_ptr<AbstractDomain>(new BoundedSet(true)));
}

shared_ptr<AbstractDomain> BoundedSet::leastUpperBound(AbstractDomain &other) {
  if (BoundedSet *otherB = static_cast<BoundedSet *>(&other)) {
    std::set<APInt, Comparator> result;
    for (auto &val : values) {
      result.insert(val);
    }
    int count = values.size();
    for (auto &val : otherB->values) {
      if (++count > SET_LIMIT) {
        return shared_ptr<BoundedSet>{new BoundedSet(true)};
      }
      result.insert(val);
    }
    shared_ptr<BoundedSet> res{new BoundedSet{result}};
    return res;
  }
  return nullptr;
}

// is this boundedSet less or equal to the other?
bool BoundedSet::lessOrEqual(AbstractDomain &other) {
  if (BoundedSet *otherB = static_cast<BoundedSet *>(&other)) {
    if(otherB->isTop()){
      return true;
    }
    if(isTop()){
      return otherB->isTop();
    }
    auto end = otherB->values.end();
    for (auto &val : values) {
      if (otherB->values.find(val) == end) {
        // val is not contained int otherB->values
        return false;
      }
    }
    return true;
  }
  // TODO: throw exception if comparison is not implemented
  return false;
}

void BoundedSet::printOut() {
  //   errs() << "BoundedSet@" << this << std::endl;
  errs() << "BoundedSet@" << this << "\n";
  if (top) {
    errs() << "T\n";
    return;
  }
  for (auto &val : values) {
    errs() << val.toString(10, false) << "\n";
  }
}

bool BoundedSet::isTop() { return top; }
size_t BoundedSet::size() { return values.size(); }
} // namespace pcpo