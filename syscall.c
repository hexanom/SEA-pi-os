#include "syscall.h"
#define SYS_REBOOT 1
#define SYS_WAIT 2

// Import current pcb
extern struct sched_pcb_s* current_process;
extern void PUT32 ( unsigned int, unsigned int );

// Ask for a reboot syscall
void syscall_reboot() {
  __asm("mov r0, %0" : : "r"(SYS_REBOOT) : "r0");
  __asm("SWI 0" : : : "lr");
}

// Ask for a wait syscall
void syscall_wait(volatile unsigned int quantums) {
  __asm("mov r0, %0" : : "r"(SYS_WAIT) : "r0");
  __asm("mov r1, sp");
  __asm("SWI 0" : : : "lr");
}

// SWI Interrupts handler
void syscall_swi_handler(unsigned int code, unsigned int * params) {
  __asm("mov lr, %0" : : "r"(params[3] + 0x4) : "lr");
  switch (code) {
    case 1:
      syscall_do_reboot();
    break;
    case 2:
      syscall_do_wait(params[1]);
    break;
  }
}
