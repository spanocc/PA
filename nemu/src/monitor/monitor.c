#include <isa.h>
#include <memory/paddr.h>

void init_rand();
void init_log(const char *log_file);
void init_mem();
void init_difftest(char *ref_so_file, long img_size, int port);
void init_device();
void init_sdb();
void init_disasm(const char *triple);


static void welcome() {
  Log("Trace: %s", MUXDEF(CONFIG_TRACE, ASNI_FMT("ON", ASNI_FG_GREEN), ASNI_FMT("OFF", ASNI_FG_RED)));
  IFDEF(CONFIG_TRACE, Log("If trace is enabled, a log file will be generated "
        "to record the trace. This may lead to a large log file. "
        "If it is not necessary, you can disable it in menuconfig"));
  Log("Build time: %s, %s", __TIME__, __DATE__);
  printf("Welcome to %s-NEMU!\n", ASNI_FMT(str(__GUEST_ISA__), ASNI_FG_YELLOW ASNI_BG_RED));
  printf("For help, type \"help\"\n");
  Log("Exercise: Please remove me in the source code and compile NEMU again.");
  //assert(0);
}


#ifndef CONFIG_TARGET_AM
#include <getopt.h>
#include<elf.h>
void sdb_set_batch_mode();

static char *log_file = NULL;
static char *diff_so_file = NULL;
static char *img_file = NULL;
static int difftest_port = 1234;


#ifdef CONFIG_FTRACE
char elf_file_name[128];
Elf32_Sym sym_table[128];
int sym_num = 0;
char str_table[512];
static int space_num = 0;
enum func_type { CALL_TYPE, RET_TYPE };

void sys_table_display() {
    for(int i =0;i<sym_num;++i) printf("%-12x%-12x%s\n", sym_table[i].st_value, sym_table[i].st_size, str_table+sym_table[i].st_name);
}


void init_ftrace() {
    int ret ;
    //将img_file指向的.bin文件后缀改成elf
    strcpy(elf_file_name, img_file);
    strcpy(elf_file_name+strlen(elf_file_name) - 3, "elf");    printf("%s\n",elf_file_name);

    FILE *fp = fopen(elf_file_name, "rb");
    Assert(fp, "Can not open '%s'", elf_file_name);

    Elf32_Ehdr elf_head;   //最开始的文件头
    Elf32_Shdr* shdr = malloc(sizeof(Elf32_Shdr) * elf_head.e_shnum);
    ret = fread(&elf_head, sizeof(Elf32_Ehdr), 1, fp);
    //移动到Section Header table
    fseek(fp, elf_head.e_shoff, SEEK_SET);
    ret = fread(shdr, sizeof(Elf32_Shdr) * elf_head.e_shnum, 1, fp);
    assert(ret);


    // 重置指针位置到文件流开头
	rewind(fp);
    // 将fp指针移到 字符串表偏移位置处
	fseek(fp, shdr[elf_head.e_shstrndx].sh_offset, SEEK_SET);
    char shstrtab[shdr[elf_head.e_shstrndx].sh_size];
    ret = fread(shstrtab, shdr[elf_head.e_shstrndx].sh_size, 1, fp);


    for(int i = 0; i < elf_head.e_shnum; i++) {
        if(!strcmp(shstrtab+shdr[i].sh_name, ".symtab")) {      //printf("111\n777\n");
            rewind(fp);
            fseek(fp, shdr[i].sh_offset, SEEK_SET);
            ret = fread(sym_table, shdr[i].sh_size, 1, fp);
            sym_num = shdr[i].sh_size / sizeof(Elf32_Sym);
        }
        else if(!strcmp(shstrtab+shdr[i].sh_name, ".strtab")) {  //printf("222\n888\n");
            rewind(fp);
            fseek(fp, shdr[i].sh_offset, SEEK_SET);
            ret = fread(str_table, shdr[i].sh_size, 1, fp);
        }
    }

    sys_table_display();

    free(shdr);
    fclose(fp);
}

void print_space() {
    for(int i = 0; i < space_num; ++i) printf("  ");
}


void ftrace_display(vaddr_t ad,int flag) {
    char func_name[32] = "???";
    vaddr_t ad_start = 0;
    for(int i = 0; i < sym_num; ++i) {
        if(((sym_table[i].st_info) & 0xff) == STT_FUNC && ad >= sym_table[i].st_value && ad < sym_table[i].st_value + sym_table[i].st_size) {
            strcpy(func_name, str_table+sym_table[i].st_name);
            ad_start = sym_table[i].st_value;
        }
    }

    if(flag == CALL_TYPE) {
        printf("0x%x: ",cpu.pc);
        print_space();
        printf("call [%s@0x%x]\n", func_name, ad_start);         //  printf("0x%x\n",ad);
        ++space_num;
    }
    else if(flag == RET_TYPE) {
        printf("0x%x: ",cpu.pc);
        --space_num;
        print_space();
        printf("ret  [%s]\n", func_name);
    }
}


#endif


static long load_img() {
  if (img_file == NULL) {
    Log("No image is given. Use the default build-in image.");
    return 4096; // built-in image size
  }

  FILE *fp = fopen(img_file, "rb");
  Assert(fp, "Can not open '%s'", img_file);

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);

  Log("The image is %s, size = %ld", img_file, size);

  fseek(fp, 0, SEEK_SET);
  int ret = fread(guest_to_host(RESET_VECTOR), size, 1, fp);
  assert(ret == 1);

  fclose(fp);
  return size;
}

static int parse_args(int argc, char *argv[]) {
  const struct option table[] = {
    {"batch"    , no_argument      , NULL, 'b'},
    {"log"      , required_argument, NULL, 'l'},
    {"diff"     , required_argument, NULL, 'd'},
    {"port"     , required_argument, NULL, 'p'},
    {"help"     , no_argument      , NULL, 'h'},
    {0          , 0                , NULL,  0 },
  };
  int o;
  while ( (o = getopt_long(argc, argv, "-bhl:d:p:", table, NULL)) != -1) {
    switch (o) {
      case 'b': sdb_set_batch_mode(); break;
      case 'p': sscanf(optarg, "%d", &difftest_port); break;
      case 'l': log_file = optarg; break;
      case 'd': diff_so_file = optarg; break;
      case 1: img_file = optarg; return optind - 1;
      default:
        printf("Usage: %s [OPTION...] IMAGE [args]\n\n", argv[0]);
        printf("\t-b,--batch              run with batch mode\n");
        printf("\t-l,--log=FILE           output log to FILE\n");
        printf("\t-d,--diff=REF_SO        run DiffTest with reference REF_SO\n");
        printf("\t-p,--port=PORT          run DiffTest with port PORT\n");
        printf("\n");
        exit(0);
    }
  }
  return 0;
}

void init_monitor(int argc, char *argv[]) {
  /* Perform some global initialization. */

  /* Parse arguments. */
  parse_args(argc, argv);

  /* Set random seed. */
  init_rand();

  /* Open the log file. */
  init_log(log_file);

  /* Initialize memory. */
  init_mem();

  /* Initialize devices. */
  IFDEF(CONFIG_DEVICE, init_device());

  /* Perform ISA dependent initialization. */
  init_isa();

  /* Load the image to memory. This will overwrite the built-in image. */
  long img_size = load_img();

#ifdef CONFIG_FTRACE
  /*ftrace（后加的）*/
  init_ftrace();
#endif
  /* Initialize differential testing. */
  init_difftest(diff_so_file, img_size, difftest_port);

  /* Initialize the simple debugger. */
  init_sdb();

  IFDEF(CONFIG_ITRACE, init_disasm(
    MUXDEF(CONFIG_ISA_x86,     "i686",
    MUXDEF(CONFIG_ISA_mips32,  "mipsel",
    MUXDEF(CONFIG_ISA_riscv32, "riscv32",
    MUXDEF(CONFIG_ISA_riscv64, "riscv64", "bad")))) "-pc-linux-gnu"
  ));

  /* Display welcome message. */
  welcome();
}
#else // CONFIG_TARGET_AM
static long load_img() {
  extern char bin_start, bin_end;
  size_t size = &bin_end - &bin_start;
  Log("img size = %ld", size);
  memcpy(guest_to_host(RESET_VECTOR), &bin_start, size);
  return size;
}

void am_init_monitor() {
  init_rand();
  init_mem();
  init_isa();
  load_img();
  IFDEF(CONFIG_DEVICE, init_device());
  welcome();
}
#endif
