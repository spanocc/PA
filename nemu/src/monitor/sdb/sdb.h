#ifndef __SDB_H__
#define __SDB_H__

#include <common.h>

word_t expr(char *e, bool *success);
typedef struct watchpoint {
  int NO;
  char WatchName[64];                                                                                                          
  uint32_t value;
  
  struct watchpoint *next;
 
  /* TODO: Add more members if necessary */
 
} WP;

#endif
