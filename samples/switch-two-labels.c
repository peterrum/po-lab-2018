#include <stdio.h>
#include <stdlib.h>
// test program:
// simple loop

int main(int argc, char const *argv[]) {
  int x = rand();
  int y = 0;
  y++;

  switch (x) {
    case 3:
    case 8:
      y = x;
      break;
    case 5:
      y = x+1;
  }

  int z = y+7;
  printf("%d\n", z);
}
