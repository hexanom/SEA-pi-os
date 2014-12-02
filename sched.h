#ifndef SCHED_H
#define SCHED_H

#define STACK_SIZE 512 // ~10 func in stack depth and ~50 bytes per function
#define NULL 0

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
};

void init_pcb(struct pcb_s* pcb, func_t entry_point, void* args);
void add_pcb(struct pcb_s* pcb);

int create_process(func_t f, void *args, unsigned int stack_size);

void start_sched();
void start_current_process();
void elect();
void ctx_switch_from_irq();

#endif
