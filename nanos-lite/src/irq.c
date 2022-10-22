#include <common.h>


void do_syscall(Context *c);
Context* schedule(Context *prev);

static Context* do_event(Event e, Context* c) {
  switch (e.event) {
    case EVENT_YIELD:
      printf("event yield!\n");  printf("%p\n",c);
      //c->mepc += 4;//在软件中更改栈中mepc的数值,不过不能在这加，因为Context是架构相关的，不能在操作系统中访问Context内部成员
      return schedule(c);
      break;
    case EVENT_SYSCALL:
      do_syscall(c);
      break;
    case EVENT_IRQ_TIMER:
      printf("clock interrupt!");
      break;;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
