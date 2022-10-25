#include <isa.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>

paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type) {  //printf("S:%x\n",cpu.satp);
  //assert(cpu.satp >> 31);

  printf("%x\n",vaddr);

  paddr_t dir, page;
  dir = (((paddr_t)vaddr) >> 22) & 0x3ff; 
  page = (((paddr_t)vaddr) >> 12) & 0x3ff; // printf("xia:%d %d\n");

  paddr_t dir_p, page_p;
  dir_p = ((cpu.satp) << 12) + dir * 4;  printf("dir:%x\n",dir_p);
  uint32_t pte = paddr_read(dir_p, 1);
  assert((pte & 1) == 1);

  page_p = (pte & (~0xfff)) + page * 4;
  pte = paddr_read(page_p, 1);
  assert((pte & 1) == 1);

  paddr_t ret = ((pte) & (~0xfff)) | MEM_RET_OK;         printf("%d\n",ret);
  return ret;  
}
