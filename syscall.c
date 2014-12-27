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

// Does an actual reboot (watchdog reset)
void syscall_do_reboot() {
  const int PM_RSTC = 0x2010001c;
  const int PM_WDOG = 0x20100024;
  const int PM_PASSWORD = 0x5a000000;
  const int PM_RSTC_WRCFG_FULL_RESET = 0x00000020;
  PUT32(PM_WDOG, PM_PASSWORD | 1);
  PUT32(PM_RSTC, PM_PASSWORD | PM_RSTC_WRCFG_FULL_RESET);
  while(1);
}

// Ask for a wait syscall
void syscall_wait(volatile unsigned int quantums) {
  __asm("mov r0, %0" : : "r"(SYS_WAIT) : "r0");
  __asm("mov r1, sp");
  __asm("SWI 0" : : : "lr");
}

// Update the PCB to put it on wait
void syscall_do_wait(unsigned int quantums) {
  current_process->sleepuntil = quantums;
  __asm("mov pc, %0" : : "r"(sched_ctx_switch_from_irq));
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
