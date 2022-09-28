#include "sdb.h"

#define NR_WP 32
/*结构体定义 我放在了sdb.h文件里
typedef struct watchpoint {
  int NO;
  char WatchName[64];
  uint32_t value;
 
  struct watchpoint *next;

  // TODO: Add more members if necessary 

} WP;
*/
static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
//插入都在链表末尾
WP* new_wp() {
    assert(free_ != NULL);
    WP* pwp = free_;
    free_ = free_->next;
    pwp->next = NULL;

    if(head == NULL) head = pwp;
    else {
        WP* p = head;
        while(p->next != NULL) {
            p = p->next;
        }
        p->next = pwp;
    }        
    return pwp;
}

void free_wp(WP *wp) {
    wp->next = NULL;
   if(free_ == NULL) free_ = wp; 
   else {
       WP* p = free_;
       while(p->next != NULL) p = p->next;
       p->next = wp;
   }
}

void delete_wp(int num) {
    WP* plast = NULL;
    WP* pwp = head;
    while(pwp != NULL) {
        if(pwp->NO == num) {
            if(pwp == head) head = head->next;
            else plast->next = pwp->next;
            free_wp(pwp);
            return;
        }
        plast = pwp;
        pwp = pwp->next;
    }
    printf("can't find watchpoint %d\n",num);
    return;
}

void display_watchpoint() {
    if(head == NULL) {
        printf("There is no watchpoint!\n");
        return;
    }

    printf("%-20s%-20s%s\n", "Num", "Value", "What" );
    WP* pwp = head;
    while(pwp != NULL) {
        printf("%-20d%-20d%s", pwp->NO, pwp->value, pwp->WatchName);
        pwp = pwp->next;
    }
}

