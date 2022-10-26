#include <am.h>
#include <nemu.h>
#include <klib.h>

static AddrSpace kas = {};
static void* (*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static Area segments[] = {      // Kernel memory mappings  内核的映射，应用的映射由protect创建
  NEMU_PADDR_SPACE
};                              //从0x80000000开始的内存，虚拟地址和物理地址相同，都属于内核空间

#define USER_SPACE RANGE(0x40000000, 0x80000000)

static inline void set_satp(void *pdir) {
  uintptr_t mode = 1ul << (__riscv_xlen - 1);
  asm volatile("csrw satp, %0" : : "r"(mode | ((uintptr_t)pdir >> 12)));
}

static inline uintptr_t get_satp() {
  uintptr_t satp;
  asm volatile("csrr %0, satp" : "=r"(satp));
  return satp << 12;
}

bool vme_init(void* (*pgalloc_f)(int), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  kas.ptr = pgalloc_f(PGSIZE);   //清0的    

  printf("ptr:%p\n",kas.ptr);

  int i;
  for (i = 0; i < LENGTH(segments); i ++) {
    void *va = segments[i].start;
    for (; va < segments[i].end; va += PGSIZE) {
      map(&kas, va, va, 0);
    }
  }

  set_satp(kas.ptr);
  vme_enable = 1;

  return true;
}

void protect(AddrSpace *as) {
  PTE *updir = (PTE*)(pgalloc_usr(PGSIZE));   //PTE是表项
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space
  memcpy(updir, kas.ptr, PGSIZE);  //所有用户进程的内核区域的映射都是相同的
}

void unprotect(AddrSpace *as) {
}

void __am_get_cur_as(Context *c) {
  c->pdir = (vme_enable ? (void *)get_satp() : NULL);
}

void __am_switch(Context *c) {
  if (vme_enable && c->pdir != NULL) {
    set_satp(c->pdir);
  }
}

//对于虚拟地址va 分成 10 10 12 三部分，分别为 一级page table的偏移  二级page table的偏移  和页内偏移
//因为是页映射到页，所以最后12位页内偏移不用管
void map(AddrSpace *as, void *va, void *pa, int prot) {
  //若这一表项还未分配出去 第0位(V) 是0 反之是1,我们把表项(PTE)的高20位当作下一级页表的基址(或者是物理页的基址)
  uintptr_t dir, page, offset;
  dir = (((uintptr_t)va) >> 22) & 0x3ff; 
  page = (((uintptr_t)va) >> 12) & 0x3ff;
  offset = ((uintptr_t)va) & 0xfff;                  
                                                  //if((uintptr_t)va == 0x8073d000) printf("%d %d\n",dir,page);
  assert(offset == 0); //按页分配，则offset为0
  assert((((uintptr_t)pa) & 0xfff) == 0);

  PTE *dir_p, *page_p; 
  dir_p = (PTE *)as->ptr + dir;
  if(((*dir_p) & 1 ) == 0) { //未分配页
    *dir_p = (PTE)(pgalloc_usr(PGSIZE)); //这个地址后12位一定为0；
    *dir_p |= 1; //表示已分配
  }
  page_p = (PTE *)((*dir_p) & (~0xfff)) + page; //表项后12位清0，作为二级页表的基址
  *page_p = (PTE)pa | 1;
                                //if((uintptr_t)va == 0x8073d000) printf("%p %p\n",dir_p,page_p);
}

Context *ucontext(AddrSpace *as, Area kstack, void *entry) {
  Context *kcon = (Context *)(kstack.end - sizeof(Context));
  kcon->gpr[0] = 0;
  kcon->mstatus = 0x1800;
  kcon->mepc = (uintptr_t)entry;

  kcon->pdir = as->ptr;

  return kcon;
}
