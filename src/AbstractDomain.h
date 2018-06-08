#ifndef ABSTRACTDOMAIN_H_
#define ABSTRACTDOMAIN_H_

#include <memory>
#include "llvm/IR/InstrTypes.h"

namespace pcpo {

using namespace llvm;
using std::shared_ptr;

class AbstractDomain {
public:
    virtual ~AbstractDomain() = default;

  // Binary Arithmetic Operations
  virtual shared_ptr<AbstractDomain> add(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) = 0;
  virtual shared_ptr<AbstractDomain> sub(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) = 0;
  virtual shared_ptr<AbstractDomain> mul(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) = 0;
  virtual shared_ptr<AbstractDomain> udiv(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) = 0;
  virtual shared_ptr<AbstractDomain> sdiv(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) = 0;
  virtual shared_ptr<AbstractDomain> urem(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) = 0;
  virtual shared_ptr<AbstractDomain> srem(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) = 0;

  // Binary Bitwise Operations
  virtual shared_ptr<AbstractDomain> shl(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) = 0;
  virtual shared_ptr<AbstractDomain> lshr(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) = 0;
  virtual shared_ptr<AbstractDomain> ashr(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) = 0;
  virtual shared_ptr<AbstractDomain> and_(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) = 0;
  virtual shared_ptr<AbstractDomain> or_(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) = 0;
  virtual shared_ptr<AbstractDomain> xor_(unsigned numBits, AbstractDomain &other,
    bool nuw, bool nsw) = 0;

  // Conversion Operations (TODO?)

  // Other operations
  virtual std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>>
    icmp(CmpInst::Predicate pred, unsigned numBits, AbstractDomain &other) = 0;

  // Lattice interface
  virtual shared_ptr<AbstractDomain> leastUpperBound(AbstractDomain &other) = 0;
  virtual bool lessOrEqual(AbstractDomain &other) = 0;
  
  // |gamma(this)|
  virtual size_t size();

  // Debugging methodes
  virtual void printOut() = 0;
  // TODO: bitwise operations
};
} // namespace pcpo
#endif
