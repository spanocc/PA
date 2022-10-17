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

static uintptr_t loader(PCB *pcb, const char *filename) {
// TODO();
// uintptr_t prog_addr = 0x83000000;

  int ret ;
  Elf_Ehdr elf_head;
  ret = ramdisk_read(&elf_head, 0, sizeof(Elf_Ehdr));
  assert(ret > 0);
  //printf("0x%x\n", *(elf_head.e_ident));
  assert(*(elf_head.e_ident) == 0x7f);
  assert(elf_head.e_machine == EXPECT_TYPE);

// Elf_Phdr *phar = malloc(sizeof(Elf_Phdr) * elf_head.e_phnum);
  Elf_Phdr *phar = (Elf_Phdr *)(&ramdisk_start + elf_head.e_phoff);    printf("\n\n%p\n\n", phar);
  for(int i = 0; i < elf_head.e_phnum; ++i) {  //printf("%d",i);
    if(phar[i].p_type == PT_LOAD) {      //printf("%x\n", phar[i].p_vaddr);
      const char *buf = (char *)(&ramdisk_start + phar[i].p_offset);
      //size_t offest = (uint8_t *)phar[i].p_vaddr - &ramdisk_start;
      //ramdisk_write(buf, offest, phar[i].p_filesz);
      memcpy((uint8_t *)phar[i].p_vaddr, buf, phar[i].p_filesz);
      memset((char *)(phar[i].p_vaddr + phar[i].p_filesz), 0, phar[i].p_memsz - phar[i].p_filesz);
    }
  }
  return elf_head.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) (); //调用entry这个地址的函数，相当于跳转到这里
}

