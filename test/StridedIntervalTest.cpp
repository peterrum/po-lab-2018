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

void testStridedIntervalLessOrEqual() {
  StridedInterval lhs, rhs;
  lhs = {4, 14, 14, 0}; rhs = {4, 1, 3, 1};
  if (lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed: case 0 False\n";
  }
  lhs = {4, 14, 14, 0}; rhs = {4, 5, 3, 1};
  if (!lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed: case 0 True\n";
  }

  lhs = {4, 1, 3, 1}; rhs = {4, 14, 14, 0};
  if (lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed: case 1 False\n";
  }

  lhs = {4, 10, 13, 3}; rhs = {4, 1, 3, 1};
  if (lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed: case 2 False\n";
  }
  lhs = {4, 10, 13, 3}; rhs = {4, 5, 3, 1};
  if (!lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed: case 2 True\n";
  }

  lhs = {4, 1, 15, 2}; rhs = {4, 0, 14, 2};
  if (lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed: case 3, 0 False\n";
  }
  lhs = {4, 1, 3, 1}; rhs = {4, 0, 15, 1};
  if (!lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed: case 3, 0 True\n";
  }

  lhs = {4, 9, 3, 5}; rhs = {4, 14, 11, 1};
  if (!lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed: case 3, 1, 0, 0, 0 True\n";
  }

  lhs = {4, 9, 3, 5}; rhs = {4, 1, 14, 1};
  if (!lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed: case 3, 1, 0, 0, 0 False\n";
  }

  lhs = {4, 1, 3, 1}; rhs = {4, 9, 11, 1};
  if (lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed: case 3, 1, 1 False\n";
  }
  // lhs = {4, 1, 3, 1}; rhs = {4, 3, 1, 1};
  // if (!lhs.lessOrEqual(rhs)) {
  //   errs() << "[testLessOrEqual] failed: case 3, 1, 1 True\n";
  // }

  lhs = {4, 1, 3, 1}; rhs = {4, 9, 1, 1};
  if (lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed: case 3, 1, 2 False\n";
  }
  lhs = {4, 1, 3, 1}; rhs = {4, 5, 3, 1};
  if (!lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed: case 3, 1, 2 True\n";
  }

  // lhs = {4, 1, 3, 1}; rhs = {4, 9, 3, 5};
  // if (!lhs.lessOrEqual(rhs)) {
  //   errs() << "[testLessOrEqual] failed: case 4 False\n";
  // }
  // lhs = {4, 14, 14, 0}; rhs = {4, 1, 3, 1};
  // if (!lhs.lessOrEqual(rhs)) {
  //   errs() << "[testLessOrEqual] failed: case 1\n";
  // }
}

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
  testStridedIntervalLessOrEqual();
  testStridedIntervalAdd();
  testContains();
}
} // namespace pcpo
