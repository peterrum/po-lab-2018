#include "../src/abstract_domain/Util.h"
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

void testStridedIntervalRealSize() {
  StridedInterval i;
  i = {4, 3, 3, 0};
  if (!(i.realSize() == 1)) {
    errs() << "[testRealSize] failed with operand: " << i << '\n';
  }
  i = {4, 1, 5, 1};
  if (!(i.realSize() == 5)) {
    errs() << "[testRealSize] failed with operand: " << i << '\n';
  }
  i = {4, 2, 8, 2};
  if (!(i.realSize() == 4)) {
    errs() << "[testRealSize] failed with operand: " << i << '\n';
  }
  i = {4, 12, 5, 3};
  if (!(i.realSize() == 4)) {
    errs() << "[testRealSize] failed with operand: " << i << '\n';
  }
}

void testStridedIntervalLessOrEqual() {
  StridedInterval lhs, rhs;
  lhs = {1, 1, 1, 0}; rhs = {1, 0, 1, 1};
  if (!(lhs<=(rhs))) {
    errs() << "[testLessOrEqual] failed with operands: 0[1, 1]_{1}, 1[0, 1]_{1}\n";
  }

  lhs = {1, 1, 1, 0}; rhs = {1, 0, 0, 0};
  if (lhs<=(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 0[1, 1]_{1}, 0[0, 0]_{1}\n";
  }

  lhs = {1, 0, 1, 1}; rhs = {1, 1, 1, 0};
  if (lhs<=(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 1[0, 1]_{1}, 0[1, 1]_{1}\n";
  }

  lhs = {1, 0, 1, 1}; rhs = {1, 0, 1, 1};
  if (!(lhs<=(rhs))) {
    errs() << "[testLessOrEqual] failed with operands: 1[0, 1]_{1}, 1[0, 1]_{1}\n";
  }

  lhs = {2, 0, 1, 1}; rhs = {2, 0, 3, 3};
  if (lhs<=(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 1[0, 1]_{2}, 3[0, 3]_{2}\n";
  }

  lhs = {2, 0, 1, 1}; rhs = {2, 1, 3, 2};
  if (lhs<=(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 1[0, 1]_{2}, 2[1, 3]_{2}\n";
  }

  lhs = {2, 0, 2, 1}; rhs = {2, 0, 3, 1};
  if (!(lhs<=(rhs))) {
    errs() << "[testLessOrEqual] failed with operands: 1[0, 2]_{2}, 1[0, 3]_{2}\n";
  }

  lhs = {3, 5, 1, 2}; rhs = {3, 0, 6, 2};
  if (lhs<=(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 2[5, 1]_{3}, 2[0, 6]_{3}\n";
  }

  lhs = {3, 5, 1, 2}; rhs = {3, 7, 5, 1};
  if (!(lhs<=(rhs))) {
    errs() << "[testLessOrEqual] failed with operands: 2[5, 1]_{3}, 1[7, 5]_{3}\n";
  }

  lhs = {4, 0, 12, 6}; rhs = {4, 5, 1, 2};
  if (lhs<=(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 6[0, 12]_{4}, 2[5, 1]_{4}\n";
  }

  lhs = {3, 5, 1, 2}; rhs = {3, 1, 7, 1};
  if (!(lhs<=(rhs))) {
    errs() << "[testLessOrEqual] failed with operands: 2[5, 1]_{3}, 1[1, 7]_{3}\n";
  }

  lhs = {4, 0, 12, 6}; rhs = {4, 11, 7, 2};
  if (lhs<=(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 6[0, 12]_{4}, 2[11, 7]_{4}\n";
  }

  lhs = {2, 0, 2, 1}; rhs = {2, 2, 0, 1};
  if (lhs<=(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 1[0, 2]_{2}, 1[2, 0]_{2}\n";
  }

  lhs = {3, 0, 3, 1}; rhs = {3, 5, 3, 1};
  if (!(lhs<=(rhs))) {
    errs() << "[testLessOrEqual] failed with operands: 1[0, 3]_{3}, 1[5, 3]_{3}\n";
  }

  lhs = {4, 9, 3, 2}; rhs = {4, 8, 4, 2};
  if (lhs<=(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 2[9, 3]_{4}, 2[8, 4]_{4}\n";
  }

  lhs = {2, 0, 2, 1}; rhs = {2, 3, 1, 1};
  if (lhs<=(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 1[0, 2]_{2}, 1[3, 1]_{2}\n";
  }

  lhs = {2, 0, 2, 1}; rhs = {2, 0, 2, 1};
  if (!(lhs<=(rhs))) {
    errs() << "[testLessOrEqual] failed with operands: 1[0, 2]_{2}, 1[0, 2]_{2}\n";
  }

  lhs = {2, 0, 2, 1}; rhs = {2, 0, 1, 1};
  if (lhs<=(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 1[0, 2]_{2}, 1[0, 1]_{2}\n";
  }

  lhs = {2, 0, 2, 1}; rhs = {2, 1, 2, 1};
  if (lhs<=(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 1[0, 2]_{2}, 1[1, 2]_{2}\n";
  }

  lhs = {3, 2, 0, 3}; rhs = {3, 4, 2, 3};
  if (lhs<=(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 3[2, 0]_{3}, 3[4, 2]_{3}\n";
  }

  lhs = {3, 2, 0, 3}; rhs = {3, 5, 3, 3};
  if (lhs<=(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 3[2, 0]_{3}, 3[5, 3]_{3}\n";
  }

  lhs = {3, 2, 0, 3}; rhs = {3, 0, 6, 3};
  if (lhs<=(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 3[2, 0]_{3}, 3[0, 6]_{3}\n";
  }

  lhs = {3, 2, 0, 3}; rhs = {3, 7, 5, 3};
  if (lhs<=(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 3[2, 0]_{3}, 3[7, 5]_{3}\n";
  }

  lhs = {3, 2, 0, 3}; rhs = {3, 6, 4, 3};
  if (lhs<=(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 3[2, 0]_{3}, 3[6, 4]_{3}\n";
  }

  lhs = {4, 9, 3, 5}; rhs = {4, 4, 3, 5};
  if (!(lhs<=(rhs))) {
    errs() << "[testLessOrEqual] failed with operands: 5[9, 3]_{4}, 5[4, 3]_{4}\n";
  }

  lhs = {4, 9, 3, 5}; rhs = {4, 5, 4, 5};
  if (lhs<=(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 5[9, 3]_{4}, 5[5, 4]_{4}\n";
  }

  lhs = {3, 2, 0, 3}; rhs = {3, 1, 7, 3};
  if (lhs<=(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 3[2, 0]_{3}, 3[1, 7]_{3}\n";
  }

  lhs = {3, 2, 0, 3}; rhs = {3, 2, 0, 3};
  if (!(lhs<=(rhs))) {
    errs() << "[testLessOrEqual] failed with operands: 3[2, 0]_{3}, 3[2, 0]_{3}\n";
  }

  lhs = {3, 2, 0, 3}; rhs = {3, 2, 5, 3};
  if (lhs<=(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 3[2, 0]_{3}, 3[2, 5]_{3}\n";
  }

  lhs = {3, 2, 0, 3}; rhs = {3, 3, 6, 3};
  if (lhs<=(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 3[2, 0]_{3}, 3[3, 6]_{3}\n";
  }

  lhs = {2, 0, 2, 1}; rhs = {2, 0, 3, 3};
  if (lhs<=(rhs)) {
    errs() << "[testLessOrEqual] failed with operands: 1[0, 2]_{2}, 3[0, 3]_{2}\n";
  }
}

void testStridedIntervalIsNormal() {
  StridedInterval i;
  i = {8, 24, 48, 12};
  if (!i.isNormal()) {
    errs() << "[testIsNormal] failed with operand " << i << ": got " << i.isNormal() << ", expected " << true << "\n";
  }
  i = {8, 60, 90, 30};
  if (!i.isNormal()) {
    errs() << "[testIsNormal] failed with operand " << i << ": got " << i.isNormal() << ", expected " << true << "\n";
  }
  i = {8, 190, 244, 6};
  if (!i.isNormal()) {
    errs() << "[testIsNormal] failed with operand " << i << ": got " << i.isNormal() << ", expected " << true << "\n";
  }
  i = {4, 0, 3, 2};
  if (i.isNormal()) {
    errs() << "[testIsNormal] failed with operand " << i << ": got " << i.isNormal() << ", expected " << false << "\n";
  }
  i = {4, 14, 2, 4};
  if (i.isNormal()) {
    errs() << "[testIsNormal] failed with operand " << i << ": got " << i.isNormal() << ", expected " << false << "\n";
  }
  i = {4, 2, 2, 1};
  if (i.isNormal()) {
    errs() << "[testIsNormal] failed with operand " << i << ": got " << i.isNormal() << ", expected " << false << "\n";
  }
}

void testStridedIntervalGamma() {
  StridedInterval i;
  std::set<APInt, Comparator> res;
  i = {4, 2, 2, 0};
  res = i.gamma();
  errs() << "[testGamma] operand: " << i << ": {";
  for (APInt k : res) {
    errs() << k.toString(10, false) << ", ";
  }
  errs() << "}\n";
  i = {4, 1, 10, 3};
  res = i.gamma();
  errs() << "[testGamma] operand: " << i << ": {";
  for (APInt k : res) {
    errs() << k.toString(10, false) << ", ";
  }
  errs() << "}\n";
  i = {4, 7, 3, 4};
  res = i.gamma();
  errs() << "[testGamma] operand: " << i << ": {";
  for (APInt k : res) {
    errs() << k.toString(10, false) << ", ";
  }
  errs() << "}\n";
}

void testStridedIntervalLimits() {
  StridedInterval i;
  i = {4, 2, 2, 0};
  if (!(i.getUMin() == 2)) {
    errs() << "[testLimits] getUMin failed with operand " << i << ": got " << i.getUMin()
      << ", expected " << 2 << "\n";
  }
  if (!(i.getUMax() == 2)) {
    errs() << "[testLimits] getUMax failed with operand " << i << ": got " << i.getUMax()
      << ", expected " << 2 << "\n";
  }
  if (!(i.getSMin() == 2)) {
    errs() << "[testLimits] getSMin failed with operand " << i << ": got " << i.getSMin()
      << ", expected " << 2 << "\n";
  }
  if (!(i.getSMax() == 2)) {
    errs() << "[testLimits] getSMax failed with operand " << i << ": got " << i.getSMax()
      << ", expected " << 2 << "\n";
  }

  i = {4, 1, 10, 3};
  if (!(i.getUMin() == 1)) {
    errs() << "[testLimits] getUMin failed with operand " << i << ": got " << i.getUMin()
      << ", expected " << 1 << "\n";
  };
  if (!(i.getUMax() == 10)) {
    errs() << "[testLimits] getUMax failed with operand " << i << ": got " << i.getUMax()
      << ", expected " << 10 << "\n";
  };
  if (!(i.getSMin() == -6)) {
    errs() << "[testLimits] getSMin failed with operand " << i << ": got " << i.getSMin()
      << ", expected " << -6 << "\n";
  };
  if (!(i.getSMax() == 7)) {
    errs() << "[testLimits] getSMax failed with operand " << i << ": got " << i.getSMax()
      << ", expected " << 7 << "\n";
  }

  i = {4, 9, 13, 2};
  if (!(i.getUMin() == 9)) {
    errs() << "[testLimits] getUMin failed with operand " << i << ": got " << i.getUMin()
      << ", expected " << 9 << "\n";
  };
  if (!(i.getUMax() == 13)) {
    errs() << "[testLimits] getUMax failed with operand " << i << ": got " << i.getUMax()
      << ", expected " << 13 << "\n";
  };
  if (!(i.getSMin() == -7)) {
    errs() << "[testLimits] getSMin failed with operand " << i << ": got " << i.getSMin()
      << ", expected " << -7 << "\n";
  };
  if (!(i.getSMax() == -3)) {
    errs() << "[testLimits] getSMax failed with operand " << i << ": got " << i.getSMax()
      << ", expected " << -3 << "\n";
  }

  i = {4, 13, 3, 2};
  if (!(i.getUMin() == 1)) {
    errs() << "[testLimits] getUMin failed with operand " << i << ": got " << i.getUMin()
      << ", expected " << 1 << "\n";
  };
  if (!(i.getUMax() == 15)) {
    errs() << "[testLimits] getUMax failed with operand " << i << ": got " << i.getUMax()
      << ", expected " << 15 << "\n";
  };
  if (!(i.getSMin() == -3)) {
    errs() << "[testLimits] getSMin failed with operand " << i << ": got " << i.getSMin()
      << ", expected " << -3 << "\n";
  };
  if (!(i.getSMax() == 3)) {
    errs() << "[testLimits] getSMax failed with operand " << i << ": got " << i.getSMax()
      << ", expected " << 3 << "\n";
  }
}

void testStridedIntervalAdd() {
  StridedInterval lhs;
  StridedInterval rhs;
  StridedInterval ref;
  shared_ptr<AbstractDomain> res_p;
  StridedInterval res;

  lhs = {6, 0, 4, 2}; rhs = StridedInterval(false, 6);
  ref = StridedInterval(false, 6);
  res_p = lhs.add(6, rhs, false, false);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testADD] failed with operands " << lhs << ", " << rhs << " () "
      << ": got " << res << ", expected " << ref << "\n";
  }

  lhs = StridedInterval(false, 6); rhs = {6, 0, 4, 2};
  ref = StridedInterval(false, 6);
  res_p = lhs.add(6, rhs, false, false);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testADD] failed with operands " << lhs << ", " << rhs << " () "
      << ": got " << res << ", expected " << ref << "\n";
  }

  lhs = {6, 3, 7, 2}; rhs = {6, 1, 9, 4};
  ref = {6, 4, 16, 2};
  res_p = lhs.add(6, rhs, false, false);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testADD] failed with operands " << lhs << ", " << rhs << " () "
      << ": got " << res << ", expected " << ref << "\n";
  }
  ref = {6, 4, 16, 2};
  res_p = lhs.add(6, rhs, true, false);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testADD] failed with operands " << lhs << ", " << rhs << " (nuw) "
      << ": got " << res << ", expected " << ref << "\n";
  }
  ref = {6, 4, 16, 2};
  res_p = lhs.add(6, rhs, false, true);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testADD] failed with operands " << lhs << ", " << rhs << " (nsw) "
      << ": got " << res << ", expected " << ref << "\n";
  }
  ref = {6, 4, 16, 2};
  res_p = lhs.add(6, rhs, true, true);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testADD] failed with operands " << lhs << ", " << rhs << " (nuw, nsw) "
      << ": got " << res << ", expected " << ref << "\n";
  }

  lhs = {6, 12, 24, 6}; rhs = {6, 2, 22, 10};
  ref = {6, 14, 46, 2};
  res_p = lhs.add(6, rhs, false, false);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testADD] failed with operands " << lhs << ", " << rhs << " () "
      << ": got " << res << ", expected " << ref << "\n";
  }
  ref = {6, 14, 46, 2};
  res_p = lhs.add(6, rhs, true, false);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testADD] failed with operands " << lhs << ", " << rhs << " (nuw) "
      << ": got " << res << ", expected " << ref << "\n";
  }
  ref = {6, 0, 63, 1};
  res_p = lhs.add(6, rhs, false, true);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testADD] failed with operands " << lhs << ", " << rhs << " (nsw) "
      << ": got " << res << ", expected " << ref << "\n";
  }
  ref = {6, 0, 63, 1};
  res_p = lhs.add(6, rhs, true, true);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testADD] failed with operands " << lhs << ", " << rhs << " (nuw, nsw) "
      << ": got " << res << ", expected " << ref << "\n";
  }

  lhs = {6, 3, 21, 6}; rhs = {6, 51, 14, 9};
  ref = {6, 54, 35, 3};
  res_p = lhs.add(6, rhs, false, false);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testADD] failed with operands " << lhs << ", " << rhs << " () "
      << ": got " << res << ", expected " << ref << "\n";
  }
  ref = {6, 0, 63, 1};
  res_p = lhs.add(6, rhs, true, false);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testADD] failed with operands " << lhs << ", " << rhs << " (nuw) "
      << ": got " << res << ", expected " << ref << "\n";
  }
  ref = {6, 0, 63, 1};
  res_p = lhs.add(6, rhs, false, true);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testADD] failed with operands " << lhs << ", " << rhs << " (nsw) "
      << ": got " << res << ", expected " << ref << "\n";
  }
  ref = {6, 0, 63, 1};
  res_p = lhs.add(6, rhs, true, true);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testADD] failed with operands " << lhs << ", " << rhs << " (nuw, nsw) "
      << ": got " << res << ", expected " << ref << "\n";
  }

  lhs = {6, 60, 4, 2}; rhs = {6, 52, 6, 3};
  ref = {6, 48, 10, 1};
  res_p = lhs.add(6, rhs, false, false);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testADD] failed with operands " << lhs << ", " << rhs << " () "
      << ": got " << res << ", expected " << ref << "\n";
  }
  ref = {6, 0, 63, 1};
  res_p = lhs.add(6, rhs, true, false);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testADD] failed with operands " << lhs << ", " << rhs << " (nuw) "
      << ": got " << res << ", expected " << ref << "\n";
  }
  ref = {6, 48, 10, 1};
  res_p = lhs.add(6, rhs, false, true);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testADD] failed with operands " << lhs << ", " << rhs << " (nsw) "
      << ": got " << res << ", expected " << ref << "\n";
  }
  ref = {6, 0, 63, 1};
  res_p = lhs.add(6, rhs, true, true);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testADD] failed with operands " << lhs << ", " << rhs << " (nuw, nsw) "
      << ": got " << res << ", expected " << ref << "\n";
  }

  lhs = {6, 35, 43, 4}; rhs = {6, 42, 50, 4};
  ref = {6, 13, 29, 4};
  res_p = lhs.add(6, rhs, false, false);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testADD] failed with operands " << lhs << ", " << rhs << " () "
      << ": got " << res << ", expected " << ref << "\n";
  }
  ref = {6, 0, 63, 1};
  res_p = lhs.add(6, rhs, true, false);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testADD] failed with operands " << lhs << ", " << rhs << " (nuw) "
      << ": got " << res << ", expected " << ref << "\n";
  }
  ref = {6, 0, 63, 1};
  res_p = lhs.add(6, rhs, false, true);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testADD] failed with operands " << lhs << ", " << rhs << " (nsw) "
      << ": got " << res << ", expected " << ref << "\n";
  }
  ref = {6, 0, 63, 1};
  res_p = lhs.add(6, rhs, true, true);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testADD] failed with operands " << lhs << ", " << rhs << " (nuw, nsw) "
      << ": got " << res << ", expected " << ref << "\n";
  }
}

void testStridedIntervalSub() {
  StridedInterval lhs;
  StridedInterval rhs;
  StridedInterval ref;
  shared_ptr<AbstractDomain> res_p;
  StridedInterval res;
  lhs = {8, 24, 48, 12};
  rhs = {8, 60, 90, 30};
  ref = {8, 190, 244, 6};
  res_p = lhs.sub(8, rhs, false, false);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSub] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {6, 24, 48, 12};
  rhs = {6, 12, 18, 6};
  ref = {6, 6, 36, 6};
  res_p = lhs.sub(6, rhs, false, false);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSub] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 13, 3, 2};
  rhs = {4, 10, 12, 2};
  ref = {4, 1, 9, 2};
  res_p = lhs.sub(4, rhs, false, false);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSub] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
}

void testStridedIntervalMul() {
  StridedInterval lhs;
  StridedInterval rhs;
  StridedInterval ref;
  shared_ptr<AbstractDomain> res_p;
  StridedInterval res;
  lhs = {8, 2, 8, 3};
  rhs = {8, 5, 9, 2};
  ref = {8, 10, 72, 1};
  res_p = lhs.mul(8, rhs, false, false);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testMul] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {8, 2, 8, 3};
  rhs = {8, 6, 10, 2};
  ref = {8, 12, 80, 2};
  res_p = lhs.mul(8, rhs, false, false);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testMul] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 4, 12, 4};
  rhs = {4, 3, 9, 3};
  ref = {4, 0, 12, 4};
  res_p = lhs.mul(4, rhs, false, false);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testMul] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
}

void testStridedIntervalUdiv() {
  StridedInterval lhs;
  StridedInterval rhs;
  StridedInterval ref;
  shared_ptr<AbstractDomain> res_p;
  StridedInterval res;
  lhs = {6, 18, 36, 6};
  rhs = {6, 3, 3, 0};
  ref = {6, 6, 12, 2};
  res_p = lhs.udiv(6, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testUdiv] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {6, 18, 36, 6};
  rhs = {6, 3, 6, 2};
  ref = {6, 3, 12, 1};
  res_p = lhs.udiv(6, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testUdiv] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {6, 48, 8, 6};
  rhs = {6, 3, 6, 2};
  ref = {6, 0, 20, 1};
  res_p = lhs.udiv(6, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testUdiv] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {6, 3, 6, 2};
  rhs = {6, 48, 8, 6};
  ref = {6, 0, 3, 1};
  res_p = lhs.udiv(6, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testUdiv] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
}

void testStridedIntervalUrem() {
  StridedInterval lhs;
  StridedInterval rhs;
  StridedInterval ref;
  shared_ptr<AbstractDomain> res_p;
  StridedInterval res;
  lhs = {6, 3, 13, 5};
  rhs = {6, 14, 18, 2};
  ref = {6, 3, 13, 5};
  res_p = lhs.urem(6, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testUrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {6, 4, 13, 3};
  rhs = {6, 10, 10, 0};
  ref = {6, 0, 9, 1};
  res_p = lhs.urem(6, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testUrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {6, 12, 48, 6};
  rhs = {6, 30, 40, 10};
  ref = {6, 0, 40, 2};
  res_p = lhs.urem(6, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testUrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 4, 8, 2};
  rhs = {4, 10, 2, 4};
  ref = {4, 0, 8, 2};
  res_p = lhs.urem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testUrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 10, 2, 1};
  rhs = {4, 13, 1, 1};
  ref = {4, 0, 15, 1};
  res_p = lhs.urem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testUrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 10, 2, 2};
  rhs = {4, 8, 8, 0};
  ref = {4, 0, 6, 2};
  res_p = lhs.urem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testUrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
}

void testStridedIntervalLeastUpperBound() {
  StridedInterval lhs, rhs, res;
  lhs = {6, 0, 12, 6}; rhs = {6, 14, 44, 10};
  res = *(static_cast<StridedInterval *>(lhs.leastUpperBound(rhs).get()));
  errs() << "[leastUpperBound] sup " << lhs << " " << rhs << ": " << res << '\n';
  lhs = {4, 1, 5, 2}; rhs = {4, 7, 13, 3};
  res = *(static_cast<StridedInterval *>(lhs.leastUpperBound(rhs).get()));
  errs() << "[leastUpperBound] sup " << lhs << " " << rhs << ": " << res << '\n';
  lhs = {6, 2, 14, 6}; rhs = {6, 8, 28, 10};
  res = *(static_cast<StridedInterval *>(lhs.leastUpperBound(rhs).get()));
  errs() << "[leastUpperBound] sup " << lhs << " " << rhs << ": " << res << '\n';
  lhs = {4, 0, 8, 2}; rhs = {4, 6, 2, 2};
  res = *(static_cast<StridedInterval *>(lhs.leastUpperBound(rhs).get()));
  errs() << "[leastUpperBound] sup " << lhs << " " << rhs << ": " << res << '\n';
  lhs = {32, 2, 2, 0}; rhs = {32, 0, 1, 1};
  res = *(static_cast<StridedInterval *>(lhs.leastUpperBound(rhs).get()));
  errs() << "[leastUpperBound] sup " << lhs << " " << rhs << ": " << res << '\n';
  lhs = {32, 4294967294, 4294967294, 0}; rhs = {32, 0, 4294967295, 4294967295};
  res = *(static_cast<StridedInterval *>(lhs.leastUpperBound(rhs).get()));
  errs() << "[leastUpperBound] sup " << lhs << " " << rhs << ": " << res << '\n';
  lhs = {32, 1, 1, 0}; rhs = {32, 0, 2, 2};
  res = *(static_cast<StridedInterval *>(lhs.leastUpperBound(rhs).get()));
  errs() << "[leastUpperBound] sup " << lhs << " " << rhs << ": " << res << '\n';
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

void assertBottom(shared_ptr<AbstractDomain> ob, std::string test) {
    if(!ob->isBottom())
      errs() << test << "  isNotBottom";

    if(ob->isTop())
        errs() << test << "  isTop";
}

void assertTop(shared_ptr<AbstractDomain> ob, std::string test) {
    if(ob->isBottom())
      errs() << test << "  isBottom\n";

    if(!ob->isTop())
        errs() << test << "  isNotTop\n";
}

void testContainsRandom() {
  const std::string testName = "[containsRandom] ";
  unsigned bitWidth = 32;

  auto bottom = StridedInterval::create_bottom(bitWidth);
  auto top = StridedInterval::create_top(bitWidth);

  assertBottom(bottom, "bottom");
  assertTop(top, "top");

  assertBottom(bottom->leastUpperBound(*bottom), "bottom LUB bottom");

  assertTop(bottom->leastUpperBound(*top), "bottom LUB top");
  assertTop(top->leastUpperBound(*bottom), "top LUB bottom");

  for(unsigned stride=1; stride < 600; stride++) {
    auto previousIteration = bottom;
    unsigned insertCount = 0;

    for(unsigned i=0; i < 10000; i++) {
      if(i % stride != 0)
        continue;

      insertCount++;

      APInt other(bitWidth,i);
      APInt zero(bitWidth,0);
      StridedInterval newSI(other, other, zero);

      auto thisIteration = previousIteration->leastUpperBound(newSI);
      auto thisIterationRev = newSI.leastUpperBound(*previousIteration);

      StridedInterval* thisRevRaw = reinterpret_cast<StridedInterval*>(thisIterationRev.get());
      auto thisRevNorm = thisRevRaw->normalize();

      StridedInterval* thisRaw = reinterpret_cast<StridedInterval*>(thisIteration.get());
      auto thisNorm = thisRaw->normalize();

      if(*reinterpret_cast<StridedInterval*>(thisIteration.get()) != *reinterpret_cast<StridedInterval*>(thisIterationRev.get())) {
          errs() << "new " <<  newSI << "    prev " << *previousIteration << "\n";
          errs() << "prev->lub(new)                 " << *thisIteration << "\n";
          errs() << "prev->lub(new) (norm.)         " << *thisNorm << "\n";
          errs() << "new->lub(prev)                 " << *thisIterationRev << "\n";
          errs() << "new->lub(prev) (norm.)         " << *thisRevNorm << "\n";

      }

      if (thisIteration->size() != insertCount) {
          errs() << thisIteration->size() << " should be  " << insertCount << " size wrong\n";
          errs() << *thisIteration;
          return;
      }

      if(*reinterpret_cast<StridedInterval*>(thisIteration.get())
        != *reinterpret_cast<StridedInterval*>(thisIteration->leastUpperBound(*thisIteration).get())) {
          errs() << "LUB(this,this) != this 1\n";
          break;
      }

      if(*reinterpret_cast<StridedInterval*>(thisIteration.get())
        != *reinterpret_cast<StridedInterval*>(thisIteration->leastUpperBound(*previousIteration).get())) {
          errs() << "LUB(prev,this) != this 2\n";
          errs() << *thisIteration << " lub " << *previousIteration << " "
          << *thisIteration->leastUpperBound(*previousIteration);
          break;
      }

      if(*reinterpret_cast<StridedInterval*>(thisIteration.get())
        != *reinterpret_cast<StridedInterval*>(previousIteration->leastUpperBound(*thisIteration).get())) {
          errs() << "LUB(prev,this) != this 3\n";
          errs() << *thisIteration << " lub " << *previousIteration << " = "
          << *previousIteration->leastUpperBound(*thisIteration) << "\n";
          break;
      }

      if(*thisIteration<=(*previousIteration)) {
        errs() << *thisIteration << " " << *previousIteration << "Less or equal failed";
        return;
      }

      if(!(*previousIteration<=(*thisIteration))) {
        errs() << "Less or equal failed";
        return;
      }

      previousIteration = thisIteration;
    }
  }
}

void testFromBoundedSet() {
    unsigned bitWidth = 32;


    for(int stride=1;stride < 60; stride++) {
      auto bs = BoundedSet::create_bottom(bitWidth);
      auto si = StridedInterval::create_bottom(bitWidth);

      for(int i=0; i <30;i++) {
          APInt other(bitWidth,i*stride);
          APInt zero(bitWidth,0);
          StridedInterval newSI(other, other, zero);

          BoundedSet newBs(other);

          bs = bs->leastUpperBound(newBs);
          si = si->leastUpperBound(newSI);

          StridedInterval siFromBs(*reinterpret_cast<BoundedSet*>(bs.get()));

          if(siFromBs != *reinterpret_cast<StridedInterval*>(si.get())) {
            errs() << "Failed: strided interval " << siFromBs << " from bounded set "
             << *bs << " \n";
          }
      }
    }
}

void testFromBoundedSetDiffStrides() {
  unsigned bitWidth = 32;

  std::set<APInt, Comparator> values;

  for(auto i=0;i < 6000; i++) {
    if(i%6 == 0 || i%8 == 0)
      values.insert(APInt(bitWidth,i));
    if(i%100 == 0){
      BoundedSet bs(bitWidth,values);
      StridedInterval si(bs);
      for(auto v:values) {
        if (!si.contains(v))
          errs() <<"Si "<< si << "should contain " << v << "from Bs " << bs <<"\n";
      }
    }
  }
}

void testFromBoundedSetWrap() { 
  BoundedSet bs(4, {6, 9, 12, 15, 2});
  StridedInterval si(bs);

  StridedInterval expected(APInt(4, 6), APInt(4, 2), APInt(4, 3));
  if(si!=expected){
    errs() << "Si " << si << " should be " << expected << "\n";
  }
}

void testContainsRandomNeg() {
  const std::string testName = "[containsRandom] ";
  unsigned bitWidth = 32;

  auto bottom = StridedInterval::create_bottom(bitWidth);
  auto top = StridedInterval::create_top(bitWidth);

  assertBottom(bottom, "bottom");
  assertTop(top, "top");

  assertBottom(bottom->leastUpperBound(*bottom), "bottom LUB bottom");

  assertTop(bottom->leastUpperBound(*top), "bottom LUB top");
  assertTop(top->leastUpperBound(*bottom), "top LUB bottom");

  for(unsigned stride=1; stride < 600; stride++) {
    auto previousIteration = bottom;
    unsigned insertCount = 0;

    for(unsigned i=0; i < 10000; i++) {
      if(i % stride != 0)
        continue;

      insertCount++;

      APInt other(bitWidth,-i);
      APInt zero(bitWidth,0);
      StridedInterval newSI(other, other, zero);

      auto thisIteration = newSI.leastUpperBound(*previousIteration);
      auto thisIterationRev = previousIteration->leastUpperBound(newSI);

      StridedInterval* thisRevRaw = reinterpret_cast<StridedInterval*>(thisIterationRev.get());
      auto thisRevNorm = thisRevRaw->normalize();

      StridedInterval* thisRaw = reinterpret_cast<StridedInterval*>(thisIteration.get());
      auto thisNorm = thisRaw->normalize();

      if(*reinterpret_cast<StridedInterval*>(thisIteration.get()) != *reinterpret_cast<StridedInterval*>(thisIterationRev.get())) {
          errs() << "new " <<  newSI << "    prev " << *previousIteration << "\n";
          errs() << "prev->lub(new)                 " << *thisIterationRev << "\n";
          errs() << "prev->lub(new) (norm.)         " << *thisRevNorm << "\n";
          errs() << "new->lub(prev)                 " << *thisIteration << "\n";
          errs() << "new->lub(prev) (norm.)         " << *thisNorm << "\n";
          return;
      }


      if (thisIteration->size() != insertCount) {
          StridedInterval* prevRaw = reinterpret_cast<StridedInterval*>(previousIteration.get());
          auto prevNorm = prevRaw->normalize();
          errs() << *prevNorm  << "\n";

          errs() << *previousIteration << "\n";
          errs() << newSI << "\n";
          errs() << *thisIteration << "\n";
          errs() << thisIteration->size() << " should be  " << insertCount << " size wrong\n";
          return;
      }

      if(*reinterpret_cast<StridedInterval*>(thisIteration.get())
        != *reinterpret_cast<StridedInterval*>(thisIteration->leastUpperBound(*thisIteration).get())) {
          errs() << "LUB(this,this) != this 1\n";
          break;
      }

      if(*reinterpret_cast<StridedInterval*>(thisIteration.get())
        != *reinterpret_cast<StridedInterval*>(thisIteration->leastUpperBound(*previousIteration).get())) {
          errs() << "LUB(prev,this) != this 2\n";
          errs() << *thisIteration << " lub " << *previousIteration << " "
          << *thisIteration->leastUpperBound(*previousIteration);
          break;
      }

      if(*reinterpret_cast<StridedInterval*>(thisIteration.get())
        != *reinterpret_cast<StridedInterval*>(previousIteration->leastUpperBound(*thisIteration).get())) {
          errs() << "LUB(prev,this) != this 3\n";
          errs() << *thisIteration << " lub " << *previousIteration << " = "
          << *previousIteration->leastUpperBound(*thisIteration) << "\n";
          break;
      }

      if(*thisIteration<=(*previousIteration)) {
        errs() << *thisIteration << " " << *previousIteration << "Less or equal failed";
        return;
      }

      if(!(*previousIteration<=(*thisIteration))) {
        errs() << "Less or equal failed";
        return;
      }

      previousIteration = thisNorm;
    }
  }
}

void testFromBoundedSetNeg() {
    unsigned bitWidth = 32;

    for(int stride=3;stride < 500; stride++) {
      auto bs = BoundedSet::create_bottom(bitWidth);
      auto si = StridedInterval::create_bottom(bitWidth);

      for(int i=0; i <30;i++) {
          APInt other(bitWidth,-i*stride);
          APInt zero(bitWidth,0);
          StridedInterval newSI(other, other, zero);

          BoundedSet newBs(other);

          bs = bs->leastUpperBound(newBs);
          si = si->leastUpperBound(newSI);

          StridedInterval siFromBsRaw(*reinterpret_cast<BoundedSet*>(bs.get()));
          auto siFromBs = siFromBsRaw.normalize();

          if(*reinterpret_cast<StridedInterval*>(siFromBs.get()) != *reinterpret_cast<StridedInterval*>(si.get())) {
            errs() << "Failed: strided interval " << *siFromBs << " from bounded set "
             << *bs << " conventional SI yielded " << *si <<"\n";
             return;
          }
      }
    }
}

void testStridedIntervalSrem() {
  StridedInterval lhs;
  StridedInterval rhs;
  StridedInterval ref;
  shared_ptr<AbstractDomain> res_p;
  StridedInterval res;
  // Below are test cases with full path coverage, automatically generated using
  // the jupyer notebook.
  // One case is commented out, because in this case, the rhs is the constant
  // zero and therefore would terminate the programm.
  lhs = {1, 0, 0, 0}; rhs = {1, 1, 1, 0}; ref = {1, 0, 0, 0};
  res_p = lhs.srem(1, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 1, 1, 0}; rhs = {2, 3, 3, 0}; ref = {2, 0, 0, 0};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 1, 1, 0}; rhs = {2, 2, 3, 1}; ref = {2, 0, 1, 1};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 1, 0}; rhs = {3, 5, 7, 1}; ref = {3, 0, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 0, 1, 1}; rhs = {2, 2, 2, 0}; ref = {2, 0, 1, 1};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 2, 3, 1}; rhs = {3, 6, 6, 0}; ref = {3, 0, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 3, 1}; rhs = {3, 7, 7, 0}; ref = {3, 0, 0, 0};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 0, 1, 1}; rhs = {2, 3, 3, 0}; ref = {2, 0, 0, 0};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 3, 1}; rhs = {3, 5, 7, 1}; ref = {3, 0, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 2, 1}; rhs = {3, 4, 6, 1}; ref = {3, 0, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 0, 1, 1}; rhs = {2, 2, 3, 1}; ref = {2, 0, 1, 1};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 0, 1, 1}; rhs = {3, 5, 7, 1}; ref = {3, 0, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 3, 3, 0}; rhs = {2, 2, 2, 0}; ref = {2, 3, 3, 0};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {1, 1, 1, 0}; rhs = {1, 1, 1, 0}; ref = {1, 0, 0, 0};
  res_p = lhs.srem(1, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 3, 3, 0}; rhs = {2, 2, 3, 1}; ref = {2, 0, 3, 3};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 2, 2, 0}; rhs = {2, 2, 3, 1}; ref = {2, 0, 3, 3};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 0, 3, 3}; rhs = {2, 2, 2, 0}; ref = {2, 0, 3, 3};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 5, 6, 1}; rhs = {3, 6, 6, 0}; ref = {3, 0, 7, 7};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 0, 2, 1}; rhs = {2, 3, 3, 0}; ref = {2, 0, 0, 0};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 5, 1, 2}; rhs = {3, 5, 5, 0}; ref = {3, 6, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {1, 0, 1, 1}; rhs = {1, 1, 1, 0}; ref = {1, 0, 0, 0};
  res_p = lhs.srem(1, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 1, 3, 2}; rhs = {2, 2, 3, 1}; ref = {2, 3, 1, 1};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 6, 5}; rhs = {3, 5, 7, 1}; ref = {3, 6, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 0, 3, 3}; rhs = {2, 2, 3, 1}; ref = {2, 0, 3, 3};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 0, 2, 1}; rhs = {2, 2, 3, 1}; ref = {2, 3, 1, 1};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 5, 1, 2}; rhs = {3, 5, 7, 1}; ref = {3, 6, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 2, 0, 1}; rhs = {2, 2, 3, 1}; ref = {2, 0, 3, 3};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 6, 3, 1}; rhs = {3, 4, 4, 0}; ref = {3, 6, 3, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 6, 3, 1}; rhs = {3, 5, 5, 0}; ref = {3, 6, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 6, 3, 1}; rhs = {3, 7, 7, 0}; ref = {3, 0, 0, 0};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 6, 3, 1}; rhs = {3, 5, 7, 1}; ref = {3, 6, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 7, 2, 1}; rhs = {3, 4, 6, 1}; ref = {3, 7, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 6, 3, 1}; rhs = {3, 6, 7, 1}; ref = {3, 7, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 0, 0, 0}; rhs = {2, 3, 1, 1}; ref = {2, 0, 0, 0};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 1, 1, 0}; rhs = {2, 3, 1, 1}; ref = {2, 0, 0, 0};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 0, 1, 1}; rhs = {3, 5, 3, 3}; ref = {3, 0, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 2, 3, 1}; rhs = {3, 6, 2, 2}; ref = {3, 0, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 3, 1}; rhs = {3, 7, 1, 1}; ref = {3, 0, 0, 0};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 0, 1, 1}; rhs = {2, 3, 1, 1}; ref = {2, 0, 0, 0};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 7, 7, 0}; rhs = {3, 5, 3, 3}; ref = {3, 7, 7, 0};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 3, 3, 0}; rhs = {2, 3, 1, 1}; ref = {2, 0, 0, 0};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 6, 5}; rhs = {3, 5, 3, 3}; ref = {3, 1, 6, 5};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 5, 6, 1}; rhs = {3, 6, 2, 2}; ref = {3, 0, 7, 7};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 0, 2, 1}; rhs = {2, 3, 1, 1}; ref = {2, 0, 0, 0};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 5, 1, 2}; rhs = {3, 5, 3, 3}; ref = {3, 6, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 0, 3, 3}; rhs = {2, 3, 1, 1}; ref = {2, 0, 0, 0};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 7, 2, 1}; rhs = {3, 5, 3, 3}; ref = {3, 7, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 6, 3, 1}; rhs = {3, 5, 3, 3}; ref = {3, 6, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 6, 3, 1}; rhs = {3, 7, 1, 1}; ref = {3, 0, 0, 0};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 0, 0, 0}; rhs = {3, 5, 3, 1}; ref = {3, 0, 0, 0};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 3, 3, 0}; rhs = {3, 5, 3, 1}; ref = {3, 0, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 1, 0}; rhs = {3, 5, 3, 1}; ref = {3, 0, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 0, 2, 1}; rhs = {4, 10, 6, 3}; ref = {4, 0, 2, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 3, 1}; rhs = {3, 5, 3, 1}; ref = {3, 0, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 1, 3, 1}; rhs = {4, 9, 7, 1}; ref = {4, 0, 3, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 0, 3, 1}; rhs = {3, 5, 3, 1}; ref = {3, 0, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 0, 1, 1}; rhs = {3, 5, 3, 1}; ref = {3, 0, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 14, 14, 0}; rhs = {4, 10, 6, 3}; ref = {4, 14, 14, 0};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 7, 7, 0}; rhs = {3, 5, 3, 1}; ref = {3, 0, 7, 7};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 6, 6, 0}; rhs = {3, 7, 2, 1}; ref = {3, 0, 7, 7};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 1, 14, 13}; rhs = {4, 10, 6, 3}; ref = {4, 1, 14, 13};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 6, 2, 1}; rhs = {3, 5, 3, 1}; ref = {3, 6, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 6, 5}; rhs = {3, 5, 3, 1}; ref = {3, 6, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 0, 6, 6}; rhs = {3, 5, 3, 1}; ref = {3, 6, 0, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 5, 1}; rhs = {3, 5, 3, 1}; ref = {3, 6, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 5, 1, 2}; rhs = {3, 5, 3, 1}; ref = {3, 6, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 5, 7, 1}; rhs = {3, 5, 3, 1}; ref = {3, 6, 0, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 15, 2, 1}; rhs = {4, 10, 6, 3}; ref = {4, 15, 2, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 6, 3, 1}; rhs = {3, 5, 3, 1}; ref = {3, 6, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 3, 15, 12}; rhs = {4, 9, 7, 1}; ref = {4, 15, 3, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 6, 3, 1}; rhs = {3, 7, 2, 1}; ref = {3, 7, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 0, 0, 0}; rhs = {2, 1, 3, 2}; ref = {2, 0, 0, 0};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 1, 1, 0}; rhs = {2, 1, 3, 2}; ref = {2, 0, 0, 0};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 1, 0}; rhs = {3, 3, 7, 2}; ref = {3, 0, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 0, 1, 1}; rhs = {3, 2, 6, 4}; ref = {3, 0, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 3, 1}; rhs = {3, 3, 7, 2}; ref = {3, 0, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 1, 3, 1}; rhs = {4, 1, 15, 2}; ref = {4, 0, 3, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 0, 1, 1}; rhs = {2, 1, 3, 2}; ref = {2, 0, 0, 0};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 0, 1, 1}; rhs = {3, 3, 7, 2}; ref = {3, 0, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 7, 7, 0}; rhs = {3, 2, 6, 4}; ref = {3, 7, 7, 0};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 7, 7, 0}; rhs = {3, 3, 7, 2}; ref = {3, 0, 7, 7};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 3, 3, 0}; rhs = {2, 1, 3, 2}; ref = {2, 0, 0, 0};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 7, 1, 1}; rhs = {3, 2, 6, 4}; ref = {3, 7, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 6, 2, 1}; rhs = {3, 3, 7, 2}; ref = {3, 6, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 6, 5}; rhs = {3, 3, 7, 2}; ref = {3, 6, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 0, 6, 6}; rhs = {3, 3, 7, 2}; ref = {3, 6, 0, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 0, 2, 1}; rhs = {2, 1, 3, 2}; ref = {2, 0, 0, 0};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 5, 1, 2}; rhs = {3, 3, 7, 2}; ref = {3, 6, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 0, 3, 3}; rhs = {2, 1, 3, 2}; ref = {2, 0, 0, 0};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 3, 15, 12}; rhs = {4, 4, 12, 8}; ref = {4, 3, 15, 12};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 6, 3, 1}; rhs = {3, 3, 7, 2}; ref = {3, 6, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 3, 15, 12}; rhs = {4, 1, 15, 2}; ref = {4, 15, 3, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 6, 3, 1}; rhs = {3, 1, 7, 6}; ref = {3, 0, 0, 0};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {1, 0, 0, 0}; rhs = {1, 0, 1, 1}; ref = {1, 0, 0, 0};
  res_p = lhs.srem(1, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 1, 1, 0}; rhs = {2, 0, 3, 3}; ref = {2, 0, 0, 0};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 0, 1, 1}; rhs = {2, 0, 2, 2}; ref = {2, 0, 1, 1};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 2, 3, 1}; rhs = {3, 0, 6, 6}; ref = {3, 0, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 3, 1}; rhs = {3, 0, 6, 6}; ref = {3, 0, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 0, 1, 1}; rhs = {2, 0, 3, 3}; ref = {2, 0, 0, 0};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 3, 3, 0}; rhs = {2, 0, 2, 2}; ref = {2, 3, 3, 0};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {1, 1, 1, 0}; rhs = {1, 0, 1, 1}; ref = {1, 0, 0, 0};
  res_p = lhs.srem(1, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 0, 3, 3}; rhs = {2, 0, 2, 2}; ref = {2, 0, 3, 3};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 5, 6, 1}; rhs = {3, 0, 6, 6}; ref = {3, 0, 7, 7};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 0, 2, 1}; rhs = {2, 0, 3, 3}; ref = {2, 0, 0, 0};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 2, 4, 2}; rhs = {3, 0, 4, 4}; ref = {3, 6, 2, 2};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {1, 0, 1, 1}; rhs = {1, 0, 1, 1}; ref = {1, 0, 0, 0};
  res_p = lhs.srem(1, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 6, 3, 1}; rhs = {3, 0, 4, 4}; ref = {3, 6, 3, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 7, 3, 1}; rhs = {3, 0, 6, 6}; ref = {3, 7, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 6, 3, 1}; rhs = {3, 0, 6, 6}; ref = {3, 7, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 0, 0, 0}; rhs = {2, 0, 2, 1}; ref = {2, 0, 0, 0};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 1, 1, 0}; rhs = {2, 0, 2, 1}; ref = {2, 0, 1, 1};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 1, 0}; rhs = {3, 1, 5, 1}; ref = {3, 0, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 0, 1, 1}; rhs = {3, 2, 4, 2}; ref = {3, 0, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 3, 1}; rhs = {3, 1, 5, 1}; ref = {3, 0, 3, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 2, 1}; rhs = {3, 1, 5, 1}; ref = {3, 0, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 0, 1, 1}; rhs = {2, 0, 2, 1}; ref = {2, 0, 1, 1};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 0, 1, 1}; rhs = {3, 1, 5, 1}; ref = {3, 0, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 7, 7, 0}; rhs = {3, 2, 4, 2}; ref = {3, 7, 7, 0};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 3, 3, 0}; rhs = {2, 0, 2, 1}; ref = {2, 0, 3, 3};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 2, 2, 0}; rhs = {2, 0, 2, 1}; ref = {2, 0, 3, 3};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 7, 1, 1}; rhs = {3, 2, 4, 2}; ref = {3, 7, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 1, 3, 2}; rhs = {2, 0, 2, 1}; ref = {2, 3, 1, 1};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 6, 5}; rhs = {3, 1, 5, 1}; ref = {3, 6, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 0, 3, 3}; rhs = {2, 0, 2, 1}; ref = {2, 0, 3, 3};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 0, 2, 1}; rhs = {2, 0, 2, 1}; ref = {2, 3, 1, 1};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 5, 1, 2}; rhs = {3, 2, 0, 3}; ref = {3, 6, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 2, 0, 1}; rhs = {2, 0, 2, 1}; ref = {2, 0, 3, 3};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 3, 15, 12}; rhs = {4, 4, 8, 4}; ref = {4, 3, 15, 12};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 6, 3, 1}; rhs = {3, 1, 5, 1}; ref = {3, 6, 3, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 7, 2, 1}; rhs = {3, 1, 5, 1}; ref = {3, 7, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 6, 3, 1}; rhs = {3, 1, 6, 5}; ref = {3, 7, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 0, 0, 0}; rhs = {3, 5, 1, 2}; ref = {3, 0, 0, 0};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 3, 3, 0}; rhs = {3, 5, 1, 2}; ref = {3, 0, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 1, 0}; rhs = {3, 5, 1, 2}; ref = {3, 0, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 0, 1, 1}; rhs = {4, 9, 3, 5}; ref = {4, 0, 1, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 3, 1}; rhs = {3, 5, 1, 2}; ref = {3, 0, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 2, 1}; rhs = {3, 4, 2, 3}; ref = {3, 0, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 0, 3, 1}; rhs = {3, 5, 1, 2}; ref = {3, 0, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 0, 1, 1}; rhs = {3, 5, 1, 2}; ref = {3, 0, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 15, 15, 0}; rhs = {4, 9, 3, 5}; ref = {4, 15, 15, 0};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 7, 7, 0}; rhs = {3, 5, 1, 2}; ref = {3, 0, 7, 7};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 5, 5, 0}; rhs = {3, 5, 1, 2}; ref = {3, 6, 0, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 1, 15, 14}; rhs = {4, 9, 3, 5}; ref = {4, 1, 15, 14};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 5, 3, 1}; rhs = {3, 4, 2, 3}; ref = {3, 5, 3, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 6, 5}; rhs = {3, 5, 1, 2}; ref = {3, 6, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 5, 7, 1}; rhs = {3, 4, 2, 3}; ref = {3, 5, 0, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 5, 1}; rhs = {3, 5, 1, 2}; ref = {3, 6, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 5, 1, 2}; rhs = {3, 5, 1, 2}; ref = {3, 6, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 5, 7, 1}; rhs = {3, 5, 1, 2}; ref = {3, 6, 0, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 6, 3, 1}; rhs = {3, 5, 1, 2}; ref = {3, 6, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 7, 2, 1}; rhs = {3, 4, 2, 3}; ref = {3, 7, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 11, 10, 3}; rhs = {4, 12, 2, 3}; ref = {4, 13, 3, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 0, 0, 0}; rhs = {4, 11, 7, 6}; ref = {4, 0, 0, 0};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 7, 7, 0}; rhs = {4, 11, 7, 6}; ref = {4, 0, 6, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 5, 5, 0}; rhs = {4, 11, 7, 6}; ref = {4, 0, 5, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 0, 1, 1}; rhs = {4, 13, 7, 5}; ref = {4, 0, 1, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 1, 3, 1}; rhs = {4, 14, 4, 3}; ref = {4, 0, 3, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 1, 3, 1}; rhs = {4, 11, 7, 6}; ref = {4, 0, 3, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 0, 3, 3}; rhs = {4, 14, 4, 3}; ref = {4, 0, 3, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 0, 2, 1}; rhs = {4, 11, 7, 6}; ref = {4, 0, 2, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 15, 15, 0}; rhs = {4, 13, 7, 5}; ref = {4, 15, 15, 0};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 14, 14, 0}; rhs = {4, 11, 7, 6}; ref = {4, 14, 0, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 8, 8, 0}; rhs = {4, 11, 7, 6}; ref = {4, 10, 0, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 1, 15, 14}; rhs = {4, 13, 7, 5}; ref = {4, 1, 15, 14};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 10, 6, 1}; rhs = {4, 11, 7, 6}; ref = {4, 10, 6, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 12, 2, 3}; rhs = {4, 11, 7, 6}; ref = {4, 12, 2, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 10, 13, 3}; rhs = {4, 11, 7, 6}; ref = {4, 10, 0, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 1, 11, 1}; rhs = {4, 11, 7, 6}; ref = {4, 10, 6, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 9, 1, 1}; rhs = {4, 11, 7, 6}; ref = {4, 10, 1, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 9, 11, 1}; rhs = {4, 11, 7, 6}; ref = {4, 10, 0, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 11, 10, 3}; rhs = {4, 11, 7, 6}; ref = {4, 10, 6, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 3, 15, 12}; rhs = {4, 11, 7, 6}; ref = {4, 15, 3, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 11, 10, 3}; rhs = {4, 14, 4, 3}; ref = {4, 13, 3, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 0, 0, 0}; rhs = {3, 1, 5, 4}; ref = {3, 0, 0, 0};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 3, 3, 0}; rhs = {3, 1, 5, 4}; ref = {3, 0, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 1, 0}; rhs = {3, 1, 5, 4}; ref = {3, 0, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 0, 2, 1}; rhs = {4, 3, 11, 8}; ref = {4, 0, 2, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 3, 1}; rhs = {3, 1, 5, 4}; ref = {3, 0, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 1, 3, 1}; rhs = {4, 5, 13, 4}; ref = {4, 0, 3, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 0, 3, 1}; rhs = {3, 1, 5, 4}; ref = {3, 0, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 0, 1, 1}; rhs = {3, 1, 5, 4}; ref = {3, 0, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 14, 14, 0}; rhs = {4, 3, 11, 8}; ref = {4, 14, 14, 0};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 7, 7, 0}; rhs = {3, 1, 5, 4}; ref = {3, 0, 7, 7};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 5, 5, 0}; rhs = {3, 1, 5, 4}; ref = {3, 6, 0, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 1, 14, 13}; rhs = {4, 3, 11, 8}; ref = {4, 1, 14, 13};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 6, 2, 1}; rhs = {3, 1, 5, 4}; ref = {3, 6, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 6, 5}; rhs = {3, 1, 5, 4}; ref = {3, 6, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 0, 6, 6}; rhs = {3, 1, 5, 4}; ref = {3, 6, 0, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 5, 1}; rhs = {3, 1, 5, 4}; ref = {3, 6, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 5, 1, 2}; rhs = {3, 1, 5, 4}; ref = {3, 6, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 5, 7, 1}; rhs = {3, 1, 5, 4}; ref = {3, 6, 0, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 15, 2, 1}; rhs = {4, 3, 11, 8}; ref = {4, 15, 2, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 6, 3, 1}; rhs = {3, 1, 5, 4}; ref = {3, 6, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 3, 15, 12}; rhs = {4, 5, 13, 4}; ref = {4, 15, 3, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 11, 10, 3}; rhs = {4, 1, 13, 12}; ref = {4, 14, 2, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  // lhs = {1, 0, 1, 1}; rhs = {1, 0, 0, 0}; ref = {1, 0, 1, 1};
  // res_p = lhs.srem(1, rhs);
  // res = *(static_cast<StridedInterval *>(res_p.get()));
  // if (res != ref) {
  //   errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  // }
  lhs = {2, 0, 0, 0}; rhs = {2, 0, 1, 1}; ref = {2, 0, 0, 0};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 1, 1, 0}; rhs = {2, 0, 1, 1}; ref = {2, 0, 0, 0};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 0, 1, 1}; rhs = {3, 0, 3, 3}; ref = {3, 0, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 2, 3, 1}; rhs = {3, 0, 2, 2}; ref = {3, 0, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 3, 1}; rhs = {3, 0, 1, 1}; ref = {3, 0, 0, 0};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 0, 1, 1}; rhs = {2, 0, 1, 1}; ref = {2, 0, 0, 0};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 7, 7, 0}; rhs = {3, 0, 3, 3}; ref = {3, 7, 7, 0};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 3, 3, 0}; rhs = {2, 0, 1, 1}; ref = {2, 0, 0, 0};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 6, 5}; rhs = {3, 0, 3, 3}; ref = {3, 1, 6, 5};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 5, 6, 1}; rhs = {3, 0, 2, 2}; ref = {3, 0, 7, 7};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 0, 2, 1}; rhs = {2, 0, 1, 1}; ref = {2, 0, 0, 0};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 5, 1, 2}; rhs = {3, 0, 3, 3}; ref = {3, 6, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 0, 3, 3}; rhs = {2, 0, 1, 1}; ref = {2, 0, 0, 0};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 7, 2, 1}; rhs = {3, 0, 3, 3}; ref = {3, 7, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 6, 3, 1}; rhs = {3, 0, 3, 3}; ref = {3, 6, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 6, 3, 1}; rhs = {3, 0, 1, 1}; ref = {3, 0, 0, 0};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 0, 0, 0}; rhs = {3, 0, 3, 1}; ref = {3, 0, 0, 0};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 3, 3, 0}; rhs = {3, 0, 3, 1}; ref = {3, 0, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 1, 0}; rhs = {3, 0, 3, 1}; ref = {3, 0, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 0, 2, 1}; rhs = {4, 0, 6, 3}; ref = {4, 0, 2, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 3, 1}; rhs = {3, 0, 3, 1}; ref = {3, 0, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 1, 3, 1}; rhs = {4, 0, 5, 1}; ref = {4, 0, 3, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 0, 3, 1}; rhs = {3, 0, 3, 1}; ref = {3, 0, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 0, 1, 1}; rhs = {3, 0, 3, 1}; ref = {3, 0, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 14, 14, 0}; rhs = {4, 0, 6, 3}; ref = {4, 14, 14, 0};
  res_p = lhs.srem(4, rhs);

  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 7, 7, 0}; rhs = {3, 0, 3, 1}; ref = {3, 0, 7, 7};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 6, 6, 0}; rhs = {3, 0, 2, 1}; ref = {3, 0, 7, 7};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 1, 14, 13}; rhs = {4, 0, 6, 3}; ref = {4, 1, 14, 13};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 6, 2, 1}; rhs = {3, 0, 3, 1}; ref = {3, 6, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 6, 5}; rhs = {3, 0, 3, 1}; ref = {3, 6, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 0, 6, 6}; rhs = {3, 0, 3, 1}; ref = {3, 6, 0, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 5, 1}; rhs = {3, 0, 3, 1}; ref = {3, 6, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 5, 1, 2}; rhs = {3, 0, 3, 1}; ref = {3, 6, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 5, 7, 1}; rhs = {3, 0, 3, 1}; ref = {3, 6, 0, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 15, 2, 1}; rhs = {4, 0, 6, 3}; ref = {4, 15, 2, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 6, 3, 1}; rhs = {3, 0, 3, 1}; ref = {3, 6, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 3, 15, 12}; rhs = {4, 0, 5, 1}; ref = {4, 15, 3, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 6, 3, 1}; rhs = {3, 0, 2, 1}; ref = {3, 7, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 0, 0, 0}; rhs = {2, 1, 1, 0}; ref = {2, 0, 0, 0};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 1, 1, 0}; rhs = {2, 1, 1, 0}; ref = {2, 0, 0, 0};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 3, 3, 0}; rhs = {3, 1, 3, 1}; ref = {3, 0, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 1, 0}; rhs = {3, 1, 3, 1}; ref = {3, 0, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 0, 1, 1}; rhs = {3, 2, 3, 1}; ref = {3, 0, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 2, 3, 1}; rhs = {3, 2, 2, 0}; ref = {3, 0, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 3, 1}; rhs = {3, 3, 3, 0}; ref = {3, 0, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 0, 1, 1}; rhs = {2, 1, 1, 0}; ref = {2, 0, 0, 0};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 3, 1}; rhs = {3, 1, 3, 1}; ref = {3, 0, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 1, 3, 1}; rhs = {4, 2, 7, 1}; ref = {4, 0, 3, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 0, 3, 1}; rhs = {3, 1, 3, 1}; ref = {3, 0, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 0, 1, 1}; rhs = {3, 1, 3, 1}; ref = {3, 0, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 7, 7, 0}; rhs = {3, 2, 3, 1}; ref = {3, 7, 7, 0};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 3, 3, 0}; rhs = {2, 1, 1, 0}; ref = {2, 0, 0, 0};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 7, 7, 0}; rhs = {3, 1, 3, 1}; ref = {3, 0, 7, 7};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 6, 6, 0}; rhs = {3, 1, 2, 1}; ref = {3, 0, 7, 7};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 6, 5}; rhs = {3, 3, 3, 0}; ref = {3, 1, 6, 5};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 5, 6, 1}; rhs = {3, 2, 2, 0}; ref = {3, 0, 7, 7};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 0, 2, 1}; rhs = {2, 1, 1, 0}; ref = {2, 0, 0, 0};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 5, 1, 2}; rhs = {3, 3, 3, 0}; ref = {3, 6, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {2, 0, 3, 3}; rhs = {2, 1, 1, 0}; ref = {2, 0, 0, 0};
  res_p = lhs.srem(2, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 6, 2, 1}; rhs = {3, 1, 3, 1}; ref = {3, 6, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 6, 5}; rhs = {3, 1, 3, 1}; ref = {3, 6, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 0, 6, 6}; rhs = {3, 1, 3, 1}; ref = {3, 6, 0, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 1, 5, 1}; rhs = {3, 1, 3, 1}; ref = {3, 6, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 5, 1, 2}; rhs = {3, 1, 3, 1}; ref = {3, 6, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 5, 7, 1}; rhs = {3, 1, 3, 1}; ref = {3, 6, 0, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 7, 2, 1}; rhs = {3, 3, 3, 0}; ref = {3, 7, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 6, 3, 1}; rhs = {3, 3, 3, 0}; ref = {3, 6, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 6, 3, 1}; rhs = {3, 1, 1, 0}; ref = {3, 0, 0, 0};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 6, 3, 1}; rhs = {3, 1, 3, 1}; ref = {3, 6, 2, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {4, 3, 15, 12}; rhs = {4, 2, 7, 1}; ref = {4, 15, 3, 1};
  res_p = lhs.srem(4, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
  lhs = {3, 6, 3, 1}; rhs = {3, 1, 2, 1}; ref = {3, 7, 1, 1};
  res_p = lhs.srem(3, rhs);
  res = *(static_cast<StridedInterval *>(res_p.get()));
  if (res != ref) {
    errs() << "[testSrem] failed with operands " << lhs << ", " << rhs << ": got " << res << ", expected " << ref << "\n";
  }
}

void runStridedInterval() {
  testStridedIntervalRealSize();
  // testStridedIntervalSrem();
  testContainsRandom();
  testContainsRandomNeg();
  testFromBoundedSet();
  testFromBoundedSetNeg();
  testFromBoundedSetDiffStrides();
  testFromBoundedSetWrap();
  testStridedIntervalLessOrEqual();
  // testStridedIntervalLeastUpperBound();
  testStridedIntervalIsNormal();
  // testStridedIntervalGamma();
  testStridedIntervalLimits();
  testStridedIntervalAdd();
  testStridedIntervalSub();
  testStridedIntervalMul();
  testStridedIntervalUdiv();
  // testStridedIntervalUrem();
  // testContains();

}
} // namespace pcpo
