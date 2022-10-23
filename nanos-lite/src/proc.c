#include <proc.h>

#define MAX_NR_PROC 4

void naive_uload(PCB *pcb, const char *filename);
void context_kload(PCB *new_pcb, void (*entry)(void *), void *arg);
void context_uload(PCB *new_pcb, const char *file_name);

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
  // kload用am的栈（_stack_pointer），uload用heap.end的栈
  context_kload(&pcb[0], hello_fun, "Adachi");
  //context_kload(&pcb[1], hello_fun, "Shimamura");
  context_uload(&pcb[1], "/bin/bird");       //如果两个都是uload，那么这两个用户程序的用户栈是一样的，会相互覆盖，发生错误

  switch_boot_pcb();

  Log("Initializing processes...");

  // load program here
   naive_uload(NULL, "/bin/bird");
  // yield();
}

Context* schedule(Context *prev) {
  // save the context pointer
  current->cp = prev;   //保存当前进程的上下文(sp)
//printf("adr:%p\n",&(pcb[0].cp));
  // always select pcb[0] as the new process
  // current = &pcb[0];
  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
//printf("adr:%p  %p\n",pcb[0].cp,pcb[1].cp);
  // then return the new context
  return current->cp;    

  //return NULL;
}



