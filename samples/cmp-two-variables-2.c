#include <stdio.h>

// test program:
// comparison of two variables

int main(int argc, char const *argv[]) {
  int x, y;

  // perform dummy intialization of values
  if (argc == 0)
    x = 1;
  else
    x = 3;

  if (argc == 0)
    y = 0;
  else
    y = 4;

  // at this point x={1,2} y={0,4}
  if (x < y)
    // at this point x={1,3} and y={4}
    if (y == 0)
      // branch is never entered
      printf("%d\n", x);
}
