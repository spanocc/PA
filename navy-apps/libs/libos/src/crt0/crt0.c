#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;

void call_main(uintptr_t *args) {   //printf("%p\n",args);
  uint8_t *p = (uint8_t *)args;       
  int argc = *(int *)p;              
  p += sizeof(int);

  char **argv = (char **)p;      printf("call main:%s %s\n",argv[0],argv[1]);
  p += (argc + 1) * sizeof(char **);

  char **envp = (char **)p;   
                                 assert(0);
  // char *empty[] =  {NULL };
  // environ = empty;
  environ = envp;
  exit(main(argc, argv, envp));   
  assert(0);
}
