#ifndef ARCH_H__
#define ARCH_H__

//从栈顶往上（低地址到高地址）依次是 寄存器 cause status epc
//必须是  36 * 4 字节的大小
struct Context {
  // TODO: fix the order of these members to match trap.S
  union {
    struct {
      uintptr_t gpr[32], mcause, mstatus, mepc, np;
    };
    void *pdir;
  };
  
};

#define GPR1 gpr[17] // a7
#define GPR2 gpr[10]
#define GPR3 gpr[11]
#define GPR4 gpr[12]
#define GPRx gpr[10]

#endif
