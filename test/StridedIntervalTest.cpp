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
  lhs = {1, 1, 1, 0}; rhs = {1, 0, 1, 1};
  if (!lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 0[1, 1]_{1}, 1[0, 1]_{1}\n";
  }

  lhs = {1, 1, 1, 0}; rhs = {1, 0, 0, 0};
  if (lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 0[1, 1]_{1}, 0[0, 0]_{1}\n";
  }

  lhs = {1, 0, 1, 1}; rhs = {1, 1, 1, 0};
  if (lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 1[0, 1]_{1}, 0[1, 1]_{1}\n";
  }

  lhs = {1, 0, 1, 1}; rhs = {1, 0, 1, 1};
  if (!lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 1[0, 1]_{1}, 1[0, 1]_{1}\n";
  }

  lhs = {2, 0, 1, 1}; rhs = {2, 0, 3, 3};
  if (lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 1[0, 1]_{2}, 3[0, 3]_{2}\n";
  }

  lhs = {2, 0, 1, 1}; rhs = {2, 1, 3, 2};
  if (lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 1[0, 1]_{2}, 2[1, 3]_{2}\n";
  }

  lhs = {2, 0, 2, 1}; rhs = {2, 0, 3, 1};
  if (!lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 1[0, 2]_{2}, 1[0, 3]_{2}\n";
  }

  lhs = {3, 5, 1, 2}; rhs = {3, 0, 6, 2};
  if (lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 2[5, 1]_{3}, 2[0, 6]_{3}\n";
  }

  lhs = {3, 5, 1, 2}; rhs = {3, 7, 5, 1};
  if (!lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 2[5, 1]_{3}, 1[7, 5]_{3}\n";
  }

  lhs = {4, 0, 12, 6}; rhs = {4, 5, 1, 2};
  if (lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 6[0, 12]_{4}, 2[5, 1]_{4}\n";
  }

  lhs = {3, 5, 1, 2}; rhs = {3, 1, 7, 1};
  if (!lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 2[5, 1]_{3}, 1[1, 7]_{3}\n";
  }

  lhs = {4, 0, 12, 6}; rhs = {4, 11, 7, 2};
  if (lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 6[0, 12]_{4}, 2[11, 7]_{4}\n";
  }

  lhs = {2, 0, 2, 1}; rhs = {2, 2, 0, 1};
  if (lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 1[0, 2]_{2}, 1[2, 0]_{2}\n";
  }

  lhs = {3, 0, 3, 1}; rhs = {3, 5, 3, 1};
  if (!lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 1[0, 3]_{3}, 1[5, 3]_{3}\n";
  }

  lhs = {4, 9, 3, 2}; rhs = {4, 8, 4, 2};
  if (lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 2[9, 3]_{4}, 2[8, 4]_{4}\n";
  }

  lhs = {2, 0, 2, 1}; rhs = {2, 3, 1, 1};
  if (lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 1[0, 2]_{2}, 1[3, 1]_{2}\n";
  }

  lhs = {2, 0, 2, 1}; rhs = {2, 0, 2, 1};
  if (!lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 1[0, 2]_{2}, 1[0, 2]_{2}\n";
  }

  lhs = {2, 0, 2, 1}; rhs = {2, 0, 1, 1};
  if (lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 1[0, 2]_{2}, 1[0, 1]_{2}\n";
  }

  lhs = {2, 0, 2, 1}; rhs = {2, 1, 2, 1};
  if (lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 1[0, 2]_{2}, 1[1, 2]_{2}\n";
  }

  lhs = {3, 2, 0, 3}; rhs = {3, 4, 2, 3};
  if (lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 3[2, 0]_{3}, 3[4, 2]_{3}\n";
  }

  lhs = {3, 2, 0, 3}; rhs = {3, 5, 3, 3};
  if (lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 3[2, 0]_{3}, 3[5, 3]_{3}\n";
  }

  lhs = {3, 2, 0, 3}; rhs = {3, 0, 6, 3};
  if (lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 3[2, 0]_{3}, 3[0, 6]_{3}\n";
  }

  lhs = {3, 2, 0, 3}; rhs = {3, 7, 5, 3};
  if (lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 3[2, 0]_{3}, 3[7, 5]_{3}\n";
  }

  lhs = {3, 2, 0, 3}; rhs = {3, 6, 4, 3};
  if (lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 3[2, 0]_{3}, 3[6, 4]_{3}\n";
  }

  lhs = {4, 9, 3, 5}; rhs = {4, 4, 3, 5};
  if (!lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 5[9, 3]_{4}, 5[4, 3]_{4}\n";
  }

  lhs = {4, 9, 3, 5}; rhs = {4, 5, 4, 5};
  if (lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 5[9, 3]_{4}, 5[5, 4]_{4}\n";
  }

  lhs = {3, 2, 0, 3}; rhs = {3, 1, 7, 3};
  if (lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 3[2, 0]_{3}, 3[1, 7]_{3}\n";
  }

  lhs = {3, 2, 0, 3}; rhs = {3, 2, 0, 3};
  if (!lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 3[2, 0]_{3}, 3[2, 0]_{3}\n";
  }

  lhs = {3, 2, 0, 3}; rhs = {3, 2, 5, 3};
  if (lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 3[2, 0]_{3}, 3[2, 5]_{3}\n";
  }

  lhs = {3, 2, 0, 3}; rhs = {3, 3, 6, 3};
  if (lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 3[2, 0]_{3}, 3[3, 6]_{3}\n";
  }

  lhs = {2, 0, 2, 1}; rhs = {2, 0, 3, 3};
  if (lhs.lessOrEqual(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 1[0, 2]_{2}, 3[0, 3]_{2}\n";
  }
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
  APInt val;
  StridedInterval msi;
  val = {1}; msi = {1, 0, 1, 1};
  if (!msi.contains(val)) {
    errs() << "[testContains] failed with operands: 1, 1[0, 1]_{1}\n";
  }

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
