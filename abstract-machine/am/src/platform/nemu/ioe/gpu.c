#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
  int i;
  int w = 32;  // TODO: get the correct width
  int h = 32;  // TODO: get the correct height
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < w * h; i ++) fb[i] = i;
  outl(SYNC_ADDR, 1);  //马上将帧缓冲中的内容同步到屏幕上
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  int w = ((*(uint32_t *)VGACTL_ADDR) >> 16);
  int h = ((*(uint32_t *)VGACTL_ADDR) & 0xffff);
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = w, .height = h,
    .vmemsz = (w * h) * sizeof(uint32_t),
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
/*    
  int w = ctl->w;
  int h = ctl->h;
*/
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
