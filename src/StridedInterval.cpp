#include "StridedInterval.h"
#include "AbstractDomain.h"
#include "llvm/ADT/APInt.h"
#include "llvm/Support/raw_os_ostream.h"
#include <initializer_list>
#include <iostream>
#include <iterator>

namespace pcpo {
using llvm::APInt;
using llvm::APIntOps::GreatestCommonDivisor;

StridedInterval::StridedInterval(APInt begin, APInt end, APInt stride)
    : bitWidth(begin.getBitWidth()), begin(begin), end(end), stride(stride),
      isBot(false) {
  assert(end.getBitWidth() == bitWidth);
  assert(stride.getBitWidth() == bitWidth);
}

StridedInterval::StridedInterval(APInt value)
    : bitWidth(value.getBitWidth()), begin(value), end(value),
      stride({value.getBitWidth(), 0}), isBot{false} {}

StridedInterval::StridedInterval() : isBot(true) {}

StridedInterval::StridedInterval(unsigned bitWidth, uint64_t begin,
    uint64_t end, uint64_t stride)
    : bitWidth(bitWidth), begin(APInt(bitWidth, begin)),
      end(APInt(bitWidth, end)), stride(APInt(bitWidth, stride)),
      isBot(false) {}

bool StridedInterval::operator==(const StridedInterval &other) {
  if (this->isBot) {
    return other.isBot;
  } else {
    return this->begin == other.begin && this->end == other.end &&
           this->stride == other.stride;
  }
}

bool StridedInterval::operator!=(const StridedInterval &other) {
  return !(this->operator==(other));
}

APInt add_(const APInt &a, const APInt &b) {
  APInt c = a;
  c += b;
  return b;
}

APInt sub_(const APInt &a, const APInt &b) {
  APInt c = a;
  c -= b;
  return b;
}

APInt mul_(const APInt &a, const APInt &b) {
  APInt c = a;
  c *= b;
  return b;
}

APInt mod(const APInt &a, const APInt &b) { return a.urem(b); }

APInt div(const APInt &a, const APInt &b) { return a.udiv(b); }

APInt pow2(unsigned n, int a) {
  APInt b{n, 0};
  b.setBit(n);
  return b;
}

std::shared_ptr<AbstractDomain> StridedInterval::normalize() {
  unsigned n{bitWidth};
  APInt N{pow2(n + 1, n)};
  APInt a{begin.zext(n + 1)};
  APInt b{end.zext(n + 1)};
  APInt s{stride.zext(n + 1)};
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
  return std::shared_ptr<AbstractDomain>{new StridedInterval(
      a.zextOrTrunc(n), b.zextOrTrunc(n), s.zextOrTrunc(n))};
}

shared_ptr<AbstractDomain> StridedInterval::add(unsigned numBits,
                                                AbstractDomain &other, bool nuw,
                                                bool nsw) {
  StridedInterval *otherB = static_cast<StridedInterval *>(&other);
  assert(numBits == bitWidth);
  assert(numBits == otherB->bitWidth);
  APInt N{pow2(numBits+1, numBits)};
  APInt a{begin.zext(numBits + 1)};
  APInt b{end.zext(numBits + 1)};
  APInt s{stride.zext(numBits + 1)};
  APInt c{otherB->begin.zext(numBits + 1)};
  APInt d{otherB->end.zext(numBits + 1)};
  APInt t{otherB->stride.zext(numBits + 1)};
  APInt u{GreatestCommonDivisor(s, t)};
  APInt b_{a.ule(b) ? b : add_(b, N)};
  APInt d_{c.ule(d) ? d : add_(d, N)};
  APInt e{add_(a, c)};
  APInt f{add_(b_, d_)};
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

// *****************************************************
// * Everything below are incorrect dummy definitions! *
// *****************************************************

shared_ptr<AbstractDomain> StridedInterval::sub(unsigned numBits,
                                                AbstractDomain &other, bool nuw,
                                                bool nsw) {
  return StridedInterval(this->bitWidth, 0, 0, 0).normalize();
}

shared_ptr<AbstractDomain> StridedInterval::mul(unsigned numBits,
                                                AbstractDomain &other, bool nuw,
                                                bool nsw) {
  return StridedInterval(this->bitWidth, 0, 0, 0).normalize();
}

shared_ptr<AbstractDomain> StridedInterval::udiv(unsigned numBits,
                                                 AbstractDomain &other,
                                                 bool nuw, bool nsw) {
  return StridedInterval(this->bitWidth, 0, 0, 0).normalize();
}
shared_ptr<AbstractDomain> StridedInterval::sdiv(unsigned numBits,
                                                 AbstractDomain &other,

                                                 bool nuw, bool nsw) {
  return StridedInterval(this->bitWidth, 0, 0, 0).normalize();
}
shared_ptr<AbstractDomain> StridedInterval::urem(unsigned numBits,
                                                 AbstractDomain &other,
                                                 bool nuw, bool nsw) {

  return StridedInterval(this->bitWidth, 0, 0, 0).normalize();
}
shared_ptr<AbstractDomain> StridedInterval::srem(unsigned numBits,
                                                 AbstractDomain &other,
                                                 bool nuw, bool nsw) {
  return StridedInterval(this->bitWidth, 0, 0, 0).normalize();
}

shared_ptr<AbstractDomain> StridedInterval::shl(unsigned numBits,
                                                AbstractDomain &other, bool nuw,
                                                bool nsw) {
  return StridedInterval(this->bitWidth, 0, 0, 0).normalize();
}

shared_ptr<AbstractDomain> StridedInterval::lshr(unsigned numBits,
                                                 AbstractDomain &other,
                                                 bool nuw, bool nsw) {
  return StridedInterval(this->bitWidth, 0, 0, 0).normalize();
}

shared_ptr<AbstractDomain> StridedInterval::ashr(unsigned numBits,
                                                 AbstractDomain &other,
                                                 bool nuw, bool nsw) {
  return StridedInterval(this->bitWidth, 0, 0, 0).normalize();
}

shared_ptr<AbstractDomain> StridedInterval::and_(unsigned numBits,
                                                 AbstractDomain &other,
                                                 bool nuw, bool nsw) {
  return StridedInterval(this->bitWidth, 0, 0, 0).normalize();
}

shared_ptr<AbstractDomain> StridedInterval::or_(unsigned numBits,
                                                AbstractDomain &other, bool nuw,
                                                bool nsw) {
  return StridedInterval(this->bitWidth, 0, 0, 0).normalize();
}

shared_ptr<AbstractDomain> StridedInterval::xor_(unsigned numBits,
                                                 AbstractDomain &other,
                                                 bool nuw, bool nsw) {
  return StridedInterval(this->bitWidth, 0, 0, 0).normalize();
}

std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>
StridedInterval::icmp(CmpInst::Predicate pred, unsigned numBits,
                      AbstractDomain &other) {
  return std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>(
      shared_ptr<AbstractDomain>(new StridedInterval(this->bitWidth, 0, 0, 0)),
      shared_ptr<AbstractDomain>(new StridedInterval(this->bitWidth, 0, 0, 0)));
}

size_t StridedInterval::size() const { return 0; }

shared_ptr<AbstractDomain>
StridedInterval::leastUpperBound(AbstractDomain &other) {
  return StridedInterval(this->bitWidth, 0, 0, 0).normalize();
}

bool StridedInterval::lessOrEqual(AbstractDomain &other) { return false; }

unsigned StridedInterval::getBitWidth() const {
  return begin.getBitWidth();
}

bool StridedInterval::isTop() const {
  if (isBot) {
    return false;
  } else {
    return stride == 1 && begin == 0 && end == APInt::getMaxValue(this->getBitWidth());
  }
}

bool StridedInterval::isBottom() const {
  return isBot;
}

llvm::raw_ostream &StridedInterval::print(llvm::raw_ostream &os) {
  if (isBot) {
    os << "[]";
  } else {
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
