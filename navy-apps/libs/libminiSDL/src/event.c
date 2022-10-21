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
  char buf[32]; 
  int ret = NDL_PollEvent(buf, sizeof(buf));    printf("%s\n",buf);
  //while(!(NDL_PollEvent(buf, sizeof(buf))));   //printf("%s\n",buf);
  if(!ret) return 0;
  ev->type = (buf[1] == 'd') ? SDL_KEYDOWN : SDL_KEYUP; 
  for(int i = 0; i < KEY_NUM; ++i) {  //printf("%s\n",keyname[i]);
    if(!strncmp(buf+3, keyname[i], strlen(buf+3)-1)) {  //buf的最后有个换行符，不能比较    
      ev->key.keysym.sym = i;
      break;
    }
  } 
  return 1;
}

int SDL_WaitEvent(SDL_Event *event) {
  char buf[32];
  while(!(NDL_PollEvent(buf, sizeof(buf))));   //printf("%s\n",buf);
  event->type = (buf[1] == 'd') ? SDL_KEYDOWN : SDL_KEYUP; 
  for(int i = 0; i < KEY_NUM; ++i) {  //printf("%s\n",keyname[i]);
    if(!strncmp(buf+3, keyname[i], strlen(buf+3)-1)) {  //buf的最后有个换行符，不能比较    
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
