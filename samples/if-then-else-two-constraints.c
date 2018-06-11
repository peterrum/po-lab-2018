#include <stdio.h>

// test program:
// simple loop

int main(int argc, char const *argv[]) {
  int x = 0;
  int y = 18;

  // at this point x={0}
  while (x < y) {
    x++;
    y--;
  }

  printf("%d\n", x);
  printf("%d\n", y);
}
