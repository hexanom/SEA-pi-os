#ifndef SCHED_H
#define SCHED_H
#define FPP

#include "types.h"

// Defines the size in Pages of a program's stack
#define STACK_PAGES 1
#define STACK_SIZE STACK_PAGES * 4096
#define WAITING_LIMIT 10
#define INIT_PRIORITY -10
#define INIT_BURST_TIME -10

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

  int priority_value;
  uint8 pid;
  uint32 waiting_time;

};

/**
 * Create a new process from a C function
 *
 * @param C function pointer
 * @param C args
 * @param stack's size in pages
 * @param the success of the operation
 */

bool sched_new_proc(func_t f, void *args, unsigned int stack_pages, int priority);

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
