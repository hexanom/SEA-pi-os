#ifndef SYSCALL_H
#define SYSCALL_H

#include "sched.h"

// NO API

// Externs
extern struct sched_pcb_s* current_process;
extern void PUT32 ( unsigned int, unsigned int );

// Does an actual reboot (watchdog reset), inline: no stack
static inline __attribute__((always_inline)) void syscall_do_reboot() {
  const int PM_RSTC = 0x2010001c;
  const int PM_WDOG = 0x20100024;
  const int PM_PASSWORD = 0x5a000000;
  const int PM_RSTC_WRCFG_FULL_RESET = 0x00000020;
  PUT32(PM_WDOG, PM_PASSWORD | 1);
  PUT32(PM_RSTC, PM_PASSWORD | PM_RSTC_WRCFG_FULL_RESET);
  while(1);
}

// Ask for a wait syscall
static inline __attribute__((always_inline)) void syscall_do_wait(unsigned int quantums) {
  current_process->sleepuntil = quantums;
  __asm("mov pc, %0" : : "r"(sched_ctx_switch_from_irq));
}

// END NO API

/**
 * Asks for a system reboot
 */
void syscall_reboot();

/**
 * Asks to put the current process on wait
 *
 * @param The amount of quantums to wait
 */
void syscall_wait(unsigned int quantums);

/*
 * Handles a syscall
 *
 * @param The syscall's code
 * @param The parameters
 */
void syscall_swi_handler(unsigned int code, unsigned int * params);

#endif
