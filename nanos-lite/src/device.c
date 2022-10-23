#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {  //printf("\n\n\n\nsds\n");
  yield();
  const char *c = buf;
  for(uintptr_t i = 0; i < len; ++i) { // printf("c");
      putch(*(c + i));
  }
  return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {  //printf("\n\n\nssssssssssssss\n\n\n");
  yield();
  size_t ret = 0;
  char event[32] = "ku ";
  AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
  if(ev.keycode == AM_KEY_NONE) return 0;          

  //printf("\n\n\nssssssssssssss\n\n\n");


  if(ev.keydown) event[1] = 'd';
  strcat(event, keyname[ev.keycode]);
  strcat(event, "\n");                                //printf("\n\n\n%s\n\n\n",event);
  while(event[ret] != '\0' && ret < len) {
    *(char *)(buf + ret) = event[ret];
    ++ret;
  } 
  if(ret < len) *(char *)(buf + ret) = '\0';
  else *(char *)(buf + len - 1) = '\0';
  return ret; 
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  int sw = io_read(AM_GPU_CONFIG).width;  
  int sh = io_read(AM_GPU_CONFIG).height; 
  char dispinfo[256];
  sprintf(dispinfo, "WIDTH : %d\nHEIGHT : %d\n", sw, sh);
  for(int i = 0; i < len; ++i) {
    *(char *)(buf + i) = dispinfo[i];
  }

  //printf("%s\n",dispinfo);

  return len;
}
//一行一行调用fb_write,len是在这一行的像素个数,一个像素是一个32位数
size_t fb_write(const void *buf, size_t offset, size_t len) {
  yield();
  int sw = io_read(AM_GPU_CONFIG).width;  
  //int sh = io_read(AM_GPU_CONFIG).height; 
  io_write(AM_GPU_FBDRAW, offset % sw, offset / sw, (uint32_t *)buf, len, 1, true); //每次都同步到屏幕

  return len;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
