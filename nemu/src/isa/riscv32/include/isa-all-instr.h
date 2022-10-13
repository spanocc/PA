#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_LIST(f) f(lui) f(lw) f(sw) f(inv) f(nemu_trap) \
                      f(auipc) f(jal) f(jalr) \
                      f(add) f(sub) f(sll) f(slt) f(sltu) f(xor) f(srl) f(sra) f(or) f(and) \
                      f(addi) f(slti) f(sltiu) f(xori) f(ori) f(andi) f(slli) f(srli) f(srai) \
                      f(beq) f(bne) f(blt) f(bge) f(bltu) f(bgeu) \
                      f(lb) f(lh) f(lbu) f(lhu) f(sb) f(sh) \
                      f(mul) f(mulh) f(divq) f(rem) f(divu) f(mulhu) f(remu) \
                      f(csrrw) f(csrrs) f(csrrc) f(csrrwi) f(csrrsi) f(csrrci) f(ecall) 
                      


def_all_EXEC_ID();

//f(csrrw) f(csrrs) f(csrrc) f(csrrwi) f(csrrsi) f(csrrci) f(ecall) 
