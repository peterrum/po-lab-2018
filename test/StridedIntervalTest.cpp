#include "../src/StridedInterval.h"
#include "llvm/Support/raw_os_ostream.h"
#include <set>
using namespace llvm;
namespace pcpo {

void testStridedIntervalAdd() {
  StridedInterval si1 {4, 2, 8, 3};
  StridedInterval si2 {4, 1, 3, 2};
  StridedInterval ref {4, 3, 11, 1};
  auto result = si1.add(4, si2, false, false);
  StridedInterval res = *(static_cast<StridedInterval *>(result.get()));
  if (res != ref) {
    errs() << "[testAdd] failed: got " << res << ", expected " << ref << "\n";
  }
}

void runStridedInterval() {
  testStridedIntervalAdd();
}
} // namespace pcpo
