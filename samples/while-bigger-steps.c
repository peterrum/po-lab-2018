#include <stdio.h>

// test program:
// simple loop

int main(int argc, char const *argv[]) {
  unsigned x = 0;
   //x++;

  // at this point x={0}
  while (x <= 80)
    x+=13;

  // at this point x={17}
  printf("%d\n", x);
}
