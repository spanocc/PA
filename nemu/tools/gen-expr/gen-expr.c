#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

static int bufsize = 65536;

static int choose(int n) {
    int ret = rand()%n;
    return ret;
}

static void gen_num(int* fault, int flag) {
    int ret = rand();
    if(ret == 0 && flag ==0) ret = 1;   
    char tmp[65536];
    sprintf(tmp, "%du", ret);
    if(strlen(tmp)+strlen(buf) >= bufsize) {
        *fault = 1;
        return;
    }
    strncat(buf, tmp, bufsize-strlen(buf));
    return ;
}

static void gen_rand_op(int* fault) {
    int ret = rand()%4;
    char tmp[2];
    switch(ret) {
        case 1: tmp[0] = '+'; break;
        case 2: tmp[0] = '-'; break;
        case 3: tmp[0] = '*'; break;
        default:tmp[0] = '/'; break;
    }
    tmp[1] = '\0';
    if(strlen(tmp)+strlen(buf) >= bufsize) {
        *fault = 1;
        return;
    }
    strncat(buf, tmp, bufsize-strlen(buf));
}

static void gen(char c, int* fault) {
    char tmp[2];
    tmp[0] = c; tmp[1] = '\0';
     if(strlen(tmp)+strlen(buf) >= bufsize) {
        *fault = 1;
        return;
    }
    strncat(buf, tmp, bufsize-strlen(buf));
}


static void gen_rand_expr(int* fault) { //防止除0（我的笨方法）：如果是除号  那么被除数的表达式只能是一个数且这个数不为0
  if(*fault == 1) return;
  switch (choose(5)) {
    case 0: gen_num(fault, 1); break;
    case 1: gen('(',fault); gen_rand_expr(fault); gen(')', fault); break;
    case 2: gen(' ',fault); gen_rand_expr(fault); break;
    case 3: gen_rand_expr(fault); gen(' ',fault); break;
    default: 
        gen_rand_expr(fault); 
        gen_rand_op(fault); 
        if(buf[strlen(buf)-1] == '/') gen_num(fault, 0); 
        else gen_rand_expr(fault);
        break;
  }
}




int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;

  for (i = 0; i < loop; i ++) {
    int fault = 0;
    do{   //如果发生错误(eg：超过buf容量)就重新再求一个表达式 如果buf空间不够就给fault赋值为true
        buf[0] = '\0';
        gen_rand_expr(&fault);
    } while(fault);

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
   // printf("%d:\n",i);
  }

  return 0;
}
