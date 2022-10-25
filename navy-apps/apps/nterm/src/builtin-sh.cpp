#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>
#include <SDL.h>

char handle_key(SDL_Event *ev);

static void sh_printf(const char *format, ...) {
  static char buf[256] = {};
  va_list ap;
  va_start(ap, format);
  int len = vsnprintf(buf, 256, format, ap);
  va_end(ap);
  term->write(buf, len);
}

static void sh_banner() {
  sh_printf("Built-in Shell in NTerm (NJU Terminal)\n\n");
}

static void sh_prompt() {
  sh_printf("sh> ");
}
//malloc在堆上，数组在栈上，为了防止被加载的用户程序覆盖，要在栈上存参数
//字面值常量在常量区，也属于静态存储区
static void sh_handle_cmd(const char *cmd) {
  //char *argv[10] = {"echo", "acha", NULL };  //这样子初始化，也会导致argv[0]的指针在0x83000000+的内存上
  //char *envp[10] = {"ARCH=riscv32-nemu", "HOME=llh", NULL};
  char *buf = (char *)malloc(strlen(cmd));
  char tmp[10][128] = {"echo", "acha"}; //所以采用数组，这样子是在用户栈上开辟空间 argv[0]地址在0x80746a90
  char *argv[10];
  argv[0] = tmp[0];
  argv[1] = tmp[1];
  argv[2] = NULL;
  //char *file_name = (char *)malloc(strlen(cmd)); 
  /*argv[0] = (char *)malloc(strlen(cmd));        //如果用malloc创建数组空间，则argv[0]的指针在0x83000000+的地方，此时加载程序，会把这些参数指针覆盖掉
  argv[1] = (char *)malloc(strlen(cmd));
  argv[2] = NULL;*/
  strcpy(buf, cmd);
  buf[strlen(buf) - 1] = '\0';  printf("arg: %s\n",buf);  //printf("cmd: %p\n",cmd);
                                                           // wc /share/games/bird/atlas.txt
  
  sscanf(buf, "%s %s", argv[0], argv[1]);  printf("%s %s\n",argv[0],argv[1]);

 
  execve("/bin/busybox", argv, NULL);
}

void builtin_sh_run() {
  sh_banner();
  sh_prompt();

  while (1) {
    SDL_Event ev;
    if (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN) {
        const char *res = term->keypress(handle_key(&ev));
        if (res) {
          sh_handle_cmd(res);
          sh_prompt();
        }
      }
    }
    refresh_terminal();
  }
}
