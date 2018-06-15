#include <stdio.h>

// test program:
// simple loop

int main(int argc, char const *argv[]) {
  int x = 0;

  // at this point x={0}
  while (x <= 100)
    x++;

  // at this point x={17}
  printf("%d\n", x);
}
