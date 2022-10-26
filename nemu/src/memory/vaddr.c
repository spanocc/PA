#include <isa.h>
#include <memory/paddr.h>

word_t vaddr_ifetch(vaddr_t addr, int len) {

  if(isa_mmu_check(addr, len, MEM_TYPE_IFETCH) == MMU_TRANSLATE) {
    paddr_t pa = isa_mmu_translate(addr, len, MEM_TYPE_IFETCH);
    pa |= (addr & 0xfff);
    //assert(pa == addr); 
    return paddr_read(pa, len);
  }

  return paddr_read(addr, len);
}

word_t vaddr_read(vaddr_t addr, int len) {  if(addr == 4) return paddr_read(addr,len);

  if(isa_mmu_check(addr, len, MEM_TYPE_READ) == MMU_TRANSLATE) {
    paddr_t pa = isa_mmu_translate(addr, len, MEM_TYPE_READ);
    pa |= (addr & 0xfff);        //printf("ADRL::%x %x\n",pa,addr);   //assert(0);
    //assert(pa == addr);   
    return paddr_read(pa, len);
  }

  return paddr_read(addr, len);
}

void vaddr_write(vaddr_t addr, int len, word_t data) {

  if(isa_mmu_check(addr, len, MEM_TYPE_WRITE) == MMU_TRANSLATE) {
    paddr_t pa = isa_mmu_translate(addr, len, MEM_TYPE_WRITE);
    pa |= (addr & 0xfff);       // printf("ADRS::%x %x\n",pa,addr);   assert(0);
    //assert(pa == addr); 
    paddr_write(pa, len, data); 
    return;
  }

  paddr_write(addr, len, data);
}
