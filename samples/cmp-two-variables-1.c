#include <stdio.h>

// test program:
// comparison of two variables

int main(int argc, char const *argv[]) {
  int x, y;

  if (argc == 0)
    x = 1;
  else
    x = 3;

  if (argc == 0)
    y = 2;
  else
    y = 4;

  // at this point: x={1,3} and y={2,4}
  if (x < y)
    // at this point: x={1,3} and y={2,4}
    printf("%d\n", x);
  else
    // at this point: x={3} and y{2}
    printf("%d\n", y);
}
