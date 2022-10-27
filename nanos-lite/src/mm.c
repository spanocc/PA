#include <memory.h>

//后加的
#include<proc.h>

static void *pf = NULL;


//heap.start是从0x8073e000开始的地址1
//返回的是这段内存的首地址
//以new_page分配的地址后12位都是0
void* new_page(size_t nr_page) {   //printf("AAA:%p   %p\n",heap.start,heap.end);
 
  char *np = (char *)pf;
  void *op = pf;
  np += (1024 * 4 * nr_page);  //以heap.start初始化,所以要往上开辟空间
  pf = (void *)np;
  return op;
}

#ifdef HAS_VME
static void* pg_alloc(int n) {
  assert(n % 4096 == 0); //一定是4096的倍数
  void *ret = new_page(n / 4096);
  memset(ret, 0, n);
  return ret;
}
#endif

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
/* malloc()被第一次调用的时候, 会通过sbrk(0)来查询用户程序当前program break的位置 */ 
int mm_brk(uintptr_t brk) {    //printf("mm:%x\n",current->max_brk);
  uintptr_t start_page = (((current->max_brk) & 0xfff) == 0) ? current->max_brk : (((current->max_brk) & (~0xfff)) + 0x1000);
  uintptr_t end_page = ((brk & 0xfff) == 0) ? (brk - 0x1000) : (brk & (~0xfff));

  printf("BK:%x %x\n",start_page,end_page);

  for(;start_page <= end_page; start_page += 4096) {
    void *pa = new_page(1);
    map(&(current->as), (void *)start_page, pa, 0xffffffff);
  }

  if(current->max_brk < brk) current->max_brk = brk;
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);  //以heap.start初始化
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
