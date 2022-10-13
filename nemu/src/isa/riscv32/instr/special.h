def_EHelper(inv) {
  rtl_hostcall(s, HOSTCALL_INV, NULL, NULL, NULL, 0);
}

def_EHelper(nemu_trap) {
  rtl_hostcall(s, HOSTCALL_EXIT, NULL, &gpr(10), NULL, 0); // gpr(10) is $a0
}

def_EHelper(ecall) {
  vaddr_t in_addr = isa_raise_intr(gpr(17), cpu.pc);
  rtl_j(s, in_addr);
}

def_EHelper(csrrw) {
  rtl_mv(s, ddest, dsrc2);
  rtl_mv(s, dsrc2, dsrc1);
}

def_EHelper(csrrs) {
  rtl_mv(s, ddest, dsrc2);
  rtl_or(s, dsrc2, dsrc1, dsrc2);
}

def_EHelper(csrrc) {
  rtl_mv(s, ddest, dsrc2);
  //将目的寄存器crs作为中间寄存器
  rtl_not(s, dsrc2, dsrc1);
  rtl_and(s, dsrc2, dsrc2, ddest);
}

def_EHelper(csrrwi) {
  rtl_mv(s, ddest, dsrc2);
  rtl_li(s, dsrc2, id_src1->imm);
}

def_EHelper(csrrsi) {
  rtl_mv(s, ddest, dsrc2);
  rtl_ori(s, dsrc2, dsrc2, id_src1->imm);
}

def_EHelper(csrrci) {
  rtl_mv(s, ddest, dsrc2);
  rtl_andi(s, dsrc2, dsrc2, ~id_src1->imm);
}
