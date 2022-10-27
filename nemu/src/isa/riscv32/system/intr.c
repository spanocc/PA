#include <isa.h>

#define IRQ_TIMER 0x80000007  // for riscv32

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  cpu.mepc = epc;
  cpu.mcause = NO;
  vaddr_t in_addr = cpu.mtvec;
  //mstatus 0 - 31位  MIE -> 3  MPIE -> 7
  uint32_t mie = ((cpu.mstatus >> 3) & 1);
  cpu.mstatus = (cpu.mstatus & (~(1 << 7))) | (mie << 7);
  cpu.mstatus = cpu.mstatus & (~(1 << 3));


#ifdef CONFIG_ETRACE
printf("\nexception trace:\nmepc: 0x%x\nmcause: %d\nmtvec: 0x%x\n\n",cpu.mepc,cpu.mcause,cpu.mtvec);
#endif

  return in_addr;

}

word_t isa_query_intr() {
  if (((cpu.mstatus >> 3) & 1) && cpu.INTR == true) {
    cpu.INTR = false;
    return IRQ_TIMER;
  }
  return INTR_EMPTY;
}
