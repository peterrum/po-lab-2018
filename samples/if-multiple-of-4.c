#include <stdio.h>

// test program:
// testing with multiple of 4

int main(int argc, char const *argv[]) {
    unsigned x = argc;
    unsigned y = 5;
    x = 4 * x;
    if (x == 3) {
      y = 6;
    }


    // at this point x={17}
    printf("%d\n", x);
    printf("%d\n", y);
}
