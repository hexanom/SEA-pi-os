#include "sched.h"
#include "kalloc.h"
#include "hw.h"

#define WORD_SIZE 4
#define SAVED_REGISTERS 13
#define CPSR_INIT 0x13

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

  uint64 sp;
};

struct pcb_s* first_pcb = NULL;
struct pcb_s* last_pcb = NULL;
struct pcb_s* current_process = NULL;

void start_current_process() {
  current_process->state = RUNNING;
  current_process->entry_point(current_process->args);
  DISABLE_IRQ();
  // set as done
  current_process->state = DONE;
  // close loop
  struct pcb_s* next_pcb = current_process->next_pcb;
  struct pcb_s* walk_pcb = next_pcb;
  while(walk_pcb->next_pcb != current_process) {
    walk_pcb = walk_pcb->next_pcb;
  }
  walk_pcb->next_pcb = next_pcb;
  // dealloc
  current_process->sp += (1 + 1 + SAVED_REGISTERS) * WORD_SIZE;
  current_process->sp -= STACK_SIZE;
  kalloc_free((void*) current_process->sp, STACK_SIZE);
  kalloc_free(current_process, sizeof(struct pcb_s));

  hw_set_tick_and_enable_timer();
  ENABLE_IRQ();
}

bool init_pcb(struct pcb_s* pcb, func_t entry_point, void* args, uint32 stack_size) {
  pcb->sp = ((unsigned int)kalloc_alloc(stack_size));
  if(pcb->sp == 0) {
    return false;
  }
  pcb->sp += STACK_SIZE;
  pcb->sp -= WORD_SIZE;
  *((int*) pcb->sp) = CPSR_INIT; // cpsr
  pcb->sp -= WORD_SIZE;
  *((int*) pcb->sp) = (unsigned int)&start_current_process; // lr
  pcb->sp -= SAVED_REGISTERS * WORD_SIZE;

  pcb->entry_point = entry_point;
  pcb->args = args;
  pcb->state = NEW;

  return true;
}

void add_pcb(struct pcb_s* pcb) {
  if(last_pcb != NULL) {
    last_pcb->next_pcb = pcb;
  }
  else {
    first_pcb = pcb;
  }
  pcb->next_pcb = first_pcb;
  last_pcb = pcb;
}

void elect() {
  do {
    current_process = current_process->next_pcb;
  } while(current_process->state != READY && current_process->state != NEW);
}

bool sched_start() {
  struct pcb_s* kmain_pcb = kalloc_alloc(sizeof(struct pcb_s));
  init_pcb(kmain_pcb, NULL, NULL, STACK_SIZE);
  kmain_pcb->next_pcb = first_pcb;
  current_process = kmain_pcb;
  hw_set_tick_and_enable_timer();
  ENABLE_IRQ();
  return true;
}

void ctx_switch_from_irq() {
  DISABLE_IRQ();

  __asm("sub lr, lr, #4");
  __asm("srsdb sp!, #0x13");
  __asm("cps #0x13");

  __asm("push {r0-r12}");
  __asm("mov %0, sp" : "=r"(current_process->sp));
  current_process->state = READY;
  
  elect();
  
  current_process->state = RUNNING;
  __asm("mov sp, %0" : : "r"(current_process->sp));
  hw_set_tick_and_enable_timer();


  __asm("pop {r0-r12}");
  ENABLE_IRQ();

  __asm("rfeia sp!"); // we're writing back into the Rn registers so we use '!'
}

bool sched_new_proc(func_t f, void *args, unsigned int stack_size) {
  struct pcb_s* pcb = kalloc_alloc(sizeof(struct pcb_s));

  if(pcb == 0 || !init_pcb(pcb, f, args, stack_size)) {
    return false;
  }
  add_pcb(pcb);
  return true;
}
