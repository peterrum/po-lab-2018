#include "../src/abstract_domain/StridedInterval.h"
#include "llvm/Support/raw_os_ostream.h"
#include <set>
using namespace llvm;
namespace pcpo {

// void testStridedIntervalNormalize() {
//   StridedInterval si1 {4, 2, 8, 3};
//   StridedInterval ref {4, 3, 11, 1};
//   auto result = si1.add(4, si2, false, false);
//   StridedInterval res = *(static_cast<StridedInterval *>(result.get()));
//   if (res != ref) {
//     errs() << "[testAdd] failed: got " << res << ", expected " << ref <<
//     "\n";
//   }
// }

void testStridedIntervalAdd() {
  StridedInterval si1{4, 2, 8, 3};
  StridedInterval si2{4, 1, 3, 2};
  StridedInterval ref{4, 3, 11, 1};
  auto result = si1.add(4, si2, false, false);
  StridedInterval res = *(static_cast<StridedInterval *>(result.get()));
  if (res != ref) {
    errs() << "[testAdd] failed: got " << res << ", expected " << ref << "\n";
  }
}

void testContains() {
  APInt begin{5, 15};
  APInt end{5, 5};
  APInt stride{5, 2};
  StridedInterval si{begin, end, stride};

  for (unsigned i = 0; i < 31; i++) {
    APInt testValue{5, i};
    errs() << testValue.toString(10, false) << " is contained "
           << (si.contains(testValue) ? "true" : "false") << "\n";
  }
}

void runStridedInterval() {
  testStridedIntervalAdd();
  testContains();
}
} // namespace pcpo
