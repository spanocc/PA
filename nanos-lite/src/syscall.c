#include <common.h>
#include "syscall.h"

//#define CONFIG_STRACE

int sys_yield();
void sys_exit(intptr_t);
int sys_write(int fd, const void * buf, size_t count);
int sys_brk(intptr_t incr);

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  switch (a[0]) {
    case SYS_yield:
      c->GPRx = sys_yield();

      #ifdef CONFIG_STRACE
        printf("sys_yield() == %d\n", (int)(c->GPRx));
      #endif

      break;
    case SYS_exit:

      #ifdef CONFIG_STRACE
        printf("sys_exit(%d) == void\n", (int)a[1]); 
      #endif

      sys_exit(a[1]);
      break;
    case SYS_write:
      c->GPRx = sys_write(a[1], (void *)a[2], a[3]);
      
      #ifdef CONFIG_STRACE
        printf("sys_write(%d, %x, %d) == %d\n", (int)a[1], a[2], (int)a[3], (int)(c->GPRx));
      #endif
    
      break;
    case SYS_brk:
      c->GPRx = sys_brk(a[1]);
      
      #ifdef CONFIG_STRACE
        printf("sys_brk(%d) == %d\n", (int)a[1], (int)(c->GPRx));
      #endif

      break;

    default: panic("Unhandled syscall ID = %d", (int)a[0]);
  }
}


int sys_yield() {
  yield();
  return 0;
}

void sys_exit(intptr_t _exit) {
  halt(_exit);
}

int sys_write(int fd, const void * buf, size_t count) {
  if(fd == 1 || fd == 2) {
    for(uintptr_t i = 0; i < count; ++i) {
      const char *c = buf;
      putch(*(c + i));
    }
    return count;
  }
  return 0;
}

int sys_brk(intptr_t incr) {

  return 0;
}