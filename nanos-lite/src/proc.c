#include <proc.h>

#define MAX_NR_PROC 4

void naive_uload(PCB *pcb, const char *filename);
void context_kload(PCB *new_pcb, void (*entry)(void *), void *arg);

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb() {
  current = &pcb_boot;   
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite with arg '%s' for the %dth time!", (char *)arg, j);
    j ++;
    yield();
  }
}

void init_proc() {

  context_kload(&pcb[0], hello_fun, "Adachi");
  context_kload(&pcb[1], hello_fun, "Shimamura");

  switch_boot_pcb();

  Log("Initializing processes...");

  // load program here
  naive_uload(NULL, "/bin/dummy");
}

Context* schedule(Context *prev) {
  // save the context pointer
  current->cp = prev;   //保存当前进程的上下文(sp)

  // always select pcb[0] as the new process
  // current = &pcb[0];
  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);

  // then return the new context
  return current->cp;

  //return NULL;
}

void context_kload(PCB *new_pcb, void (*entry)(void *), void *arg) {
  Area kstack;
  kstack.start = new_pcb->stack;
  kstack.end = new_pcb->stack + STACK_SIZE;
  new_pcb->cp = kcontext(kstack, entry, arg);
}

