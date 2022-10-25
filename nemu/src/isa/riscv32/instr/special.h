#ifdef CONFIG_FTRACE
void ftrace_display();
#endif


def_EHelper(inv) {
  rtl_hostcall(s, HOSTCALL_INV, NULL, NULL, NULL, 0);
}

def_EHelper(nemu_trap) {
  rtl_hostcall(s, HOSTCALL_EXIT, NULL, &gpr(10), NULL, 0); // gpr(10) is $a0
}
/*
RISC-V 中通过 ecall 指令进行 Syscall 的调用。 
ecall 指令会将 CPU 从用户态转换到内核态，并跳转到 Syscall 的入口处。通过 a7 寄存器来标识是哪个 Syscall。
至于调用 Syscall 要传递的参数则可以依次使用 a0-a5 这 6 个寄存器来存储。
*/
//从 M-mode 进行环境调用,异常号是11，我们的ecall环境调用都是从M（最高级）模式调用的，所以ecall的异常号都是11
def_EHelper(ecall) { 
  vaddr_t in_addr = isa_raise_intr(11, cpu.pc);  //返回的是下一条指令的pc
  rtl_j(s, in_addr);
#ifdef CONFIG_FTRACE 
  ftrace_display(in_addr, 3);  // 3 代表ECALL_TYPE
#endif
}

def_EHelper(mret) {
  rtl_j(s, cpu.mepc);
#ifdef CONFIG_FTRACE 
  ftrace_display(cpu.pc, 2);  // 2 代表mret
#endif
}

def_EHelper(csrrw) {   printf("666:%d %d %d\n",*ddest,*dsrc1,*dsrc2);
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
