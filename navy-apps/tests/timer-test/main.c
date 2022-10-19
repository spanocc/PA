#include<stdio.h>
#include<NDL.h>

int main() {
    uint32_t msec = 1000;
    while(1) {
        while(NDL_GetTicks() < msec);
        printf("%dms\n",msec);
        msec += 1000;
    }
}