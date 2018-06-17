#include <llvm/ADT/APInt.h>
#include <llvm/Support/raw_os_ostream.h>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <vector>

#include "StridedInterval.h"
#include "AbstractDomain.h"
#include "Util.h"
#include "BoundedSet.h"

namespace pcpo {
using llvm::APInt;
using llvm::APIntOps::GreatestCommonDivisor;
using std::vector;


StridedInterval::StridedInterval(const StridedInterval& other)
    : bitWidth(other.bitWidth),
      begin(other.begin),
      end(other.end),
      stride(other.stride),
      isBot(other.isBot) {}

StridedInterval::StridedInterval(APInt begin, APInt end, APInt stride)
    : bitWidth(begin.getBitWidth()), begin(begin), end(end), stride(stride),
      isBot(false) {
  assert(end.getBitWidth() == bitWidth);
  assert(stride.getBitWidth() == bitWidth);
}

StridedInterval::StridedInterval(APInt value)
    : bitWidth(value.getBitWidth()), begin(value), end(value),
      stride({value.getBitWidth(), 0}), isBot{false} {}

StridedInterval::StridedInterval(bool isTop, unsigned bitWidth): bitWidth(bitWidth)
  ,begin(APInt(bitWidth, 0)),
  end(APInt::getMaxValue(bitWidth)),
  stride(APInt(bitWidth, 1)),
  isBot(!isTop) {
}

StridedInterval::StridedInterval(unsigned bitWidth, uint64_t begin,
                                 uint64_t end, uint64_t stride)
    : bitWidth(bitWidth), begin(APInt(bitWidth, begin)),
      end(APInt(bitWidth, end)), stride(APInt(bitWidth, stride)), isBot(false) {
}

StridedInterval::StridedInterval(BoundedSet &set)
    : bitWidth(set.getBitWidth()) {
  if (set.isBottom()) {
    // create this a bottom
    isBot = true;
    return;
  } else if (set.isTop()) {
    isBot = false;
    begin = APInt{bitWidth, 0};
    end = APInt::getMaxValue(bitWidth);
    stride = APInt{bitWidth, 1};
  } else {
    // create a custom interval

    // we do a case distinction on the size of the BoundedSet
    const auto vals = set.getValues();
    assert(vals.size() > 0);
    if (vals.size() == 1) {
      // Create singleton
      auto value = vals.begin();
      begin = *value;
      end = *value;
      stride = *value;
      isBot = false;
    } else {
      // Create StridedInterval from BoundedSet with multiple values
      // for faster access, we first load the values of the BS into a vector
      vector<APInt> values{};
      for (auto &val : vals) {
        values.push_back(val);
      }
      // variables for begin, end and stride
      APInt b;
      APInt e;
      APInt s;
      // We consider every point in the BoundedSet as a start point of the Interval
      // We will pick the interval with the minimum number of elements
      APInt second;
      const auto size = values.size();
      const auto offset = size - 1;
      size_t min = std::numeric_limits<size_t>::max();
      StridedInterval minInterval{};

      // Consider every value as a start point
      for (size_t i = 0; i < size; i++) {
        auto lastIndex = (i + offset) % size;
        b = values.at(i % size);
        second = values.at((i + 1) % size);
        e = values.at(lastIndex);

        // Stride will be gcd of all differences between points next to each other
        APInt diff;
        APInt gcd;
        if (second.uge(b)) {
          diff = second;
          diff -= b;
        }
        else {
          diff = b;
          diff -= second;
        }
        gcd = diff;

        // calculate stride
        for (size_t j = (i + 1) % size; (j + 1) % size != lastIndex;
             j = (j + 1) % size) {
          auto current = values.at(j);
          auto next = values.at((j + 1) % size);

          if (next.uge(current)) {
            diff = next;
            diff -= current;
            gcd = GreatestCommonDivisor(gcd, diff);
          } else {
            diff = current;
            diff -= next;
            gcd = GreatestCommonDivisor(gcd, diff);            
          }
        }

        // check whether interval starting at b has minimum number of elements
        StridedInterval tmp{b, e, gcd};
        if (tmp.size() <= min) {
          min = tmp.size();
          minInterval = StridedInterval(tmp);
        }
      }
      isBot = false;
      begin = minInterval.begin;
      end = minInterval.end;
      stride = minInterval.stride;
    }
  }
}

bool StridedInterval::operator==(const StridedInterval &other) {
  if (this->isBot) {
    return other.isBot;
  } else {
    return this->begin == other.begin && this->end == other.end &&
           this->stride == other.stride;
  }
}

APInt add_(const APInt &a, const APInt &b) {
  APInt c = APInt(a.getBitWidth(), 0);
  c += a;
  c += b;
  return c;
}

APInt sub_(const APInt &a, const APInt &b) {
  APInt c = APInt(a.getBitWidth(), 0);
  c += a;
  c -= b;
  return c;
}

APInt mul_(const APInt &a, const APInt &b) {
  APInt c = APInt(a.getBitWidth(), 0);
  c += a;
  c *= b;
  return c;
}

APInt mod(const APInt &a, const APInt &b) { return a.urem(b); }

APInt div(const APInt &a, const APInt &b) { return a.udiv(b); }

APInt pow2(unsigned n, unsigned bitWidth) {
  assert(n <= bitWidth); 
  APInt b{bitWidth, 0};
  b.setBit(n);
  return b;
}

std::shared_ptr<AbstractDomain> StridedInterval::normalize() {
  unsigned n = bitWidth;
  APInt N(pow2(n, n + 1));
  APInt a(begin.zext(n + 1));
  APInt b(end.zext(n + 1));
  APInt s(stride.zext(n + 1));
  if (s == 0) {
    b = a;
  } else {
    // b_ = min{k in Z | k >= a and k = b mod N}
    APInt b_ = a.ule(b) ? b : add_(b, N);
    b = mod(sub_(b_, mod(sub_(b_, a), s)), N);
    if (a == b) {
      s = 0;
    } else {
      if (mod(N, s) == 0 && mod(sub_(a, b), N) == s) {
        a = mod(a, s);
        b = mod(sub_(a, s), N);
      } else if (b == mod(add_(a, s), N) && b.ult(a)) {
        std::swap(a, b);
        s = sub_(b, a);
      }
    }
  }
  return std::shared_ptr<AbstractDomain>(new StridedInterval(
      a.zextOrTrunc(n), b.zextOrTrunc(n), s.zextOrTrunc(n)));
}

bool StridedInterval::isNormal() {
  std::shared_ptr<AbstractDomain> thisNorm = this->normalize();
  return *this == *(static_cast<StridedInterval *>(thisNorm.get()));
}

std::set<APInt, Comparator> StridedInterval::gamma() {
  unsigned n = bitWidth;
  std::set<APInt, Comparator> res {};
  APInt s(stride.zext(n+1));
  if (s.isNullValue()) {
    res.insert(begin);
    return res;
  } else {
    APInt N(pow2(n, n+1));
    APInt a(begin.zext(n+1));
    APInt b(end.zext(n+1));
    b = a.ule(b) ? b : add_(b, N);
    for (APInt k = a; k.ule(b); k += s) {
      res.insert(k.trunc(n));
    }
    return res;
  }
}

APInt StridedInterval::umax() {
  APInt a = begin;
  APInt b = end;
  APInt s = stride;
  if (a.ule(b)) {
    return b;
  } else {
    APInt m = APInt::getMaxValue(bitWidth);
    return sub_(m, mod(sub_(m, a), s));
  }
}

APInt StridedInterval::umin() {
  APInt a = begin;
  APInt b = end;
  APInt s = stride;
  if (a.ule(b)) {
    return a;
  } else {
    return mod(b, s);
  }
}

APInt StridedInterval::smax() {
  APInt a = begin;
  APInt b = end;
  APInt s = stride;
  if (a.sle(b)) {
    return b;
  } else {
    APInt m = APInt::getSignedMaxValue(bitWidth);
    return sub_(m, mod(sub_(m, a), s));
  }
}

APInt StridedInterval::smin() {
  APInt a = begin;
  APInt b = end;
  APInt s = stride;
  if (a.sle(b)) {
    return a;
  } else {
    APInt m = APInt::getSignedMinValue(bitWidth);
    return add_(mod(sub_(b, m), s), m);
  }
}

shared_ptr<AbstractDomain> StridedInterval::add(unsigned numBits,
    AbstractDomain &other, bool nuw, bool nsw) {
  StridedInterval *otherSI = static_cast<StridedInterval *>(&other);
  assert(numBits == bitWidth);
  assert(numBits == otherSI->bitWidth);
  if (this->isBottom() || otherSI->isBottom()) {
    return StridedInterval::create_bottom(bitWidth);
  }
  if (nuw) {
    APInt maxThis = this->umax();
    APInt maxOther = otherSI->umax();
    bool ov;
    ///ignoring return value -> checking if overflow bit (ov) is being set
    (void) maxThis.uadd_ov(maxOther, ov);
    if (ov) {
      return StridedInterval::create_top(bitWidth);
    }
  }
  if (nsw) {
    APInt maxThis = this->smax();
    APInt maxOther = otherSI->smax();
    APInt minThis = this->smin();
    APInt minOther = otherSI->smin();
    bool ov;
    ///ignoring return value -> checking if overflow bit (ov) is being set
    (void) maxThis.sadd_ov(maxOther, ov);
    if (ov) {
      return StridedInterval::create_top(bitWidth);
    }
    ///ignoring return value -> checking if overflow bit (ov) is being set
    (void) minThis.sadd_ov(minOther, ov);
    if (ov) {
      return StridedInterval::create_top(bitWidth);
    }
  }
  APInt N(pow2(numBits, numBits + 1));
  APInt a(begin.zext(numBits + 1));
  APInt b(end.zext(numBits + 1));
  APInt s(stride.zext(numBits + 1));
  APInt c(otherSI->begin.zext(numBits + 1));
  APInt d(otherSI->end.zext(numBits + 1));
  APInt t(otherSI->stride.zext(numBits + 1));
  APInt u(GreatestCommonDivisor(s, t));
  APInt b_(a.ule(b) ? b : add_(b, N));
  APInt d_(c.ule(d) ? d : add_(d, N));
  APInt e(add_(a, c));
  APInt f(add_(b_, d_));
  APInt u_, e_, f_;
  if (sub_(f, e).ult(N)) {
    u_ = u;
    e_ = mod(e, N);
    f_ = mod(f, N);
  } else {
    u_ = GreatestCommonDivisor(u, N);
    e_ = mod(e, N);
    f_ = mod(sub_(e_, u_), N);
  }
  return StridedInterval(e_.zextOrTrunc(numBits), f_.zextOrTrunc(numBits),
                         u_.zextOrTrunc(numBits))
      .normalize();
}

shared_ptr<AbstractDomain> StridedInterval::sub(unsigned numBits,
    AbstractDomain &other, bool nuw, bool nsw) {
  StridedInterval *otherSI = static_cast<StridedInterval *>(&other);
  assert(numBits == bitWidth);
  assert(numBits == otherSI->bitWidth);
  if (this->isBottom() || otherSI->isBottom()) {
    return StridedInterval::create_bottom(bitWidth);
  }
  if (nuw) {
    APInt maxThis = this->smax();
    APInt maxOther = otherSI->smax();
    APInt minThis = this->smin();
    APInt minOther = otherSI->smin();
    bool ov;
    ///ignoring return value -> checking if overflow bit (ov) is being set
    (void) maxThis.usub_ov(minOther, ov);
    if (ov) {
      return StridedInterval::create_top(bitWidth);
    }
    ///ignoring return value -> checking if overflow bit (ov) is being set
    (void) minThis.usub_ov(maxOther, ov);
    if (ov) {
      return StridedInterval::create_top(bitWidth);
    }
  }
  if (nsw) {
    APInt maxThis = this->smax();
    APInt maxOther = otherSI->smax();
    APInt minThis = this->smin();
    APInt minOther = otherSI->smin();
    bool ov;
    ///ignoring return value -> checking if overflow bit (ov) is being set
    (void) maxThis.ssub_ov(minOther, ov);
    if (ov) {
      return StridedInterval::create_top(bitWidth);
    }
    ///ignoring return value -> checking if overflow bit (ov) is being set
    (void) minThis.ssub_ov(maxOther, ov);
    if (ov) {
      return StridedInterval::create_top(bitWidth);
    }
  }
  APInt N(pow2(numBits, numBits + 1));
  APInt a(begin.zext(numBits + 1));
  APInt b(end.zext(numBits + 1));
  APInt s(stride.zext(numBits + 1));
  APInt c(otherSI->begin.zext(numBits + 1));
  APInt d(otherSI->end.zext(numBits + 1));
  APInt t(otherSI->stride.zext(numBits + 1));
  APInt u(GreatestCommonDivisor(s, t));
  APInt b_(a.ule(b) ? b : add_(b, N));
  APInt d_(c.ule(d) ? d : add_(d, N));
  APInt e(sub_(a, d_));
  APInt f(sub_(b_, c));
  APInt u_, e_, f_;
  if (sub_(f, e).ult(N)) {
    u_ = u;
    e_ = e.trunc(numBits);
    f_ = f.trunc(numBits);
  } else {
    u_ = GreatestCommonDivisor(u, N).trunc(numBits);
    e_ = e.trunc(numBits);
    f_ = sub_(e_, u_);
  }
  return StridedInterval(e_.zextOrTrunc(numBits), f_.zextOrTrunc(numBits),
                         u_.zextOrTrunc(numBits))
      .normalize();
}

shared_ptr<AbstractDomain> StridedInterval::mul(unsigned numBits,
                                                AbstractDomain &other, bool nuw,
                                                bool nsw) {
  StridedInterval *otherSI = static_cast<StridedInterval *>(&other);
  assert(numBits == bitWidth);
  assert(numBits == otherSI->bitWidth);
  if (this->isBottom() || otherSI->isBottom()) {
    return StridedInterval::create_bottom(bitWidth);
  }
  if (nuw) {
    APInt maxThis = this->umax();
    APInt maxOther = otherSI->umax();
    bool ov;
    ///ignoring return value -> checking if overflow bit (ov) is being set
    (void) maxThis.umul_ov(maxOther, ov);
    if (ov) {
      return StridedInterval::create_top(bitWidth);
    }
  }
  if (nsw) {
    APInt maxThis = this->smax();
    APInt maxOther = otherSI->smax();
    APInt minThis = this->smin();
    APInt minOther = otherSI->smin();
    bool ov;
    ///ignoring return value -> checking if overflow bit (ov) is being set
    (void) maxThis.smul_ov(maxOther, ov);
    if (ov) {
      return StridedInterval::create_top(bitWidth);
    }
    ///ignoring return value -> checking if overflow bit (ov) is being set
    (void) maxThis.smul_ov(minOther, ov);
    if (ov) {
      return StridedInterval::create_top(bitWidth);
    }
    ///ignoring return value -> checking if overflow bit (ov) is being set
    (void) minThis.smul_ov(maxOther, ov);
    if (ov) {
      return StridedInterval::create_top(bitWidth);
    }
    ///ignoring return value -> checking if overflow bit (ov) is being set
    (void) minThis.smul_ov(minOther, ov);
    if (ov) {
      return StridedInterval::create_top(bitWidth);
    }
  }
  APInt N(pow2(numBits+1, 2*numBits));
  APInt a(begin.zext(2*numBits));
  APInt b(end.zext(2*numBits));
  APInt s(stride.zext(2*numBits));
  APInt c(otherSI->begin.zext(2*numBits));
  APInt d(otherSI->end.zext(2*numBits));
  APInt t(otherSI->stride.zext(2*numBits));
  APInt u(mul_(GreatestCommonDivisor(a, s), GreatestCommonDivisor(c, t)));
  APInt b_(a.ule(b) ? b : add_(b, N));
  APInt d_(c.ule(d) ? d : add_(d, N));
  APInt e(mul_(a, c));
  APInt f(mul_(b_, d_));
  APInt u_, e_, f_;
  if (sub_(f, e).ult(N)) {
    u_ = u.trunc(bitWidth);
    e_ = e.trunc(bitWidth) /* mod 2^n */;
    f_ = f.trunc(bitWidth) /* mod 2^n */;
  } else {
    u_ = GreatestCommonDivisor(u, N).trunc(bitWidth);
    e_ = e.trunc(bitWidth) /* mod 2^n */;
    f_ = sub_(e_, u_) /* mod 2^n */;
  }
  return StridedInterval(e_, f_, u_).normalize();
}

// *****************************************************
// * Everything below are incorrect dummy definitions! *
// *****************************************************

shared_ptr<AbstractDomain> StridedInterval::udiv(unsigned numBits,
                                                 AbstractDomain &other) {
  return StridedInterval(this->bitWidth, 0, 0, 0).normalize();
}
shared_ptr<AbstractDomain> StridedInterval::sdiv(unsigned numBits,
                                                 AbstractDomain &other) {
  return StridedInterval(this->bitWidth, 0, 0, 0).normalize();
}
shared_ptr<AbstractDomain> StridedInterval::urem(unsigned numBits,
                                                 AbstractDomain &other) {

  return StridedInterval(this->bitWidth, 0, 0, 0).normalize();
}
shared_ptr<AbstractDomain> StridedInterval::srem(unsigned numBits,
                                                 AbstractDomain &other) {
  return StridedInterval(this->bitWidth, 0, 0, 0).normalize();
}

shared_ptr<AbstractDomain> StridedInterval::shl(unsigned numBits,
                                                AbstractDomain &other, bool nuw,
                                                bool nsw) {
  return StridedInterval(this->bitWidth, 0, 0, 0).normalize();
}

shared_ptr<AbstractDomain> StridedInterval::lshr(unsigned numBits,
                                                 AbstractDomain &other) {
  return StridedInterval(this->bitWidth, 0, 0, 0).normalize();
}

shared_ptr<AbstractDomain> StridedInterval::ashr(unsigned numBits,
                                                 AbstractDomain &other) {
  return StridedInterval(this->bitWidth, 0, 0, 0).normalize();
}

shared_ptr<AbstractDomain> StridedInterval::and_(unsigned numBits,
                                                 AbstractDomain &other) {
  return StridedInterval(this->bitWidth, 0, 0, 0).normalize();
}

shared_ptr<AbstractDomain> StridedInterval::or_(unsigned numBits,
                                                AbstractDomain &other) {
  return StridedInterval(this->bitWidth, 0, 0, 0).normalize();
}

shared_ptr<AbstractDomain> StridedInterval::xor_(unsigned numBits,
                                                 AbstractDomain &other) {
  return StridedInterval(this->bitWidth, 0, 0, 0).normalize();
}

std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>
StridedInterval::subsetsForPredicate(
    AbstractDomain &other,
    CmpInst::Predicate pred) {
  if (StridedInterval *otherB = static_cast<StridedInterval *>(&other)) {

    if (isTop()) {
      if (pred == CmpInst::Predicate::ICMP_EQ) {
        shared_ptr<AbstractDomain> trueSet(new StridedInterval(*otherB));
        shared_ptr<AbstractDomain> falseSet(StridedInterval::create_top(this->bitWidth));
        return std::pair<shared_ptr<AbstractDomain>,
                         shared_ptr<AbstractDomain>>(trueSet, falseSet);
      } else if (pred == CmpInst::Predicate::ICMP_NE) {
        shared_ptr<AbstractDomain> trueSet(StridedInterval::create_top(this->bitWidth));
        shared_ptr<AbstractDomain> falseSet(new StridedInterval(*otherB));
        return std::pair<shared_ptr<AbstractDomain>,
                         shared_ptr<AbstractDomain>>{trueSet, falseSet};
      }
      
      // if this set is top, it will be top in both branches afterwards

      return std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>(
        StridedInterval::create_top(this->bitWidth),
        StridedInterval::create_top(this->bitWidth));
    }

    if (otherB->isTop()) {
      // if the other set is top; we cannot infer more details about our set
      // in both branches afterwards thus, the set stays the same
      shared_ptr<AbstractDomain> copy(new StridedInterval(*this));
      shared_ptr<AbstractDomain> anotherCopy(new StridedInterval(*this));
      return std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>(
          copy, anotherCopy);
    }
    
    if(pred == CmpInst::Predicate::ICMP_EQ){
      return subsetsForPredicateEQ(*this, *otherB);  
    } else if(pred == CmpInst::Predicate::ICMP_NE){
        auto temp = subsetsForPredicateEQ(*this, *otherB);
        return std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>(temp.second, temp.first);        
    } else if(pred == CmpInst::Predicate::ICMP_ULE ||
                    pred == CmpInst::Predicate::ICMP_SLE){    
        return subsetsForPredicateULE(*this, *otherB);
    } else if(pred == CmpInst::Predicate::ICMP_UGE ||
                    pred == CmpInst::Predicate::ICMP_SGE){
        auto temp = subsetsForPredicateULE(*this, *otherB);
        return std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>(temp.second, temp.first);
    }

  }
  
  return std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>(
      StridedInterval::create_top(this->bitWidth),
      StridedInterval::create_top(this->bitWidth));
}

std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>
StridedInterval::subsetsForPredicateEQ(
    StridedInterval &A, StridedInterval &B) {
  APInt maxBegin = A.begin.uge(B.begin) ? 
            A.begin : B.begin;
        
        APInt minEnd = A.end.ule(B.end) ? 
            A.end : B.end;
        
        if(maxBegin.ugt(minEnd)){
            // no intersection
            return std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>(
                  StridedInterval::create_bottom(A.bitWidth),
                  shared_ptr<AbstractDomain>(new StridedInterval(A)));
        } // else
        
        return std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>(
              shared_ptr<AbstractDomain>(new StridedInterval(maxBegin, minEnd, 
                        APInt(A.bitWidth, 1))),
              shared_ptr<AbstractDomain>(new StridedInterval(A)));
}

std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>
StridedInterval::subsetsForPredicateULE(
    StridedInterval &A, StridedInterval &B) {
        APInt maxBeginFalse = A.begin.uge(B.begin+1) ? 
            A.begin : (B.begin+1);
        
        APInt minEndTrue = A.end.ule(B.end) ? 
            A.end : B.end;
        
        shared_ptr<AbstractDomain> a1, a2;
                
        if(A.begin.ugt(minEndTrue)){
            a1 = StridedInterval::create_bottom(A.bitWidth);
        }else {
            a1 = shared_ptr<AbstractDomain>(new StridedInterval(
                    A.begin, minEndTrue, APInt(A.bitWidth, 1)));
        }
                
        if(maxBeginFalse.ugt(A.end)){
            a2 = StridedInterval::create_bottom(A.bitWidth);
        }else {
            a2 = shared_ptr<AbstractDomain>(new StridedInterval(
                    maxBeginFalse, A.end, APInt(A.bitWidth, 1)));
        }
        
        return std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>(
              a1, a2);
}

std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>
StridedInterval::icmp(CmpInst::Predicate pred, unsigned numBits,
                      AbstractDomain &other) {
  if (pred >= CmpInst::Predicate::ICMP_EQ &&
      pred <= CmpInst::Predicate::ICMP_SLE) {
    return subsetsForPredicate(other, pred);
  }
  return std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>(
      shared_ptr<AbstractDomain>(StridedInterval::create_top(this->bitWidth)),
      shared_ptr<AbstractDomain>(StridedInterval::create_top(this->bitWidth)));
}

size_t StridedInterval::size() const {
  if (isBottom()) {
    return 0;
  } else if (stride == 0) {
    return 1;
  } else {
    APInt d = sub_(this->end, this->begin);
    APInt res = div(d, this->stride).zext(bitWidth+1);
    res += 1;
    return res.getZExtValue();
  }
}

shared_ptr<AbstractDomain>
StridedInterval::leastUpperBound(AbstractDomain &other) {
  StridedInterval *otherMSI = static_cast<StridedInterval *>(&other);
  assert(otherMSI->bitWidth == bitWidth);
  if(*this == *otherMSI){
      return shared_ptr<AbstractDomain>(new StridedInterval(*this));
  } 
  if (isBot) {
    return std::shared_ptr<AbstractDomain>(new StridedInterval(*otherMSI)); // pm
  } else if (otherMSI->isBot) {
    return std::shared_ptr<AbstractDomain>(new StridedInterval(*this)); // pm  
  }
  APInt a = begin;
  APInt b = end;
  APInt s = stride;
  APInt c = otherMSI->begin;
  APInt d = otherMSI->end;
  APInt t = otherMSI->stride;
  APInt b_ (sub_(b, a) /* mod N */);
  APInt c_ (sub_(c, a) /* mod N */);
  APInt d_ (sub_(d, a) /* mod N */);
  StridedInterval res;
  if (b_.ult(c_) && c_.ult(d_)) { // no overlapping regions 
    APInt u1 = GreatestCommonDivisor(GreatestCommonDivisor(s, t), sub_(c, b));
    APInt e1 = a, f1 = d;
    APInt u2 = GreatestCommonDivisor(GreatestCommonDivisor(s, t), sub_(a, d));
    APInt e2 = c, f2 = b;
    StridedInterval opt1 (e1, f1, u1);
    StridedInterval opt2 (e2, f2, u2);
    if (opt1.size() < opt2.size()) { // choose the option representing the smallest set
      res = StridedInterval(e1, f1, u1);
    } else {
      res = StridedInterval(e2, f2, u2);
    }
  } else if (d_.ult(c_) and c_.ule(b_)) { // tow overlapping regions
    APInt u = GreatestCommonDivisor(
      GreatestCommonDivisor(s, t),
      GreatestCommonDivisor(c_.ule(d_) ? c_ : d_, pow2(bitWidth-1, bitWidth))
    );
    APInt e = mod(a, u);
    res = StridedInterval(e, sub_(e, u), u);
  } else { // one overlapping region
    APInt u = GreatestCommonDivisor(GreatestCommonDivisor(s, t), c_.ule(d_) ? c_ : d_);
    res = StridedInterval(c_.ule(d_) ? a : c, d_.ule(b_) ? b : d, u);
  }
  return res.normalize();
}

bool StridedInterval::lessOrEqual(AbstractDomain &other) {
  StridedInterval *otherMSI = static_cast<StridedInterval *>(&other);

  // pm: shortcut for both abstract domains are equal
  if(*this == *otherMSI){
      return true;
  }
  
  // pm: check for topness
  if(isTop()){
      if(other.isTop())
          return true;
      return false;
  }

  if (isBot) {
    return true;
  } else if (otherMSI->isBot) {
    return false;
  }
  if (otherMSI->bitWidth != bitWidth) {
  }
  assert(otherMSI->bitWidth == bitWidth);
  APInt a(begin);
  APInt b(end);
  APInt s(stride);
  APInt c(otherMSI->begin);
  APInt d(otherMSI->end);
  APInt t(otherMSI->stride);
  if (s.isNullValue()) {
    return otherMSI->contains(a);
  } else if (t == 0) {
    return false;
  } else if (b == add_(a, s)) /* mod N */ {
    return otherMSI->contains(a) && otherMSI->contains(b);
  } else if (mod(s, t).isNullValue()) {
    if (mod(pow2(bitWidth, bitWidth+1), t.zext(bitWidth+1)).isNullValue() && sub_(c, d) /* mod N */ == t) { // t | 2^n <=> 2**(n-1) = 0 mod t
      return mod(sub_(a.zext(bitWidth+1), c.zext(bitWidth+1)).trunc(bitWidth), t).isNullValue() && mod(s, t).isNullValue();
    } else {
      APInt b_ (sub_(b, a) /* mod N */);
      APInt c_ (sub_(c, a) /* mod N */);
      APInt d_ (sub_(d, a) /* mod N */);
      if (d_.ult(c_) && c_.ule(b_)) {
        APInt e_ = mul_(s, div(d_, s));
        APInt f_ = sub_(b_, mul_(s, div(sub_(b_, c_), s))) /* mod N */; // save since c_ < b_
        if (sub_(f_, e_) == s) { // e_ <= f_?
          if (e_.ult(s)) {
            if (otherMSI->contains(a) && mod(c_, t).isNullValue()) {
              return true;
            }
          } else if (otherMSI->contains(b) && mod(d_, t).isNullValue()) {
            return true;
          }
        }
      }
      if (c_.ule(d_)) {
        return c_.isNullValue() && b_.ule(d_);
      } else {
        return b_.ule(d_) && mod(sub_(d_, b_), t).isNullValue();
      }
    }
  } else {
    return false;
  }
}

bool StridedInterval::contains(APInt &value) const {
  assert(value.getBitWidth() == bitWidth);
  if (value.getBitWidth() != bitWidth) {
    return false;
  } else if (isBottom()) {
    return false;
  } else {
    if (begin.ule(end)) {
      if (value.ult(begin) || value.ugt(end)) {
        return false;
      } else {
        APInt offset(value);
        offset -= begin;
        APInt remainder = offset.urem(stride);
        return remainder.isNullValue();
      }
    } else { // begin > end
      if (value.ugt(end) && value.ult(begin)) {
        return false;
      } else {
        APInt offset(value);
        offset -= begin;
        APInt remainder = offset.urem(stride);
        return remainder.isNullValue();
      }
    }
  }
}
bool StridedInterval::isTop() const {
  if (isBot) {
    return false;
  } else {
    return stride == 1 && begin == 0 &&
           end == APInt::getMaxValue(this->getBitWidth());
  }
}

llvm::raw_ostream &StridedInterval::print(llvm::raw_ostream &os) {
  if (isBot) {
    os << "[]" << "_" << bitWidth;
  } else if(isTop()) {
    os << "T" << "_" << bitWidth;
  }
  else {
    os << stride << "[" << begin.toString(OUTPUT_BASE, OUTPUT_SIGNED) << ", "
       << end.toString(OUTPUT_BASE, OUTPUT_SIGNED) << "]_" << bitWidth;
  }
  return os;
}

void StridedInterval::printOut() const {
  errs() << "StridedInterval@" << this << "\n";
  if (isBot) {
    errs() << "[]\n";
    return;
  }
}

} // namespace pcpo
