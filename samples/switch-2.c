#include <stdio.h>
#include <stdlib.h>
// test program:
// simple loop

int main(int argc, char const *argv[]) {
  int x = 0;
  int y = 6;

  while (x < y) {
    x++;
    y--;
  }

  // x = [0,1,2,3,4,5,6]
  switch (x) {
    case 3:
      y = x*1; // x=3, y=2
      break;
    case 5:
      y = x*3; // x=5, y=40
      break;
    default:
      y = x;    // x=y=[0,1,2,4,6]
  }

  // y=[0, 1, 2, 4, 6, 40]
  printf("%d\n", y);
}
