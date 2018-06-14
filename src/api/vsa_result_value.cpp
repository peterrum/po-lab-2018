#include <llvm/Analysis/LazyValueInfo.h>

#include "vsa_result_value.h"
#include "llvm/IR/Constants.h"
using namespace llvm;

namespace pcpo {

LazyValueInfo::Tristate VsaResultValue::testIf(CmpInst::Predicate predicate,
                                               Constant *C) const{

  // only comparison with ConstantInt is implemented
  if (ConstantInt::classof(C)) {
    // create abstract domain for constant
    auto temp = AD_TYPE(reinterpret_cast<ConstantInt *>(C)->getValue());

    // perform comparison
    auto result = abstractValue->icmp(predicate,
                                      C->getType()->getIntegerBitWidth(), temp);

    // temp. bools
    bool b_t = !result.first->isBottom();  // predicate might be true
    bool b_f = !result.second->isBottom(); // predicate might not be true

    if (b_t && b_f) // predicate might be true or not
      return LazyValueInfo::Unknown;
    if (b_f) // predicate is not true
      return LazyValueInfo::False;
    else // predicate is true
      return LazyValueInfo::True;
  } else { // not implemented: top
    return LazyValueInfo::Unknown;
  }
}

bool VsaResultValue::isTop() const{
  return abstractValue->isTop();
}

bool VsaResultValue::isConstant() const{
  return abstractValue->size()==1;
}

APInt VsaResultValue::getConstant() const{
  assert(isConstant() && "getConstant failed: non Constant Value");
  return getValueAt(0);
}

APInt VsaResultValue::getNumValues() const{
  assert(!abstractValue->isTop() && " called getNumValues() on T");

  const auto size = abstractValue->size();
  return APInt(sizeof(size)*8,size);
}

APInt VsaResultValue::getValueAt(uint64_t i) const{
  assert(!abstractValue->isTop() && " called getValueAt() on T");
  return abstractValue->getValueAt(i);
}

APInt VsaResultValue::getUMin() const {
  assert(!abstractValue->isTop() && " called getUMin() on T");
  return abstractValue->getUMin();
}
APSInt VsaResultValue::getSMin() const {
  assert(!abstractValue->isTop() && " called getSMin() on T");
  return abstractValue->getSMin();
}
APInt VsaResultValue::getUMax() const{
  assert(!abstractValue->isTop() && " called getUMax() on T");
  return abstractValue->getUMax();
}
APSInt VsaResultValue::getSMax() const{
  assert(!abstractValue->isTop() && " called getSMax() on T");
  return abstractValue->getSMax();
}
}
