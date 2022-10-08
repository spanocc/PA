#include <isa.h>
#include <cpu/difftest.h>
#include "../local-include/reg.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
    bool flag = true;
    if(pc != cpu.pc) {
        flag = false;
        printf("pc fault!   spike: 0x%x nemu: 0x%x\n",pc,cpu.pc);
    }
    for(int i = 0; i < 32; ++i) {
        if(ref_r->gpr[i]._32 != cpu.gpr[i]._32) {
            flag = false;
            printf("reg%-2d fault! spike: %u nemu: %u\n",i,ref_r->gpr[i]._32, cpu.gpr[i]._32);
        }

    }
    return flag;
}

void isa_difftest_attach() {
}
