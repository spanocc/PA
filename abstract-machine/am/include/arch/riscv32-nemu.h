#ifndef ARCH_H__
#define ARCH_H__

//从栈顶往上（低地址到高地址）依次是 寄存器 cause status epc

struct Context {
  // TODO: fix the order of these members to match trap.S
  uintptr_t gpr[32], mcause, mstatus, mepc;
  void *pdir;
};

#define GPR1 gpr[17] // a7
#define GPR2 gpr[0]
#define GPR3 gpr[1]
#define GPR4 gpr[2]
#define GPRx gpr[0]

#endif
