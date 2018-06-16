#include <stdio.h>

// test program:
// simple loop

int main(int argc, char const *argv[]) {
  int x = 0;

  // at this point x={0}
  while (x > -17) {
    x--;
  }

  int y = x*x+x;
  printf("%d\n", y);
}
