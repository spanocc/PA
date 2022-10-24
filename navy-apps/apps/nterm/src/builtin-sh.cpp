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

static void sh_handle_cmd(const char *cmd) {
  char *argv[10] = { };
 // char *envp[10] = {"ARCH=riscv32-nemu", "HOME=llh", NULL};
  char *buf = (char *)malloc(strlen(cmd));
  char *file_name = (char *)malloc(strlen(cmd));
  argv[0] = (char *)malloc(strlen(cmd));
  argv[1] = NULL;
  strcpy(buf, cmd);
  buf[strlen(buf) - 1] = '\0';  printf("%s\n",buf);
  
  sscanf(buf, "%s %s", file_name, argv[0]);  printf("%s %s\n",file_name,argv[0]);


  execve(file_name, argv, NULL);
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
