#include "../src/BoundedSet.h"
#include <set>
using namespace llvm;
namespace pcpo {

void testConstructor() {
  APInt value{32, 13, false};

  BoundedSet left{value};
  BoundedSet right{value};
  auto result = left.add(right);
  result->printOut();
}

void testTop() {
  APInt first{32, 13, false};
  APInt second{32, 42, false};

  BoundedSet left{first};
  BoundedSet right{second};
  auto result = left.leastUpperBound(right);
  BoundedSet tmp{false};
  APInt tmpInt;
  for (int i = 0; i < 20; i++) {
    tmpInt = APInt(32, i, false);
    tmp = BoundedSet(tmpInt);
    result = result->leastUpperBound(tmp);
  }

  result->printOut();
}

void testLeastUpperBound() {
  APInt first{32, 13, false};
  APInt second{32, 42, false};

  BoundedSet left{first};
  BoundedSet right{second};
  auto result = left.leastUpperBound(right);
  result->printOut();
}

int main() {
  testConstructor();
  return 0;
}
} // namespace pcpo