#include "../src/abstract_domain/CompositeDomain.h"
#include "llvm/Support/raw_os_ostream.h"
#include <memory>
#include <vector>

using namespace llvm;
using std::shared_ptr;
using std::vector;

namespace pcpo {

void testCompositeDomainLeastUpperBound() {
  CompositeDomain a{APInt{32, 0}};

  shared_ptr<AbstractDomain> comp{new CompositeDomain{APInt{32, 0}}};

  for (unsigned i = 0; i < 100; i ++) {
    CompositeDomain tmp{APInt{32, (i+1)*10}};
    comp = comp->leastUpperBound(tmp);
    errs() << *comp.get() << "\n";
  }
  errs() << *comp.get() << "\n";
}


void testCompositeDomainAdd() {
  shared_ptr<AbstractDomain> a{new CompositeDomain{APInt{32, 10}}};
  shared_ptr<AbstractDomain> comp{new CompositeDomain{APInt{32, 0}}};
  a = a->leastUpperBound(*comp.get());
  comp = comp->leastUpperBound(*a.get());
  for (unsigned i = 0; i < 50; i++) {
    comp = comp->add(32, *a.get(), false, false);
    errs() << *comp.get() << "\n";
  }
  errs() << *comp.get() << "\n";
}


void testSIConstructorFromBS(){
  BoundedSet bs{32, {0,10}};
  StridedInterval si{bs};
  errs() << si << "\n";
}

void runCompositeDomain() {
//   testCompositeDomainLeastUpperBound();
  testCompositeDomainAdd();
  testSIConstructorFromBS();
}
}