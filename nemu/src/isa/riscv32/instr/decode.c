#include "../local-include/reg.h"
#include <cpu/ifetch.h>
#include <isa-all-instr.h>

def_all_THelper();

static uint32_t get_instr(Decode *s) {
  return s->isa.instr.val;
}

// decode operand helper
#define def_DopHelper(name) \
  void concat(decode_op_, name) (Decode *s, Operand *op, word_t val, bool flag)

static def_DopHelper(i) {
  op->imm = val;
}

static def_DopHelper(r) {
  bool is_write = flag;
  static word_t zero_null = 0;
  //不能写0号寄存器,对0号寄存器的写操作，会在这个静态变量上进行
  op->preg = (is_write && val == 0) ? &zero_null : &gpr(val); //第val个寄存器
}

static def_DopHelper(c) {
  //printf("0x%x\n",val);
  switch(val) {
    case 0x341:
      op->preg = &cpu.mepc; break;
    case 0x300:
      op->preg = &cpu.mstatus; break;
    case 0x342:
      op->preg = &cpu.mcause; break;
    case 0x305:
      op->preg = &cpu.mtvec; break;
    case 0x180:
      op->preg = &cpu.satp; break;
    default: printf("0x%x\n",val); assert(0);
  }
}

static def_DHelper(I) {
  decode_op_r(s, id_src1, s->isa.instr.i.rs1, false);
  decode_op_i(s, id_src2, s->isa.instr.i.simm11_0, false); //将12位的int 传递给32位的无符号整数会有符号扩展
  decode_op_r(s, id_dest, s->isa.instr.i.rd, true);
}

static def_DHelper(U) {
  decode_op_i(s, id_src1, s->isa.instr.u.imm31_12 << 12, true);
  decode_op_r(s, id_dest, s->isa.instr.u.rd, true);
}
//src1寄存器的值是基址 ，立即数是偏移， src2寄存器的值是写入的值
static def_DHelper(S) {
  decode_op_r(s, id_src1, s->isa.instr.s.rs1, false);
  sword_t simm = (s->isa.instr.s.simm11_5 << 5) | s->isa.instr.s.imm4_0;  //符号扩展在这一步,将12位的数符号扩展到32位
  decode_op_i(s, id_src2, simm, false);
  decode_op_r(s, id_dest, s->isa.instr.s.rs2, false);
}

static def_DHelper(B) {
  decode_op_r(s, id_src1, s->isa.instr.b.rs1, false);                                              
  decode_op_r(s, id_src2, s->isa.instr.b.rs2, false);
  sword_t simm = (s->isa.instr.b.imm4_1 | (s->isa.instr.b.imm10_5 << 4) | (s->isa.instr.b.imm11 << 10) | 
               (s->isa.instr.b.simm12 << 11) ) << 1;
  decode_op_i(s, id_dest, simm, false);
}

static def_DHelper(J) {
  sword_t simm = (s->isa.instr.j.imm10_1 | (s->isa.instr.j.imm11 << 10) | (s->isa.instr.j.imm19_12 << 11) |
               (s->isa.instr.j.simm20 << 19)) << 1;
  decode_op_i(s, id_src1, simm, false);
  decode_op_r(s, id_dest, s->isa.instr.j.rd, true);
}

static def_DHelper(R) {
  decode_op_r(s, id_src1, s->isa.instr.r.rs1, false);                                              
  decode_op_r(s, id_src2, s->isa.instr.r.rs2, false);
  decode_op_r(s, id_dest, s->isa.instr.r.rd, true);
}

static def_DHelper(C) {
  decode_op_r(s, id_dest, s->isa.instr.c.rd, true);
  decode_op_r(s, id_src1, s->isa.instr.c.rs1, false);
  decode_op_c(s, id_src2, s->isa.instr.c.csr, true);
}

static def_DHelper(CI) {
  decode_op_r(s, id_dest, s->isa.instr.ci.rd, true);
  decode_op_i(s, id_src1, s->isa.instr.ci.uimm4_0, false);
  decode_op_c(s, id_src2, s->isa.instr.ci.csr, true);
}


//TODO() 要定义一个def_DHelper(empty) 什么也不做


def_THelper(load) {
  def_INSTR_TAB("??????? ????? ????? 010 ????? ????? ??", lw); //成功匹配会return  EXEC_ID_lw

  def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", lb);
  def_INSTR_TAB("??????? ????? ????? 001 ????? ????? ??", lh);
  def_INSTR_TAB("??????? ????? ????? 100 ????? ????? ??", lbu);
  def_INSTR_TAB("??????? ????? ????? 101 ????? ????? ??", lhu);

  return EXEC_ID_inv; //都没匹配上，会return EXEC_ID_inv 标识非法指令的ID.
}

def_THelper(store) {
  def_INSTR_TAB("??????? ????? ????? 010 ????? ????? ??", sw);

  def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", sb); 
  def_INSTR_TAB("??????? ????? ????? 001 ????? ????? ??", sh);

  return EXEC_ID_inv;
}

def_THelper(icr) {
  def_INSTR_TAB("0000000 ????? ????? 000 ????? ????? ??", add);
  def_INSTR_TAB("0100000 ????? ????? 000 ????? ????? ??", sub);
  def_INSTR_TAB("0000000 ????? ????? 001 ????? ????? ??", sll);
  def_INSTR_TAB("0000000 ????? ????? 010 ????? ????? ??", slt);
  def_INSTR_TAB("0000000 ????? ????? 011 ????? ????? ??", sltu);
  def_INSTR_TAB("0000000 ????? ????? 100 ????? ????? ??", xor);
  def_INSTR_TAB("0000000 ????? ????? 101 ????? ????? ??", srl);
  def_INSTR_TAB("0100000 ????? ????? 101 ????? ????? ??", sra);
  def_INSTR_TAB("0000000 ????? ????? 110 ????? ????? ??", or);
  def_INSTR_TAB("0000000 ????? ????? 111 ????? ????? ??", and);

//额外加的命令
  def_INSTR_TAB("0000001 ????? ????? 000 ????? ????? ??", mul);
  def_INSTR_TAB("0000001 ????? ????? 001 ????? ????? ??", mulh);
  def_INSTR_TAB("0000001 ????? ????? 011 ????? ????? ??", mulhu);
  def_INSTR_TAB("0000001 ????? ????? 100 ????? ????? ??", divq);
  def_INSTR_TAB("0000001 ????? ????? 101 ????? ????? ??", divu);
  def_INSTR_TAB("0000001 ????? ????? 110 ????? ????? ??", rem); //取余数
  def_INSTR_TAB("0000001 ????? ????? 111 ????? ????? ??", remu);


  return EXEC_ID_inv;
}

def_THelper(ici) {
  def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", addi);
  def_INSTR_TAB("??????? ????? ????? 010 ????? ????? ??", slti);
  def_INSTR_TAB("??????? ????? ????? 011 ????? ????? ??", sltiu);
  def_INSTR_TAB("??????? ????? ????? 100 ????? ????? ??", xori);
  def_INSTR_TAB("??????? ????? ????? 110 ????? ????? ??", ori);
  def_INSTR_TAB("??????? ????? ????? 111 ????? ????? ??", andi);
  def_INSTR_TAB("??????? ????? ????? 001 ????? ????? ??", slli);
  def_INSTR_TAB("0000000 ????? ????? 101 ????? ????? ??", srli);
  def_INSTR_TAB("0100000 ????? ????? 101 ????? ????? ??", srai);
  return EXEC_ID_inv;
}

def_THelper(branch) {
  def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", beq);   
  def_INSTR_TAB("??????? ????? ????? 001 ????? ????? ??", bne);
  def_INSTR_TAB("??????? ????? ????? 100 ????? ????? ??", blt);
  def_INSTR_TAB("??????? ????? ????? 101 ????? ????? ??", bge);
  def_INSTR_TAB("??????? ????? ????? 110 ????? ????? ??", bltu);
  def_INSTR_TAB("??????? ????? ????? 111 ????? ????? ??", bgeu);
  return EXEC_ID_inv;
}

def_THelper(csr) {
  def_INSTR_TAB("??????? ????? ????? 001 ????? ????? ??", csrrw);   
  def_INSTR_TAB("??????? ????? ????? 010 ????? ????? ??", csrrs);
  def_INSTR_TAB("??????? ????? ????? 011 ????? ????? ??", csrrc);
  return EXEC_ID_inv;
}

def_THelper(csri) {
  def_INSTR_TAB("??????? ????? ????? 101 ????? ????? ??", csrrwi);   
  def_INSTR_TAB("??????? ????? ????? 110 ????? ????? ??", csrrsi);
  def_INSTR_TAB("??????? ????? ????? 111 ????? ????? ??", csrrci);
  return EXEC_ID_inv;
}



def_THelper(main) {
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 00000 11", I     , load);
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 01000 11", S     , store);
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 01101 11", U     , lui); //def_THelper(lui)函数定义已经在 def_all_THelper();里了

  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 00100 11", I    , ici); //立即数整数操作
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 01100 11", R    , icr); //寄存器整数操作
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 00101 11", U    , auipc);

  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 11000 11", B    , branch);
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 11011 11", J    , jal);
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 11001 11", I    , jalr);

  def_INSTR_TAB  ("0000000 00000 00000 000 00000 11100 11",        ecall);
  def_INSTR_TAB  ("0011000 00010 00000 000 00000 11100 11",        mret);
  def_INSTR_IDTAB("??????? ????? ????? 0?? ????? 11100 11", C    , csr);
  def_INSTR_IDTAB("??????? ????? ????? 1?? ????? 11100 11", CI   , csri);

  def_INSTR_TAB  ("??????? ????? ????? ??? ????? 11010 11",        nemu_trap);

  return table_inv(s);
};

int isa_fetch_decode(Decode *s) {
  s->isa.instr.val = instr_fetch(&s->snpc, 4);
  int idx = table_main(s);
  return idx;
}
