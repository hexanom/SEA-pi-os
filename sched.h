#ifndef SCHED_H
#define SCHED_H

#include "types.h"

#define STACK_SIZE 512 // ~10 func in stack depth and ~50 bytes per function

typedef void (*func_t) (void*);

enum process_state {
  NEW,
  READY,
  RUNNING,
  BLOCKED,
  DONE
};

struct pcb_s {
  enum process_state state;

  struct pcb_s* next_pcb;

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
 * @param (UNUSED?) stack's size
 * @param the success of the operation
 */
bool sched_new_proc(func_t f, void *args, unsigned int stack_size);

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
