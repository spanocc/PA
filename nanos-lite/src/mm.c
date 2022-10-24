#include <memory.h>

static void *pf = NULL;

void* new_page(size_t nr_page) {  printf("AAA:%p   %p\n",heap.start,heap.end);
  if(pf == NULL) pf = (void *)heap.end; //初始化   
  char *np = (char *)pf;
  void *op = pf;
  np -= (1024 * 4 * nr_page);
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
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
