#include "stdio.h"
int main () {
    int b = 0;
    for (int i = 0; i < 10 ; i = i + 1) {
        b = b + i;       
        if (i == 5) {    
            break;
            b;
        }
        b = b + i;       
        continue;        
        b = b + i;       
    }
    printf("%d\n", b);
}