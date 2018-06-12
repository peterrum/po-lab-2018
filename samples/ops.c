#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {

int a = 5;
int b = -2;

unsigned aU = 5;
unsigned bU = 2;

int cC = a+1;
int cV = a+b;

int dC = a-1;
int dV = a-b;

int eC = a*2;
int eV = a*b;

unsigned fuC = aU/5;
int fsC = a/5;
unsigned fuV = aU/bU;
int fsV = a/b;

unsigned guC = aU%5;
int gsC = a%5;
unsigned guV = aU%bU;
int gsV = a%b;

int hC = a << 1;
int hV = a << bU;

unsigned iuC = aU >> 1;
int isC = a >> 1;
unsigned iuV = aU >> bU;
int isV = a >> bU;

int jC = a & 1;
int jV = a & bU;

int kC = a | 1;
int kV = a | bU;

int lC = a^1;
int lV = a^bU;
}
