#include "phyAlloc.h"
#include "hw.h"

#ifndef SCHEDULER_H
#define SCHEDULER_H

#define STACK_SIZE 512 // ~10 func in stack depth and ~50 bytes per function

#define WORD_SIZE 4
#define INIT_PRIORITY -10
#define SAVED_REGISTERS 13
#define CPSR_INIT 0x13

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

  unsigned int sp;
  int priorityValue;
  unsigned int pid;
  unsigned int waiting_time;
};

#endif
