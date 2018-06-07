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
    int count = 0;
    shared_ptr<AbstractDomain> newValues {new BoundedSet(false)};
    if (top || otherB->top) {
      BoundedSet *res = new BoundedSet(true);
      return shared_ptr<BoundedSet>{res};
    }
    for (auto &leftVal : values) {
      for (auto &rightVal : otherB->values) {
        if (++count > SET_LIMIT) {
          shared_ptr<BoundedSet> topSet{new BoundedSet(true)};
        }

        BoundedSet res = op(leftVal, rightVal);
        newValues = newValues->leastUpperBound(res);
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
  auto opMinus = [numBits, nuw, nsw](APInt left, APInt right) {
    APInt newValue{numBits, 0};
    // TODO: check nsw nuw
    newValue += left;
    newValue -= right;
    return BoundedSet{newValue};
  };
  return compute(other, opMinus);
}
shared_ptr<AbstractDomain> BoundedSet::mul(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) {
  auto opMinus = [numBits, nuw, nsw](APInt left, APInt right) {
    APInt newValue{numBits, 0};
    // TODO: check nsw nuw
    newValue += left;
    newValue *= right;
    return BoundedSet{newValue};
  };
  return compute(other, opMinus);
}
shared_ptr<AbstractDomain> BoundedSet::udiv(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) {
  return shared_ptr<AbstractDomain> (new BoundedSet(true));
}
shared_ptr<AbstractDomain> BoundedSet::sdiv(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) {
  return shared_ptr<AbstractDomain> (new BoundedSet(true));
}
shared_ptr<AbstractDomain> BoundedSet::urem(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) {
  return shared_ptr<AbstractDomain> (new BoundedSet(true));
}
shared_ptr<AbstractDomain> BoundedSet::srem(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) {
  return shared_ptr<AbstractDomain> (new BoundedSet(true));
}
shared_ptr<AbstractDomain> BoundedSet::shl(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) {
  return shared_ptr<AbstractDomain> (new BoundedSet(true));
}
shared_ptr<AbstractDomain> BoundedSet::shlr(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) {
  return shared_ptr<AbstractDomain> (new BoundedSet(true));
}
shared_ptr<AbstractDomain> BoundedSet::ashr(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) {
  return shared_ptr<AbstractDomain> (new BoundedSet(true));
}
shared_ptr<AbstractDomain> BoundedSet::and_(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) {
  return shared_ptr<AbstractDomain> (new BoundedSet(true));
}
shared_ptr<AbstractDomain> BoundedSet::or_(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) {
  return shared_ptr<AbstractDomain> (new BoundedSet(true));
}
shared_ptr<AbstractDomain> BoundedSet::xor_(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) {
  return shared_ptr<AbstractDomain> (new BoundedSet(true));
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

// implements contains
bool BoundedSet::lessOrEqual(AbstractDomain &other) {
  if (BoundedSet *otherB = static_cast<BoundedSet *>(&other)) {
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
} // namespace pcpo