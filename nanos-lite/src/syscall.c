#include <common.h>
#include "syscall.h"

#define CONFIG_STRACE
struct timeval {
    long int tv_sec;     // 秒数
    long int tv_usec;     // 微秒数
};
struct timezone {
    int tz_minuteswest;/*格林威治时间往西方的时差*/
    int tz_dsttime;    /*DST 时间的修正方式*/
};
int sys_yield();
void sys_exit(intptr_t);
size_t sys_write(int fd, const void * buf, size_t count);
size_t sys_read(int fd, void * buf, size_t count);
int sys_brk(intptr_t incr);
size_t sys_lseek(int fd, size_t offset, int whence);
int sys_close(int fd);
int sys_open(const char *pathname, int flags, int mode);
int sys_gettimeofday(struct timeval* tv, struct timezone* tz);


int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);

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
    case SYS_read:
      c->GPRx = sys_read(a[1], (void *)a[2], a[3]);

      #ifdef CONFIG_STRACE
        printf("sys_read(%d, %x, %d) == %d\n", (int)a[1], a[2], (int)a[3], (int)(c->GPRx));
      #endif

      break;
    case SYS_open:
      c->GPRx = sys_open((const char*)a[1], a[2], a[3]);

      #ifdef CONFIG_STRACE
        printf("sys_open(%s, %d, %d) == %d\n", (const char*)a[1], (int)a[2], (int)a[3], (int)(c->GPRx));
      #endif

      break;
    case SYS_lseek:
      c->GPRx = sys_lseek(a[1], a[2], a[3]);

      #ifdef CONFIG_STRACE
        printf("sys_lseek(%d, %d, %d) == %d\n", (int)a[1], (int)a[2], (int)a[3], (int)(c->GPRx));
      #endif

      break;
    case SYS_close:
      c->GPRx = sys_close(a[1]);

      #ifdef CONFIG_STRACE
        printf("sys_close(%d == %d\n", (int)a[1], (int)(c->GPRx));
      #endif

      break;
    case SYS_gettimeofday:
      #ifdef CONFIG_STRACE
        printf("sys_gettimeofday()\n");
      #endif

      c->GPRx = sys_gettimeofday((struct timeval*)a[1], (struct timezone*)a[2]);
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

size_t sys_write(int fd, const void * buf, size_t count) {
  /*if(fd == 1 || fd == 2) {
    for(uintptr_t i = 0; i < count; ++i) {
      const char *c = buf;
      putch(*(c + i));
    }
    return count;
  }*/
  //else {
    return fs_write(fd, buf, count);
  //}
}

int sys_brk(intptr_t incr) {

  return 0;
}

size_t sys_read(int fd, void * buf, size_t count) {
  return fs_read(fd, buf, count);
}

int sys_open(const char *pathname, int flags, int mode) {
  return fs_open(pathname, flags, mode);
}

size_t sys_lseek(int fd, size_t offset, int whence) {
  return fs_lseek(fd, offset, whence);
}

int sys_close(int fd) {
  return fs_close(fd);
}

int sys_gettimeofday(struct timeval* tv, struct timezone* tz) {
  uint64_t us = io_read(AM_TIMER_UPTIME).us;
  tv->tv_sec = us / 1000000;
  tv->tv_usec = us - (tv->tv_sec * 1000000);
  return 0;
}