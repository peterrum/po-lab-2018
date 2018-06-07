#include "../src/BoundedSet.h"
#include <set>
#include "llvm/Support/raw_os_ostream.h"
using namespace llvm;
namespace pcpo {

APInt apint0{32, 0, false};
APInt apint1{32, 1, false};
APInt apint2{32, 2, false};
APInt apint3{32, 3, false};
APInt apint5{32, 5, false};
BoundedSet top {true};
BoundedSet bot {false};
BoundedSet set0 {apint1};
BoundedSet set1_3{std::set<APInt, Comparator> {apint1, apint3}};
BoundedSet set2_5{std::set<APInt, Comparator> {apint2, apint5}};

void testConstructor() {
  errs() << "[testConstructor] top: ";
  top.printOut();
  errs() << "[testConstructor] bot: ";
  bot.printOut();
  errs() << "[testConstructor] singleton: ";
  set0.printOut();
  errs() << "[testConstructor] multi: ";
  set1_3.printOut();
}

void testLeastUpperBound() {
  auto result = set2_5.leastUpperBound(set1_3);
  BoundedSet res = *(static_cast<BoundedSet*> (result.get()));
  BoundedSet expRes {32, {1, 2, 3, 5}};
  if (!(res == expRes)) {
    errs() << "testLeastUpperBound failed\n";
  }
}

void testAdd() {
  auto result = set2_5.add(32, set1_3, false, false);
  BoundedSet res = *(static_cast<BoundedSet*> (result.get()));
  BoundedSet expRes {32, {3, 5, 6, 8}};
  if (!(res == expRes)) {
    errs() << "testAdd failed\n";
  }
}

void testLeastUpperBoundTop() {
  APInt first{32, 13, false};
  APInt second{32, 42, false};

  BoundedSet left{first};
  BoundedSet right{second};
  auto result = left.leastUpperBound(right);
  APInt tmpInt;
  for (int i = 0; i < 20; i++) {
    tmpInt = APInt(32, i, false);
    BoundedSet tmp(tmpInt);
    result = result->leastUpperBound(tmp);
  }
  BoundedSet res = *(static_cast<BoundedSet*> (result.get()));
  if (!(res == top)) {
    errs() << "testLeastUpperBoundTop failed\n";
    result->printOut();
  }
}

void run() {
  testConstructor();
  testLeastUpperBound();
  testAdd();
  testLeastUpperBoundTop();
}
} // namespace pcpo