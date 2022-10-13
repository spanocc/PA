#include <isa.h>
#include "local-include/reg.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

void isa_reg_display() {
    for(int i = 0; i < 32; ++i) {
        printf("%-10s: %-15u0x%x\n",regs[i],cpu.gpr[i]._32, cpu.gpr[i]._32);
    }
    printf("\n%s: %-15u0x%x\n\n", "mepc", cpu.mepc, cpu.mepc);
    printf("\n%s: %-15u0x%x\n\n", "mstatus", cpu.mstatus, cpu.mstatus);
    printf("\n%s: %-15u0x%x\n\n", "mcause", cpu.mcause, cpu.mcause);
    printf("\n%s: %-15u0x%x\n\n", "mtvec", cpu.mtvec, cpu.mtvec);
    printf("\n%s: %-15u0x%x\n\n", "pc", cpu.pc, cpu.pc);
    return;
}

word_t isa_reg_str2val(const char *s, bool *success) { 
  for(int i = 0; i  < 32; ++i) {
    if(!strcmp(s, regs[i])) {
        *success = true;
        return cpu.gpr[i]._32;
    }
  }
  *success = false;
  return 0;
}
