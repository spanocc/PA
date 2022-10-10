#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  kbd->keydown = 0;
  kbd->keycode = AM_KEY_NONE;
  uint32_t key = *(uint32_t *)KBD_ADDR;
  if(key & KEYDOWN_MASK) {
    kbd->keydown = true;
    key ^= KEYDOWN_MASK;
  }
  kbd->keycode = key;
}
