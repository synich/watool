#include <stdio.h>

int main(){
    int i = 32;
    for (; i<32+24; i++){
      printf("0x%x,%d: %c\t\t0x%x,%d: %c\t\t0x%x,%d: %c\t\t0x%x,%d: %c\n",
        i, i, i, i+24, i+24, i+24, i+48, i+48, i+48, i+72, i+72, i+72);
    }
}
