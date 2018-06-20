#include <stdio.h>
#include <stdlib.h>
// test program:
// unsigned
// simple loop
// function call -> top
// if constraint with two constraints
// switch
// switch with case 3: bottom
// switch with case 6: fallthrough
// switch with default
// modulo on top


int main(int argc, char const *argv[]) {
    unsigned a = 0;
    unsigned b = 6;

    while (a < b) {
        a++;
        b--;
    }

    unsigned c = rand();
    if(a>3 && b<3){
        switch (a) {
            case 3:
                b = 99;
                break;
            case 6:
            case 7:
                b = a*2;
                break;
            default:
                c = c%13;
        }
    } else {
        a = 88;
    }

    printf("%d\n", a);
    printf("%d\n", b);
    printf("%d\n", c);
}