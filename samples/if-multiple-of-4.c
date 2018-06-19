#include <stdio.h>

// test program:
// testing with multiple of 4

int main(int argc, char const *argv[]) {
    unsigned x = 0;
    unsigned y = 5;

    while (x <= 5){
        if(4*x==3){
            y = 6;
        }
        x++;
    }

    // at this point x={17}
    printf("%d\n", x);
    printf("%d\n", y);
}