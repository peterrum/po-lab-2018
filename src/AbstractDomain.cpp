namespace pcpo {

class AbstractDomain {
public:
  virtual AbstractDomain add(AbstractDomain other);
  virtual AbstractDomain subtract(AbstractDomain other);
  virtual AbstractDomain multiply(AbstractDomain other);
  virtual AbstractDomain unaryMinus();
  virtual AbstractDomain increment();
  virtual AbstractDomain decrement();

  virtual AbstractDomain leastUpperBound(AbstractDomain other);
  virtual boolean lessOrEqual(AbstractDomain other);
  // TODO: bitwise operations
}
} // namespace pcpo
