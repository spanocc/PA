.globl  _start
_start:
  move s0, zero

  move sp, a0 //把GPRx（存着栈顶指针）赋值给sp

  jal call_main
