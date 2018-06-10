#include <stdio.h>
#include <stdlib.h>
// test program:
// simple loop

int main(int argc, char const *argv[]) {
  int x = rand();

  if (x < 17){
    int z = 42;
    z += 7;
    printf("%d\n", z);
  }

  printf("%d\n", x);
}
