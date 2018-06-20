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
    begin = APInt(bitWidth, 0);
    end = APInt::getMaxValue(bitWidth);
    stride = APInt(bitWidth, 1);
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
      stride = APInt(bitWidth, 0);
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
        diff = second;
        diff -= b;

        gcd = diff;

        // calculate stride
        for (size_t j = i % size; j % size != lastIndex;
             j = (j + 1) % size) {
          auto current = values.at(j);
          auto next = values.at((j + 1) % size);

          diff = next;
          diff -= current;
          gcd = GreatestCommonDivisor(gcd, diff);
        }

        // check whether interval starting at b has minimum number of elements
        StridedInterval tmp{b, e, gcd};
        if (tmp.size() <= min) {
          if(tmp.size() < set.size()) {
            continue;
          }
          min = tmp.size();
          minInterval = StridedInterval(tmp);
        }
      }
      isBot = false;
      begin = minInterval.begin;
      end = minInterval.end;
      stride = minInterval.stride;
    }

    std::shared_ptr<AbstractDomain> thisNorm = this->normalize();
    StridedInterval other = *(static_cast<StridedInterval *>(thisNorm.get()));
    begin = other.begin;
    end = other.end;
    stride = other.stride;
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

APInt mod(const APInt &a, const APInt &b) {
  return a.urem(b);
}

APInt neg(const APInt a) {
  APInt zero = APInt(a.getBitWidth(), 0);
  return sub_(zero, a);
}

APInt pow2(unsigned n, unsigned bitWidth) {
  assert(n <= bitWidth);
  APInt b{bitWidth, 0};
  b.setBit(n);
  return b;
}

APInt smax_(const APInt a, const APInt b) {
  return a.sge(b) ? a : b;
}

APInt umax_(const APInt a, const APInt b) {
  return a.uge(b) ? a : b;
}

APInt smin_(const APInt a, const APInt b) {
  return a.sle(b) ? a : b;
}

APInt sabs(const APInt a) {
  APInt zero = APInt(a.getBitWidth(), 0);
  return a.sge(zero) ? a : neg(a);
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

APInt StridedInterval::umax() const{
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

APInt StridedInterval::umin() const{
  APInt a = begin;
  APInt b = end;
  APInt s = stride;
  if (a.ule(b)) {
    return a;
  } else {
    return mod(b, s);
  }
}

APInt StridedInterval::smax() const{
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

APInt StridedInterval::smin() const{
  APInt a = begin;
  APInt b = end;
  APInt s = stride;
  if (a.sle(b)) {
    return a;
  } else {
    return sub_(mod(add_(b, pow2(bitWidth-1, bitWidth)), s), pow2(bitWidth-1, bitWidth));
    // APInt m = APInt::getSignedMinValue(bitWidth);
    // return add_(mod(sub_(b, m), s), m);
  }
}

APInt StridedInterval::ustride() const {
  APInt a = begin;
  APInt b = end;
  APInt s = stride;
  if (a.ule(b)) {
    return s;
  } else {
    return GreatestCommonDivisor(s, sub_(a, b));
  }
}

APInt StridedInterval::sstride() const {
  APInt a = begin;
  APInt b = end;
  APInt s = stride;
  if (a.sle(b)) {
    return s;
  } else {
    return GreatestCommonDivisor(s, sub_(a, b));
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

shared_ptr<AbstractDomain> StridedInterval::udiv(unsigned numBits,
                                                 AbstractDomain &other) {
  StridedInterval *otherSI = static_cast<StridedInterval *>(&other);
  assert(numBits == bitWidth);
  assert(numBits == otherSI->bitWidth);
  if (this->isBottom() || otherSI->isBottom()) {
    return StridedInterval::create_bottom(bitWidth);
  }
  APInt a = this->umin(); APInt  b = this->umax();
  APInt c = otherSI->umin(); APInt  d = otherSI->umax();
  APInt s = this->ustride();
  APInt t = otherSI->ustride();
  if (c == 0) { // handling possible division by 0
    if (t == 0) { // definite division by 0
      errs() << "ERROR: Input program includes division by zero.\n";
      errs() << "Exiting.\n";
      exit(EXIT_FAILURE);
    } else {
      errs() << "WARNING: Input program may include division by zero.\n";
      c = t; // exclude 0 from rhs
    }
  }
  StridedInterval res;
  if (t == 0) { // division by constant
    APInt s_ = GreatestCommonDivisor(a, s);
    APInt u = s_.udiv(c);
    u = mul_(u, c) == s ? u : APInt(bitWidth, 1);
    res = StridedInterval(a.udiv(c), b.udiv(c), u);
  } else { // general case
    res = StridedInterval(a.udiv(d), b.udiv(c), APInt(bitWidth, 1));
  }
  return res.normalize();
}

shared_ptr<AbstractDomain> StridedInterval::urem(unsigned numBits,
  AbstractDomain &other) {
  StridedInterval *otherSI = static_cast<StridedInterval *>(&other);
  assert(numBits == bitWidth);
  assert(numBits == otherSI->bitWidth);
  if (this->isBottom() || otherSI->isBottom()) {
    return StridedInterval::create_bottom(bitWidth);
  }
  APInt a = this->umin(); APInt  b = this->umax();
  APInt c = otherSI->umin(); APInt  d = otherSI->umax();
  APInt s = this->ustride();
  APInt t = otherSI->ustride();
  if (c == 0) { // handling possible division by 0
    if (t == 0) { // definite division by 0
      errs() << "ERROR: Input program includes remainder by zero.\n";
      errs() << "Exiting.\n";
      exit(EXIT_FAILURE);
    } else {
      errs() << "WARNING: Input program may include remainder by zero.\n";
      c = t; // exclude 0 from rhs
    }
  }
  StridedInterval res;
  if (b.ult(c)) { // urem has no effect
    res = *this;
  } else if (t == 0) { // division by constant
    if (a.udiv(c) == b.udiv(c)) { // all remainders are obtained by subtracting
                                  // the same value from lhs
      res = StridedInterval(mod(a, c), mod(b, c), s);
    } else {
      APInt u = GreatestCommonDivisor(s, c);
      res = StridedInterval(mod(a, u), c-1, u);
    }
  } else { // general case
    APInt u = GreatestCommonDivisor(GreatestCommonDivisor(c, t), s);
    res = StridedInterval(mod(a, u), APIntOps::umin(b, d-1), u);
  }
  return res.normalize();
}

shared_ptr<AbstractDomain> StridedInterval::srem(unsigned numBits,
    AbstractDomain &other) {
  StridedInterval *otherSI = static_cast<StridedInterval *>(&other);
  assert(numBits == bitWidth);
  assert(numBits == otherSI->bitWidth);
  if (this->isBottom() || otherSI->isBottom()) {
    return StridedInterval::create_bottom(bitWidth);
  }
  APInt zero = APInt(bitWidth+1, 0); APInt one = APInt(bitWidth+1, 1);
  APInt a = this->smin().sext(bitWidth+1); APInt  b = this->smax().sext(bitWidth+1);
  APInt c = otherSI->smin().sext(bitWidth+1); APInt  d = otherSI->smax().sext(bitWidth+1);
  APInt s = this->sstride().sext(bitWidth+1);
  APInt t = otherSI->sstride().sext(bitWidth+1);
  // check for remainder by zero and reduce remainder by negative rhs to
  // remainder negative of rhs
  if (d.slt(0)) {                      // rhs is definitly negative
    std::swap(c, d);
    c = neg(c); d = neg(d);
  } else if (c.slt(zero)) {            // rhs by be negative
    APInt d_ = d; APInt c_ = c;        // save values
    c = smin_(mod(neg(c_), t), mod(d_, t));
    d = smax_(neg(c_), d_);
    t = GreatestCommonDivisor(t, add_(c_, d_));
  }
  if (c == 0) {                        // remainder by bound not possible
    if (t == 0) {                      // definitly remainder by zero
      errs() << "ERROR: Input program includes remainder by zero.\n";
      errs() << "Exiting.\n";
      exit(EXIT_FAILURE);
    } else {                           // possibly remainer by zero
      errs() << "WARNING: Input program may include remainder by zero.\n";
      c = add_(c, t);                  // exclude zero from rhs
      if (c == d) {                    // renormalization neccessary
        t = zero;
      }
    }
  }
  APInt m = smax_(sabs(a), sabs(b));
  if (m.slt(c)) {                      // remainder has no effect
    return this->normalize();
  } else if (t == 0) {                 // remainder by constant
    if (a.sdiv(c) == b.sdiv(c)) {      // E x. x*rhs <= lhs < (x+1)*rhs
      return StridedInterval(
        a.srem(c).trunc(bitWidth), b.srem(c).trunc(bitWidth),
        s.trunc(bitWidth)
      ).normalize();
    }
  }
  APInt u = GreatestCommonDivisor(GreatestCommonDivisor(c, t), s);
  APInt e, f;
  if (zero.slt(a)) {
    e = mod(a, u);
  } else {
    e = smax_(a, add_(sub_(one, d), mod(sub_(add_(a, d), one), u)));
  }
  if (zero.slt(b)) {
    f = smin_(b, sub_(d, one));
  } else {
    f = add_(mod(sub_(e, one), u), sub_(one, u));
  }
  return StridedInterval(
    e.trunc(bitWidth), f.trunc(bitWidth),
    u.trunc(bitWidth)
  ).normalize();
}

// All operations below always return top, even if one of the operands are bottom.

shared_ptr<AbstractDomain> StridedInterval::sdiv(unsigned numBits,
    AbstractDomain &other) {
  return StridedInterval(this->bitWidth, 0, 0, 0).normalize();
}

shared_ptr<AbstractDomain> StridedInterval::shl(unsigned numBits,
  AbstractDomain &other, bool nuw, bool nsw) {
  return StridedInterval::create_top(numBits);
}

shared_ptr<AbstractDomain> StridedInterval::lshr(unsigned numBits,
    AbstractDomain &other) {
  return StridedInterval::create_top(numBits);
}

shared_ptr<AbstractDomain> StridedInterval::ashr(unsigned numBits,
    AbstractDomain &other) {
  return StridedInterval::create_top(numBits);
}

shared_ptr<AbstractDomain> StridedInterval::and_(unsigned numBits,
    AbstractDomain &other) {
  return StridedInterval::create_top(numBits);
}

shared_ptr<AbstractDomain> StridedInterval::or_(unsigned numBits,
    AbstractDomain &other) {
  return StridedInterval::create_top(numBits);
}

shared_ptr<AbstractDomain> StridedInterval::xor_(unsigned numBits,
    AbstractDomain &other) {
  return StridedInterval::create_top(numBits);
}

std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>
StridedInterval::subsetsForPredicate(AbstractDomain &other,
    CmpInst::Predicate pred) {
  StridedInterval *otherB = static_cast<StridedInterval *>(&other);

  if (otherB->isTop()) {
    // if the other set is top; we cannot infer more details about our set
    // in both branches afterwards thus, the set stays the same
    shared_ptr<AbstractDomain> copy(new StridedInterval(*this));
    shared_ptr<AbstractDomain> anotherCopy(new StridedInterval(*this));
    return std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>(
        copy, anotherCopy);
  }

  // We do a case distinction on the type of predicate
  // SGE will be reduced to SLE, UGT to SGT and so on.
  if (pred == CmpInst::Predicate::ICMP_EQ) {
    return subsetsForPredicateEQ(*this, *otherB);
  }
  if (pred == CmpInst::Predicate::ICMP_NE) {
    auto temp = subsetsForPredicateEQ(*this, *otherB);
    return std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>(
        temp.second, temp.first);
  }
  if (pred == CmpInst::Predicate::ICMP_SLE) {
    return subsetsForPredicateSLE(*this, *otherB);
  }
  if (pred == CmpInst::Predicate::ICMP_SGE) {
    auto temp = subsetsForPredicateSLT(*this, *otherB);
    return std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>(
        temp.second, temp.first);
  }
  if (pred == CmpInst::Predicate::ICMP_SLT) {
    return subsetsForPredicateSLT(*this, *otherB);
  }
  if (pred == CmpInst::Predicate::ICMP_SGT) {
    auto temp = subsetsForPredicateSLE(*this, *otherB);
    return std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>(
        temp.second, temp.first);
  }
  if (pred == CmpInst::Predicate::ICMP_ULE) {
    return subsetsForPredicateULE(*this, *otherB);
  }
  if (pred == CmpInst::Predicate::ICMP_UGE) {
    auto temp = subsetsForPredicateULT(*this, *otherB);
    return std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>(
        temp.second, temp.first);
  }
  if (pred == CmpInst::Predicate::ICMP_ULT) {
    return subsetsForPredicateULT(*this, *otherB);
  }
  if (pred == CmpInst::Predicate::ICMP_UGT) {
    auto temp = subsetsForPredicateULE(*this, *otherB);
    return std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>(
        temp.second, temp.first);
  }

  // In case we don't know the predicate, we return top in both cases
  return std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>(
      StridedInterval::create_top(this->bitWidth),
      StridedInterval::create_top(this->bitWidth));
}

std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>
StridedInterval::subsetsForPredicateEQ(StridedInterval &A, StridedInterval &B) {
  assert(A.bitWidth == B.bitWidth);

  shared_ptr<AbstractDomain> falseSet;
  shared_ptr<AbstractDomain> trueSet;

  if (B.size() == 1) {
    // In case B == {x}, we can exclude x from A in the false branch 

    // The set in the false branch thus is the intersection of A with the complement of B
    StridedInterval bComplement(B.begin + 1, B.begin - 1, APInt(B.bitWidth, 1));
    falseSet = intersectWithBounds(A, bComplement);
  } else {
    // if |B| > 1, we cannot exclue anything from the false branch
    falseSet = shared_ptr<AbstractDomain>(new StridedInterval(A));
  }
  // The values in A that can make a == b (a in A, b in B) true
  // are those in the intersection of A and B.
  trueSet = intersect(A, B);
  return std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>(
      trueSet, falseSet);
}

std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>
StridedInterval::subsetsForPredicateSLE(StridedInterval &A,
                                        StridedInterval &B) {
  assert(A.bitWidth == B.bitWidth);

  auto minSigned = APInt::getSignedMinValue(B.bitWidth);
  auto maxSigned = APInt::getSignedMaxValue(B.bitWidth);
  auto maxB = B.smax();
  auto minB = B.smin();

  // the subset of A that can be less or equal to some element in B
  auto trueSet =
      intersectWithBounds(A, StridedInterval(minSigned, maxB, APInt(B.bitWidth, 1)));
  // the subset of A that can be greater than some element in B
  auto falseSet =
      intersectWithBounds(A, StridedInterval(minB + 1, maxSigned, APInt(B.bitWidth, 1)));
  return std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>(
      trueSet, falseSet);
}

std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>
StridedInterval::subsetsForPredicateSLT(StridedInterval &A,
                                        StridedInterval &B) {
  assert(A.bitWidth == B.bitWidth);

  auto minSigned = APInt::getSignedMinValue(B.bitWidth);
  auto maxSigned = APInt::getSignedMaxValue(B.bitWidth);
  auto maxB = B.smax();
  auto minB = B.smin();

  // the subset of A that can be less to some element in B
  auto trueSet =
      intersectWithBounds(A, StridedInterval(minSigned, maxB - 1, APInt(B.bitWidth, 1)));
  // the subset of A that can be greater or equal to some element in B
  auto falseSet =
      intersectWithBounds(A, StridedInterval(minB, maxSigned, APInt(B.bitWidth, 1)));
  return std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>(
      trueSet, falseSet);
}

std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>
StridedInterval::subsetsForPredicateULE(StridedInterval &A,
                                        StridedInterval &B) {
  assert(A.bitWidth == B.bitWidth);

  auto minUnsigned = APInt::getMinValue(B.bitWidth);
  auto maxUnsigned = APInt::getMaxValue(B.bitWidth);
  auto maxB = B.umax();
  auto minB = B.umin();

  // the subset of A that can be less or equal to some element in B
  auto trueSet =
      intersectWithBounds(A, StridedInterval(minUnsigned, maxB, APInt(B.bitWidth, 1)));
  // the subset of A that can be greater than some element in B
  auto falseSet =
      intersectWithBounds(A, StridedInterval(minB + 1, maxUnsigned, APInt(B.bitWidth, 1)));
  return std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>(
      trueSet, falseSet);
}

std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>
StridedInterval::subsetsForPredicateULT(StridedInterval &A,
                                        StridedInterval &B) {
  assert(A.bitWidth == B.bitWidth);

  auto minUnsigned = APInt::getMinValue(B.bitWidth);
  auto maxUnsigned = APInt::getMaxValue(B.bitWidth);
  auto maxB = B.umax();
  auto minB = B.umin();

  // the subset of A that can be less to some element in B
  auto trueSet =
      intersectWithBounds(A, StridedInterval(minUnsigned, maxB - 1, APInt(B.bitWidth, 1)));
  // the subset of A that can be greater or equal to some element in B
  auto falseSet =
      intersectWithBounds(A, StridedInterval(minB, maxUnsigned, APInt(B.bitWidth, 1)));
  return std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>(
      trueSet, falseSet);
}

// A possibly overapproximated intersection of two StridedIntervals
shared_ptr<AbstractDomain> StridedInterval::intersect(const StridedInterval &first,
                                                      const StridedInterval &second) {
  assert(first.bitWidth == second.bitWidth);

  StridedInterval A(first);
  StridedInterval B(second);

  // if one of the SIs is bottom, the intersection is bottom, too
  if (A.isBottom() || B.isBottom()) {
    return create_bottom(A.bitWidth);
  }

  // if one the SIs is top, we return the other, so we don't overapproximate
  if (A.isTop()) {
    return shared_ptr<AbstractDomain>(new StridedInterval(B));
  }
  if (B.isTop()) {
    return shared_ptr<AbstractDomain>(new StridedInterval(A));
  }

  if(A.size() == 1){
    if (B.contains(A.begin)) {
      return shared_ptr<AbstractDomain>(new StridedInterval(A));
    } else {
      return create_bottom(A.bitWidth);
    }
  } 

  if(B.size() == 1){
    if(A.contains(B.begin)){
      return shared_ptr<AbstractDomain>(new StridedInterval(B));
    } else {
      return create_bottom(A.bitWidth);
    }
  } 

  // We do a case distinction on the kind of intervals
  // Case 1: both aren't wrap around
  if (!A.isWrapAround() && !B.isWrapAround()) {
    auto beginMax = A.begin.uge(B.begin) ? A.begin : B.begin;
    auto endMin = A.end.ule(B.end) ? A.end : B.end;

    if (beginMax.ugt(endMin)) {
      // We have no overlap in this case
      return create_bottom(A.bitWidth);
    } else {
      return shared_ptr<AbstractDomain>(
          new StridedInterval(beginMax, endMin, APInt(A.bitWidth, 1)));
    }
  }

  // Case 2: both are wrap around
  if (A.isWrapAround() && B.isWrapAround()) {
    // If both are wrap around, we always have an overlap
    auto beginMax = A.begin.uge(B.begin) ? A.begin : B.begin;
    auto endMin = A.end.ule(B.end) ? A.end : B.end;
    return shared_ptr<AbstractDomain>(
        new StridedInterval(beginMax, endMin, APInt(A.bitWidth, 1)));
  }

  // Case 3: B is wrap around, A is not
  // will be reduced to case 4
  if (B.isWrapAround()) {
    std::swap(A, B);
  }

  // Case 4: A is wrap around, B is not
  // Check if and where we have an overlap
  if (B.end.ult(A.begin) && B.begin.ugt(A.end)) {
    // We have no overlap
    return create_bottom(A.bitWidth);
  }
  if (B.begin.ule(A.end) && B.end.uge(A.begin)) {
    // We have an overlap at both ends
    // We return the interval that has fewer elements
    if (A.size() < B.size()) {
      return shared_ptr<AbstractDomain>(new StridedInterval(A));
    } else {
      return shared_ptr<AbstractDomain>(new StridedInterval(B));
    }
  }
  if (B.begin.ule(A.end)) {
    // We have an overlap at the left side
    auto endMin = A.end.ule(B.end) ? A.end : B.end;
    return shared_ptr<AbstractDomain>(
        new StridedInterval(B.begin, endMin, APInt(A.bitWidth, 1)));
  }
  // We have an overlap the the right side
  auto beginMax = A.begin.uge(B.begin) ? A.begin : B.begin;
  return shared_ptr<AbstractDomain>(
      new StridedInterval(beginMax, B.end, APInt(A.bitWidth, 1)));
}

// A possibly overapproximated intersection of two StridedIntervals
shared_ptr<AbstractDomain> StridedInterval::intersectWithBounds(const StridedInterval &first,
                                                      const StridedInterval &second) {
  assert(first.bitWidth == second.bitWidth);
  assert(second.stride == 1);

  StridedInterval A(first);
  StridedInterval B(second);

  // if one of the SIs is bottom, the intersection is bottom, too
  if (A.isBottom() || B.isBottom()) {
    return create_bottom(A.bitWidth);
  }

  // if one the SIs is top, we return the other, so we don't overapproximate
  if (A.isTop()) {
    return shared_ptr<AbstractDomain>(new StridedInterval(B));
  }
  if (B.isTop()) {
    return shared_ptr<AbstractDomain>(new StridedInterval(A));
  }

  if (A.size()==1){
    if(B.contains(A.begin)){
      return shared_ptr<AbstractDomain>(new StridedInterval(A));
    } else{
      return create_bottom(A.bitWidth);
    }
  }

  // We do a case distinction on the kind of intervals
  // Case 1: both aren't wrap around
  if (!A.isWrapAround() && !B.isWrapAround()) {
    auto beginMax = A.begin.uge(B.begin) ? A.begin : B.begin;
    auto endMin = A.end.ule(B.end) ? A.end : B.end;
    auto offset = beginMax;
    offset -= A.begin;
    offset = offset.urem(A.stride);

    auto resultingOffset = stride;
    resultingOffset -= offset;
    resultingOffset = resultingOffset.urem(A.stride);
    beginMax += resultingOffset;

    if (beginMax.ugt(endMin)) {
      // We have no overlap in this case
      return create_bottom(A.bitWidth);
    } else {
      return StridedInterval(beginMax, endMin, A.stride).normalize();
    }
  }

  // Case 2: both are wrap around
  if (A.isWrapAround() && B.isWrapAround()) {
    // If both are wrap around, we always have an overlap
    auto beginMax = A.begin.uge(B.begin) ? A.begin : B.begin;
    auto endMin = A.end.ule(B.end) ? A.end : B.end;
    return StridedInterval(beginMax, endMin, APInt(A.bitWidth, 1)).normalize();
  }

  // Case 3: B is wrap around, A is not
  // will be reduced to case 4
  if (B.isWrapAround()) {
    std::swap(A, B);
  }

  // Case 4: A is wrap around, B is not
  // Check if and where we have an overlap
  if (B.end.ult(A.begin) && B.begin.ugt(A.end)) {
    // We have no overlap
    return create_bottom(A.bitWidth);
  }
  if (B.begin.ule(A.end) && B.end.uge(A.begin)) {
    // We have an overlap at both ends
    // We return the interval that has fewer elements
    if (A.size() < B.size()) {
      return shared_ptr<AbstractDomain>(new StridedInterval(A));
    } else {
      return shared_ptr<AbstractDomain>(new StridedInterval(B));
    }
  }
  if (B.begin.ule(A.end)) {
    // We have an overlap at the left side
    auto endMin = A.end.ule(B.end) ? A.end : B.end;
    return StridedInterval(B.begin, endMin, APInt(A.bitWidth, 1)).normalize();
  }
  // We have an overlap the the right side
  auto beginMax = A.begin.uge(B.begin) ? A.begin : B.begin;
  return StridedInterval(beginMax, B.end, APInt(A.bitWidth, 1)).normalize();
}

bool StridedInterval::isWrapAround() const{
  if(isBottom()){
    return false;
  } else {
    // if begin > end this is a wrap around interval
    return begin.ugt(end);
  }
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
    APInt res = d.udiv(this->stride).zext(bitWidth+1);
    res += 1;
    return res.getZExtValue();
  }
}

shared_ptr<AbstractDomain>
StridedInterval::leastUpperBound(AbstractDomain &other) {
  // get other
  StridedInterval *otherMSI = static_cast<StridedInterval *>(&other);

  // LUB of things with differet bitWidth is not defined
  assert(otherMSI->bitWidth == bitWidth);

  if(*this == *otherMSI){
      // operands equal
      return shared_ptr<AbstractDomain>(new StridedInterval(*this));
  }

  if (isBot) {
    // if this is bot, LUB is other
    return std::shared_ptr<AbstractDomain>(new StridedInterval(*otherMSI));
  } else if (otherMSI->isBot) {
    // if other is bot, LUB is this
    return std::shared_ptr<AbstractDomain>(new StridedInterval(*this));
  }

  if (isTop()) {
    // if this is top, LUB is top
    return std::shared_ptr<AbstractDomain>(new StridedInterval(*this));
  } else if (otherMSI->isTop()) {
    // if other is top, LUB is other
    return std::shared_ptr<AbstractDomain>(new StridedInterval(*otherMSI));
  }

  /// To simplify cases we assume that |other| >= |this|
  if(otherMSI->size() < size()) {
    return otherMSI->leastUpperBound(*this);
  }

  APInt a = begin;
  APInt b = end;
  APInt s = stride;
  APInt c = otherMSI->begin;
  APInt d = otherMSI->end;
  APInt t = otherMSI->stride;

  /// If an interval of two elements is represented with a stride > 2^(bitWidth-1)
  /// we convert to non-normalized representation to get tighter intervals later
  if(add_(a,s) == b && s.ugt(pow2(bitWidth-1, bitWidth))) {
    std::swap(a,b);
    s = sub_(APInt(bitWidth,0),s);
  }

  /// If an interval of two elements is represented with a stride > 2^(bitWidth-1)
  /// we convert to non-normalized representation to get tighter intervals later
  if(add_(c,t) == d && t.ugt(pow2(bitWidth-1, bitWidth))) {
    std::swap(c,d);
    t = sub_(APInt(bitWidth,0),t);
  }

  // shift both intervals to the left by `a`, so fewer cases need to be considered
  APInt b_ (sub_(b, a) /* mod N */);
  APInt c_ (sub_(c, a) /* mod N */);
  APInt d_ (sub_(d, a) /* mod N */);

  if (b_.ult(c_) && c_.ult(d_)) { // no overlapping regions
    APInt u1 = GreatestCommonDivisor(GreatestCommonDivisor(s, t), sub_(c, b));
    APInt e1 = a, f1 = d;
    APInt u2 = GreatestCommonDivisor(GreatestCommonDivisor(s, t), sub_(a, d));
    APInt e2 = c, f2 = b;
    StridedInterval opt1 (e1, f1, u1);
    StridedInterval opt2 (e2, f2, u2);

    // choose the option representing the smallest set
    StridedInterval smaller = opt1.size() < opt2.size()?opt1:opt2;
    return smaller.normalize();
  }

  if (d_.ult(c_) and c_.ule(b_)) {
    // two overlapping regions
    APInt distanceFromStartOfA = c_.ule(d_) ? c_ : d_;
    // to ensure that the stride behaves well for wraparounds
    APInt wrapAround = pow2(bitWidth-1, bitWidth);

    APInt u = GreatestCommonDivisor(
      GreatestCommonDivisor(s, t),
      GreatestCommonDivisor(distanceFromStartOfA, wrapAround)
    );
    APInt e = mod(a, u); // could be any member, shift as far left as possible
    APInt f = sub_(e, u); // last member to the left
    return StridedInterval(e, f, u).normalize();
  }

  // one overlapping region
  APInt u = GreatestCommonDivisor(GreatestCommonDivisor(s, t), c_.ule(d_) ? c_ : d_);
  return StridedInterval(c_.ule(d_) ? a : c, d_.ule(b_) ? b : d, u).normalize();
}

bool StridedInterval::operator<=(AbstractDomain &other) {
  StridedInterval *otherMSI = static_cast<StridedInterval *>(&other);

  /// shortcut for both abstract domains are equal
  if(*this == *otherMSI){
      return true;
  }

  /// check for topness
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
        APInt e_ = mul_(s, d_.udiv(s));
        APInt f_ = sub_(b_, mul_(s, sub_(b_, c_).udiv(s))) /* mod N */; // save since c_ < b_
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

  if (value.getBitWidth() != bitWidth)
    return false;

  if (isBottom())
    return false;

  if(isTop())
    return true;

  if (begin.ule(end)) {
    // normal intervals
    if (value.ult(begin) || value.ugt(end)) {
      // value outside of interval bounds
      return false;
    }
  } else {
    // wrapAround intervals begin > end
    if (value.ugt(end) && value.ult(begin)) {
      // value outside of interval bounds
      return false;
    }
  }

  // value inside of interval bounds
  // check if it agrees with stride

  /// Singleton
  if(size() == 1) return true;

  APInt offset(value);
  // subtract interval start
  offset -= begin;

  APInt remainder = offset.urem(stride);
  return remainder.isNullValue();
}

bool StridedInterval::isTop() const {
  if (isBot) {
    return false;
  } else {
    return stride == 1 && begin == 0 &&
           end == APInt::getMaxValue(this->getBitWidth());
  }
}

shared_ptr<AbstractDomain> StridedInterval::widen() {
  /// This is where we should look at smarter ways to do this...
  return create_top(bitWidth);
}

bool StridedInterval::requiresWidening() {
  // This AD requires widening to ensure speedy termination
  return true;
}


llvm::raw_ostream &StridedInterval::print(llvm::raw_ostream &os) {
  if (isBot) {
    os << "[]" << "_" << bitWidth;
  } else if(isTop()) {
    os << "T" << "_" << bitWidth;
  }
  else {
    os << stride.toString(10, false) << "[" << begin.toString(10, false) << ", "
       << end.toString(10, false) << "]_" << bitWidth;
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
