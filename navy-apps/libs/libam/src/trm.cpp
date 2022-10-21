#include <am.h>

// 自己加的 
// #define PMEM_SIZE (128 * 1024 * 1024) 

Area heap; //= RANGE(sbrk(0), sbrk(PMEM_SIZE) + PMEM_SIZE);
/*heap.start = sbrk(PMEM_SIZE);
heap.end = heap.start + PMEM_SIZE;*/

void putch(char ch) {

    //write(1, &ch, 1);
}

void halt(int code) {

    //exit(code);
}
