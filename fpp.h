#ifndef FPP_H
#define FPP_H

#include "types.h"

#define WAITING_LIMIT 10
#define INIT_PRIORITY -10
#define INIT_BURST_TIME -10

// Defines the size in Bytes of a program's stack
// ~10 func in stack depth and ~50 bytes per function
#define STACK_SIZE 512

/**
 * Enumerates a PCB's possible state
 */
enum fpp_process_state {
  NEW,
  READY,
  RUNNING,
  BLOCKED,
  DONE
};

/**
 * Describes a process's PCB
 */
struct fpp_pcb_s {
  enum fpp_process_state state;

  struct fpp_pcb_s* next_pcb;

  func_t entry_point;
  void* args;

  uint64 sleepuntil;
  uint64 sp;
  
  int priority_value;
  unsigned int pid;
  unsigned int waiting_time;
};

/**
 * Create a new process from a C function
 *
 * @param C function pointer
 * @param C args
 * @param (UNUSED?) stack's size
 * @param the success of the operation
 */

bool fpp_new_proc(func_t f, void *args, unsigned int stack_size,
 int priority);

/**
 * Start the fpp_scheduler
 */
bool fpp_start();

/**
 * Performs a context switch from an IRQ
 *
 * NOTE: exposed for the syscalls: should not be used directly
 */
void fpp_ctx_switch_from_irq();

#endif
