#ifndef SYSCALL_H
#define SYSCALL_H

#include "sched.h"

extern struct pcb_s* current_process;
extern void PUT32 ( unsigned int, unsigned int );

static inline __attribute__((always_inline)) void doSysCallReboot() {
  const int PM_RSTC = 0x2010001c;
  const int PM_WDOG = 0x20100024;
  const int PM_PASSWORD = 0x5a000000;
  const int PM_RSTC_WRCFG_FULL_RESET = 0x00000020;
  PUT32(PM_WDOG, PM_PASSWORD | 1);
  PUT32(PM_RSTC, PM_PASSWORD | PM_RSTC_WRCFG_FULL_RESET);
  while(1);
}

static inline __attribute__((always_inline)) void doSysCallWait(unsigned int quantums) {
  current_process->sleepuntil = quantums;
  __asm("mov pc, %0" : : "r"(ctx_switch_from_irq));
}

void sys_reboot();
void sys_wait(unsigned int quantums);
void SWIHandler(unsigned int code, unsigned int * params);

#endif
