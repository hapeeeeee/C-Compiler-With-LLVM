#include "stdio.h"
int main () {
    int a1 = 1;
    int a2 = 0;
    int a3 = 1;
    int a4 = 1159;
    int a5 = 1;
    int a6 = 1;
    int a7 = 1;
    int a8 = 1984;

    int b1 = a1 && 1;
    int b2 = a2 || 0;
    int b3 = a3 << 2;
    int b4 = a4 >> 2;
    int b5 = a5 & 11; 
    int b6 = a6 | 7;
    int b7 = a7 ^ 78;
    int b8 = a8 % 10;


    printf("%d\n", b8 + b7 + b6 + b5 + b4 + b3 + b2 + b1);
}