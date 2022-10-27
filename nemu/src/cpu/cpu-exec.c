#include <cpu/cpu.h>
#include <cpu/exec.h>
#include <cpu/difftest.h>
#include <isa-all-instr.h>
#include <locale.h>

/* The assembly code of instructions executed is only output to the screen
 * when the number of instructions executed is less than this value.
 * This is useful when you use the `si' command.
 * You can modify this value as you want.
 */
#define MAX_INSTR_TO_PRINT 10

CPU_state cpu = {};
uint64_t g_nr_guest_instr = 0;
static uint64_t g_timer = 0; // unit: us
static bool g_print_step = false;
const rtlreg_t rzero = 0;
rtlreg_t tmp_reg[4];

#ifdef CONFIG_ITRACE
//按照Decode的logbuf成员来定义
#define IRINGBUF_SIZE 20  //指令环形缓冲区大小
char iringbuf[IRINGBUF_SIZE][128];
char (*iringbuf_start)[128] = iringbuf;
char (*iringbuf_end)[128] = iringbuf + IRINGBUF_SIZE;
char (*ptirb)[128] = iringbuf;
static vaddr_t now_pc;

void add_to_irbuf(Decode *s) {  
    if(ptirb == iringbuf_end) ptirb = iringbuf_start;
    strcpy(*ptirb, s->logbuf);                               // printf("%s  111\n",*ptirb);
    ptirb++;
    now_pc = s->pc;
}

void irbuf_display() {
    for(int i = 0; i < IRINGBUF_SIZE; ++i) {
        vaddr_t adr_tmp = 0;
        sscanf(*(iringbuf+i), "%x", &adr_tmp);
        if(adr_tmp == now_pc) printf(" --> ");
        else printf("     ");
        printf("%s\n", *(iringbuf+i));
    }
    //printf("%x %x\n",cpu.pc, now_pc);
}
#endif


void device_update();
void fetch_decode(Decode *s, vaddr_t pc);

#ifdef CONFIG_WATCHPOINT
int check_watchpoint(vaddr_t); //检查监视点 ~/ics2021/nemu/src/monitor/sdb/watchpoint.c
#endif

//该函数是在一条指令执行完才调用的，但是在执行指令之前就要把指令的logbuf放在iringbuf里，所以不能在这个函数里实现
//但是查看watchpoint的值要在指令执行之后，所以要放在这个函数里
static void trace_and_difftest(Decode *_this, vaddr_t dnpc) {
  //make menuconfig中开启 Only trace instructions when the condition is true 
#ifdef CONFIG_ITRACE_COND
  if (ITRACE_COND) log_write("%s\n", _this->logbuf); 
#endif
  //make menuconfig中开启Enable instruction tracer 
  if (g_print_step) { IFDEF(CONFIG_ITRACE, puts(_this->logbuf)); }  //if n < 10 ,print each step. else don't print
  IFDEF(CONFIG_DIFFTEST, difftest_step(_this->pc, dnpc));
 
  //todo()
#ifdef CONFIG_WATCHPOINT
   if(check_watchpoint(_this->pc) == 1) {
       nemu_state.state = NEMU_STOP;
   }
#endif

}

#include <isa-exec.h>

#define FILL_EXEC_TABLE(name) [concat(EXEC_ID_, name)] = concat(exec_, name),//[i] = p  表示第i个元素的值是p(函数指针)
static const void* g_exec_table[TOTAL_INSTR] = {
  MAP(INSTR_LIST, FILL_EXEC_TABLE)
};

static void fetch_decode_exec_updatepc(Decode *s) {
  fetch_decode(s, cpu.pc);
  s->EHelper(s);//调用函数
  
  //TODO()可以用TODO(）宏来测试iringbuf

  cpu.pc = s->dnpc;
}

static void statistic() {
  IFNDEF(CONFIG_TARGET_AM, setlocale(LC_NUMERIC, ""));
#define NUMBERIC_FMT MUXDEF(CONFIG_TARGET_AM, "%ld", "%'ld")
  Log("host time spent = " NUMBERIC_FMT " us", g_timer);
  Log("total guest instructions = " NUMBERIC_FMT, g_nr_guest_instr);
  if (g_timer > 0) Log("simulation frequency = " NUMBERIC_FMT " instr/s", g_nr_guest_instr * 1000000 / g_timer);
  else Log("Finish running in less than 1 us and can not calculate the simulation frequency");
}
//出错时会调用(在debug.h中)  x 1 0 或者 TODO()
void assert_fail_msg() {
  isa_reg_display();
#ifdef CONFIG_ITRACE
  irbuf_display();
#endif
  statistic();
}

void fetch_decode(Decode *s, vaddr_t pc) {
  s->pc = pc;
  s->snpc = pc;
  int idx = isa_fetch_decode(s);
  s->dnpc = s->snpc;
  s->EHelper = g_exec_table[idx];
#ifdef CONFIG_ITRACE
  char *p = s->logbuf;
  p += snprintf(p, sizeof(s->logbuf), FMT_WORD ":", s->pc);
  int ilen = s->snpc - s->pc;
  int i;
  uint8_t *instr = (uint8_t *)&s->isa.instr.val;
  for (i = 0; i < ilen; i ++) {
    p += snprintf(p, 4, " %02x", instr[i]);
  }
  int ilen_max = MUXDEF(CONFIG_ISA_x86, 8, 4);
  int space_len = ilen_max - ilen;
  if (space_len < 0) space_len = 0;
  space_len = space_len * 3 + 1;
  memset(p, ' ', space_len);
  p += space_len;

  void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);
  disassemble(p, s->logbuf + sizeof(s->logbuf) - p,
      MUXDEF(CONFIG_ISA_x86, s->snpc, s->pc), (uint8_t *)&s->isa.instr.val, ilen);


  //在指令环形缓冲区添加指令(在执行指令之前)
  add_to_irbuf(s);


#endif
}

/* Simulate how the CPU works. */
void cpu_exec(uint64_t n) {
  g_print_step = (n < MAX_INSTR_TO_PRINT);// if n < 10 ,print each step. else don't print
  switch (nemu_state.state) {
    case NEMU_END: case NEMU_ABORT:
      printf("Program execution has ended. To restart the program, exit NEMU and run again.\n");
      return;
    default: nemu_state.state = NEMU_RUNNING;
  }

  uint64_t timer_start = get_time();

  Decode s;
  for (;n > 0; n --) {                       
    fetch_decode_exec_updatepc(&s);
    g_nr_guest_instr ++;
    trace_and_difftest(&s, cpu.pc);
    if (nemu_state.state != NEMU_RUNNING) break;
    IFDEF(CONFIG_DEVICE, device_update());

    word_t intr = isa_query_intr();
    if (intr != INTR_EMPTY) {
      cpu.pc = isa_raise_intr(intr, cpu.pc);
    }

  }

  uint64_t timer_end = get_time();
  g_timer += timer_end - timer_start;

  switch (nemu_state.state) {
    case NEMU_RUNNING: nemu_state.state = NEMU_STOP; break;

    case NEMU_END: case NEMU_ABORT:
      Log("nemu: %s at pc = " FMT_WORD,
          (nemu_state.state == NEMU_ABORT ? ASNI_FMT("ABORT", ASNI_FG_RED) :
           (nemu_state.halt_ret == 0 ? ASNI_FMT("HIT GOOD TRAP", ASNI_FG_GREEN) :
            ASNI_FMT("HIT BAD TRAP", ASNI_FG_RED))),
          nemu_state.halt_pc);
      // fall through
    case NEMU_QUIT: statistic();
  }
}
