#include <NDL.h>
#include <SDL.h>
#include <string.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

#define KEY_NUM (int)(sizeof(keyname) / sizeof(keyname[0]))

int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
  char buf[32];
  while(!(NDL_PollEvent(buf, sizeof(buf))));   printf("%s\n",buf);
  event->type = (buf[1] == 'd') ? SDL_KEYDOWN : SDL_KEYUP; 
  for(int i = 0; i < KEY_NUM; ++i) {
    if(!strcmp(buf+3, keyname[i])) {          printf("%d\n",i);
      event->key.keysym.sym = i;
      break;
    }
  }
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return NULL;
}
