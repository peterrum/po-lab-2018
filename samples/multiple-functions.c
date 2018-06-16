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


int gcd() {
    int a = 7387;
    int b = 8633;
    int r;
    while (b > 0) {
        r = a % b;
        a = b;
        b = r;
    }

    // expected solution 89
    return a;
}
