#include <stdio.h>
#include <stdlib.h>
// test program:
// simple loop

int main(int argc, char const *argv[]) {
  int x = 42;
  x += 7;
  int y = rand();

  if (x < y && x > y){
    int z = 42;
    printf("%d\n", z);
  }

  printf("%d\n", x);
  printf("%d\n", y);
}
