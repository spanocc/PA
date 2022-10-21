#include <am.h>
/*
#define KEYDOWN_MASK 0x8000

int fd_table[128];
*/
bool ioe_init() {
 /* int fd;
  fd = open();
*/


  return true;
}

void ioe_read (int reg, void *buf) {
/*
  struct timeval tv;
  AM_TIMER_CONFIG_T * cfg;
  AM_TIMER_RTC_T * rtc;
  AM_TIMER_UPTIME_T *uptime;
  char tmp[32];

  swhitch(reg) {
    case AM_TIMER_CONFIG:
      cfg = (AM_TIMER_CONFIG_T *)buf;
      cfg->present = true; cfg->has_rtc = true;
      break;

    case AM_TIMER_RTC:
      rtc = (AM_TIMER_RTC_T *)buf;
      rtc->second = 0;
      rtc->minute = 0;
      rtc->hour   = 0;
      rtc->day    = 0;
      rtc->month  = 0;
      rtc->year   = 1900;
      break;

    case AM_TIMER_UPTIME:
      gettimeofday(&tv, NULL);
      uptime = AM_TIMER_UPTIME_T *)buf;
      uptime->us = tv.sec * 1000000 + tv.usec;
      break;

    case AM_INPUT_CONFIG:
      (AM_INPUT_CONFIG_T *)buf->present = true;
      break;
    
    case AM_INPUT_KEYBRD:
      read(fd_table[AM_INPUT_KEYBRD], tmp, sizeof(tmp));

  }
*/
}
void ioe_write(int reg, void *buf) { 

}
