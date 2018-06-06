#ifndef ABSTRACTDOMAIN_H_
#define ABSTRACTDOMAIN_H_

#include <memory>

namespace pcpo {

using std::unique_ptr;

class AbstractDomain {
public:
  virtual unique_ptr<AbstractDomain> add(AbstractDomain &other) = 0;
  virtual unique_ptr<AbstractDomain> subtract(AbstractDomain &other) = 0;
  virtual unique_ptr<AbstractDomain> multiply(AbstractDomain &other) = 0;
  virtual unique_ptr<AbstractDomain> unaryMinus() = 0;
  virtual unique_ptr<AbstractDomain> increment() = 0;
  virtual unique_ptr<AbstractDomain> decrement() = 0;

  virtual unique_ptr<AbstractDomain> leastUpperBound(AbstractDomain &other) = 0;
  virtual bool lessOrEqual(AbstractDomain &other) = 0;

  // TODO: bitwise operations
};
} // namespace pcpo
#endif
