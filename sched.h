#ifndef SCHED_H
#define SCHED_H

#include "types.h"

// Defines the size in Pages of a program's stack
#define STACK_PAGES 1
#define STACK_SIZE STACK_PAGES * 4096

/**
 * Enumerates a PCB's possible state
 */
enum sched_process_state {
  NEW,
  READY,
  RUNNING,
  BLOCKED,
  DONE
};

/**
 * Describes a process's PCB
 */
struct sched_pcb_s {
  enum sched_process_state state;

  struct sched_pcb_s* next_pcb;

  func_t entry_point;
  void* args;

  uint64 sleepuntil;
  uint64 sp;
};

/**
 * Create a new process from a C function
 *
 * @param C function pointer
 * @param C args
 * @param stack's size in pages
 * @param the success of the operation
 */
bool sched_new_proc(func_t f, void *args, unsigned int stack_pages);

/**
 * Start the scheduler
 */
bool sched_start();

/**
 * Performs a context switch from an IRQ
 *
 * NOTE: exposed for the syscalls: should not be used directly
 */
void sched_ctx_switch_from_irq();

#endif
