#include <common.h>
#include "syscall.h"

#ifdef CONFIG_STRACE
printf("sys_yield() == %d\n", c->GPRx);
#endif

uintptr_t sys_yield();
void sys_exit(uintptr_t);
void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
    case SYS_yield:
      c->GPRx = sys_yield();

      #ifdef CONFIG_STRACE
        printf("sys_yield() == %d\n", c->GPRx);
      #endif

      break;
    case SYS_exit:
    
      #ifdef CONFIG_STRACE
        printf("sys_exit(%d) == void\n", a[0]);
      #endif

      sys_exit(a[0]);
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}


uintptr_t sys_yield() {
  yield();
  return 0;
}

void sys_exit(uintptr_t _exit) {
  halt(_exit);
}