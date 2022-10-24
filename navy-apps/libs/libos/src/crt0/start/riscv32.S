.globl  _start
_start:
  move s0, zero

  //move sp, a0 //把GPRx（存着栈顶指针）赋值给sp   
  // move a0, a0  a0作为call_main的参数
  jal call_main
