#include <stdio.h>

// test program:
// simple loop

int main(int argc, char const *argv[]) {
  int x = 0;
   //x++;

  // at this point x={0}
  while (x <= 17)
    x++;

  // at this point x={17}
  printf("%d\n", x);
}
