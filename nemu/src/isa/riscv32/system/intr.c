#include <isa.h>

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  cpu.mepc = epc;
  cpu.mcause = NO;
  vaddr_t in_addr = cpu.mtvec;
#ifdef CONFIG_ETRACE
printf("\nexception trace:\nmepc: 0x%x\nmcause: %d\nmtvec: 0x%x\n\n",cpu.mepc,cpu.mcause,cpu.mtvec);

#endif

  return in_addr;

}

word_t isa_query_intr() {
  return INTR_EMPTY;
}
