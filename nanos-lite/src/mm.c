#include <memory.h>

static void *pf = NULL;


//返回的是这段内存的首地址
void* new_page(size_t nr_page) {   //printf("AAA:%p   %p\n",heap.start,heap.end);
 
  char *np = (char *)pf;
  void *op = pf;
  np += (1024 * 4 * nr_page);  //以heap.start初始化,所以要往上开辟空间
  pf = (void *)np;
  return op;
}

#ifdef HAS_VME
static void* pg_alloc(int n) {
  return NULL;
}
#endif

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);  //以heap.start初始化
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
