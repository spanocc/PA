#include <common.h>

void init_monitor(int, char *[]);
void am_init_monitor();
void engine_start();
int is_exit_status_bad();


word_t expr(char *e, bool *success);
int main(int argc, char *argv[]) { 
  /* Initialize the monitor. */
#ifdef CONFIG_TARGET_AM
  am_init_monitor();
#else
  init_monitor(argc, argv);
#endif

  /* Start engine. */
  //engine_start();

  //return is_exit_status_bad();  
 
  int bufsize = 65536;
printf("000");  FILE *fp = fopen("tools/gen-expr/input", "r"); printf("111");
   char array[bufsize];
   int cnt = 1;
   while(fgets(array,bufsize,fp)){
       array[strlen(array)-1] = '\0';
       char* result = strtok(array, " ");
       char* arg = result + strlen(result) +1;
       bool success;
       uint32_t ret = expr(arg,&success);
       if(ret == (unsigned)atoi(result)) {
           printf("successfully! %d\n",cnt++);
       }
       else {
           printf("false: %d %d\n",ret,(unsigned)atoi(result));
           break;
       }
   }

}


