namespace pcpo {

class AbstractDomain {
public:
  virtual AbstractDomain add(AbstractDomain other) = 0;
  virtual AbstractDomain subtract(AbstractDomain other) = 0;
  virtual AbstractDomain multiply(AbstractDomain other) = 0;
  virtual AbstractDomain unaryMinus() = 0;
  virtual AbstractDomain increment() = 0;
  virtual AbstractDomain decrement() = 0;

  virtual AbstractDomain leastUpperBound(AbstractDomain other) = 0;
  virtual boolean lessOrEqual(AbstractDomain other) = 0;

  // TODO: bitwise operations
}
} // namespace pcpo
