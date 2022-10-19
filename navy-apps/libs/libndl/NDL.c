#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//后加的
#include <sys/time.h> //timeval
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<assert.h>


static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;


uint32_t NDL_GetTicks() {
  struct timeval t;
  gettimeofday(&t, NULL);
  uint32_t ret = t.tv_sec * 1000 + t.tv_usec / 1000; //不用*0.001 浮点数没实现   
  return ret;
}

int NDL_PollEvent(char *buf, int len) {
  int fd = open("/dev/events", 0, 0);
  int ret = read(fd, buf, len);
  close(fd);
  return ret ? 1 : 0;
}

void NDL_OpenCanvas(int *w, int *h) {
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }

  char dispinfo[64];
  int ret = read(4, dispinfo, sizeof(dispinfo)-1);
  int sw, sh, flag = 0;
  char *c = dispinfo;
  while(*c != '\0') {
    if(*c <= '9' && *c >= '0') {
      if(flag == 0) {
        sscanf(c, "%d", &sw);
        flag = 1;
      }
      else sscanf(c, "%d", &sh);
    }
    c++;
  }
  
  printf("sw:%d  sh:%d\n",sw ,sh);

  if(*w == 0 && *h == 0) {
    *w = sw;
    *h = sh;
  }
  assert(*w <= sw && *h <= sh);

}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  return 0;
}

void NDL_Quit() {
}
