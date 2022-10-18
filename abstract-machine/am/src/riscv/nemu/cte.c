#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;

Context* __am_irq_handle(Context *c) {  //这个c指针是trap.s汇编代码中变出来的参数sp指针，而不是在c语言中创建的
 
  /*for(int i = 0 ; i < 32; i++) printf("%d ",c->gpr[i]);
  printf("\n%d\n%d\n%d\n",c->mcause,c->mstatus,c->mepc);*/

  if (user_handler) {
    Event ev = {0};
    switch (c->mcause) {
      case 11:     //$a7 是第一个参数，也就是syscall函数中的type参数，且type从0开始（enum类型）
        printf("mcause == 11\n");
        printf("$a7 == %d\n\n", c->gpr[17]);        //printf("%x %x %x\n\n", (c->gpr[0]),c->gpr[1],c->gpr[2]);
        if(c->gpr[17] == -1) {
          ev.event = EVENT_YIELD; 
          c->mepc += 4;
          break;
        }else {
          ev.event = EVENT_SYSCALL;
          c->mepc += 4;
          break;
        }
      default: ev.event = EVENT_ERROR; break;
    }

    c = user_handler(ev, c);
    assert(c != NULL);
  }

  return c;   //返回c的指针，同时也是sp的指针，所以sp可以不用保存
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  //把_am_asm_trap（在trap.s中）的地址作为异常入口地址
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));  //asm volatile是内联汇编指令，类似于之前的halt指令，不过csrw mtvec是riscv32可以识别的指令（本来就有的指令，这点与halt不同）

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  return NULL;
}

void yield() {
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
