#include <stdio.h>
#include <stdlib.h>
// test program:
// simple loop

int main(int argc, char const *argv[]) {
    int x = 42;
    //x += 7;
    int y = 2;

    for(int i=0; i<y;i++){
        x+=3;
    }

    printf("%d\n", x);
}