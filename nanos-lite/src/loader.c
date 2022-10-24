#include <proc.h>
#include <elf.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif


#if defined(__ISA_AM_NATIVE__)
# define EXPECT_TYPE EM_X86_64
#elif defined(__ISA_RISCV32__)
# define EXPECT_TYPE EM_RISCV  // see /usr/include/elf.h to get the right type

#else
# error Unsupported ISA
#endif

extern uint8_t ramdisk_start;
extern uint8_t ramdisk_end;
size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);
void* new_page(size_t nr_page);



static uintptr_t loader(PCB *pcb, const char *filename) {
// TODO();
// uintptr_t prog_addr = 0x83000000;

  int fd;
  fd = fs_open(filename, 0, 0);

  int ret ;
  Elf_Ehdr elf_head;
  ret = fs_read(fd, &elf_head, sizeof(Elf_Ehdr));
  assert(ret > 0);
  //printf("0x%x\n", *(elf_head.e_ident));
  assert(*(elf_head.e_ident) == 0x7f);
  assert(elf_head.e_machine == EXPECT_TYPE);

// Elf_Phdr *phar = malloc(sizeof(Elf_Phdr) * elf_head.e_phnum);
// Elf_Phdr *phar = (Elf_Phdr *)(&ramdisk_start + elf_head.e_phoff);   // printf("\n\n%p\n\n", phar);
// ramdisk_start的地址不是4字节对齐的，所以可能出问题  
  Elf_Phdr phar;
  fs_lseek(fd, elf_head.e_phoff, 0);
  for(int i = 0; i < elf_head.e_phnum; ++i) {  //printf("%d",i);
    fs_lseek(fd, elf_head.e_phoff + i * sizeof(phar), 0);
    fs_read(fd, &phar, sizeof(phar));
    //ramdisk_read(&phar, elf_head.e_phoff + i * sizeof(phar), sizeof(phar)); //必须借助memcpy函数把phdr结构体读进来，直接用指针访问的话会有地址不是4字节对齐的问题，所以只要是访问内存都用memcpy函数就行了，而创建的这个phdr变量肯定是对齐的
    //if(phar[i].p_type == PT_LOAD) {      这样子访问p_type会产生lw操作，且地址不是4字节对齐，会有difftest错误                       //printf("%x\n", phar[i].p_vaddr);
    if(phar.p_type == PT_LOAD) {           printf("%x\n", phar.p_vaddr);
      //const char *buf = (char *)(&ramdisk_start + phar.p_offset);
      //size_t offest = (uint8_t *)phar[i].p_vaddr - &ramdisk_start;
      //ramdisk_write(buf, offest, phar[i].p_filesz);
      //memcpy((uint8_t *)phar.p_vaddr, buf, phar.p_filesz);

      fs_lseek(fd, phar.p_offset, 0);
      fs_read(fd, (uint8_t *)phar.p_vaddr, phar.p_filesz);
      memset((char *)(phar.p_vaddr + phar.p_filesz), 0, phar.p_memsz - phar.p_filesz);
    }
  }

  fs_close(fd);
  return elf_head.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) (); //调用entry这个地址的函数，相当于跳转到这里
}

void context_kload(PCB *new_pcb, void (*entry)(void *), void *arg) {
  Area kstack;
  kstack.start = new_pcb->stack;
  kstack.end = new_pcb->stack + STACK_SIZE;
  new_pcb->cp = kcontext(kstack, entry, arg);
}

void context_uload(PCB *new_pcb, const char *file_name, char *const argv[], char *const envp[]) {
                                                  if(argv && argv[0] && argv[1])  printf("LOAD1:%s %s\n",argv[0],argv[1]);
  printf("uload: %s\n",file_name);

  Area kstack;
  kstack.start = new_pcb->stack;
  kstack.end = new_pcb->stack + STACK_SIZE;
  
  uintptr_t entry = loader(new_pcb, file_name);        printf("entry:%p\n",entry);

  new_pcb->cp = ucontext(NULL, kstack, (void *)entry);
  //new_pcb->cp->gpr[10] = (uintptr_t)heap.end;
  uint8_t* stack_end = new_page(8);                                   
  stack_end += (4 * 8 * 1024);  //new_page返回低地址，栈要用高地址
  uint8_t* pstack = stack_end;  //栈从stack_end向下延伸

   printf("stack:%p\n",stack_end);

  int argc = 0, envpc = 0;
  int argv_size = 0, envp_size = 0;
  char **av = (char **)argv, **ep = (char **)envp;
  while(av !=NULL && *av != NULL) {
    argc++;
    argv_size += (strlen(*av) + 1); //空字符也算长度
    av++;
  }
  while(ep != NULL && *ep != NULL) {
    envpc++;
    envp_size += (strlen(*ep) + 1); //空字符也算长度
    ep++;
  }                                                          //NULL
  pstack -= (((argv_size + envp_size + sizeof(int) + (argc + 1 + envpc + 1) * sizeof(char *)) / 8 + 1) * 8);  //开辟8的倍数的空间
  uint8_t *str_tab = stack_end - (argv_size + envp_size);   //printf("0:%p\n",str_tab);
  uint8_t *p = pstack;

  *(int *)p = argc;         printf("1:%d\n",argc);
  p += sizeof(int);

  av = (char **)argv, ep = (char **)envp;  // printf("1:%p\n",p);
  while(av != NULL && *av != NULL) {
    strcpy((char *)str_tab, *av);
    *(char **)p = (char *)str_tab;    //printf("S:%s  %s\n", *av, *(char **)p);
    av++;
    str_tab += (strlen((char *)str_tab) +1);
    p += sizeof(char *);
  }

  *(char **)p = NULL;
  p += sizeof(char *);

  while(ep != NULL && *ep != NULL) {
    strcpy((char *)str_tab, *ep);
    *(char **)p = (char *)str_tab;
    ep++;
    str_tab += (strlen((char *)str_tab) + 1);
    p += sizeof(char *);
  }

  *(char **)p = NULL;
  p += sizeof(char *);
                                               uint8_t *pg = (uint8_t *)pstack; 
                                               int c = *(int *)pg;    pg += sizeof(int);
                                               printf("LOAD:%d %s %s\n",c,(char *)pg, (char *)(pg+8));
                                                
  new_pcb->cp->gpr[10] = (uintptr_t)pstack;

}
