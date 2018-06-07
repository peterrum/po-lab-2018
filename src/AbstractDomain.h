#ifndef ABSTRACTDOMAIN_H_
#define ABSTRACTDOMAIN_H_

#include <memory>

namespace pcpo {

using std::shared_ptr;

class AbstractDomain {
public:
    public:
        explicit AbstractDomain();
        AbstractDomain(const AbstractDomain&) = delete;
        AbstractDomain& operator=(const AbstractDomain&) = delete;
        virtual ~AbstractDomain() = default;
    
  virtual shared_ptr<AbstractDomain> add(AbstractDomain &other) = 0;
  virtual shared_ptr<AbstractDomain> sub(AbstractDomain &other) = 0;
  virtual shared_ptr<AbstractDomain> mul(AbstractDomain &other) = 0;

  virtual shared_ptr<AbstractDomain> leastUpperBound(AbstractDomain &other) = 0;
  virtual bool lessOrEqual(AbstractDomain &other) = 0;
  virtual void printOut() = 0;
  // TODO: bitwise operations
};
} // namespace pcpo
#endif
