#include "sched.h"
#include "kalloc.h"
#include "hw.h"

#define WORD_SIZE 4
#define SAVED_REGISTERS 13
#define CPSR_INIT 0x13

struct sched_pcb_s* first_pcb = NULL;
struct sched_pcb_s* last_pcb = NULL;
struct sched_pcb_s* current_process = NULL;

// Starts the current process in PCB and remove it when done (execution wrapper)
void sched_start_current_process() {
  current_process->state = RUNNING;
  current_process->entry_point(current_process->args);
  DISABLE_IRQ();
  // set as done
  current_process->state = DONE;
  // close loop
  struct sched_pcb_s* next_pcb = current_process->next_pcb;
  struct sched_pcb_s* walk_pcb = next_pcb;
  while(walk_pcb->next_pcb != current_process) {
    walk_pcb = walk_pcb->next_pcb;
  }
  walk_pcb->next_pcb = next_pcb;
  // dealloc
  current_process->sp += (1 + 1 + SAVED_REGISTERS) * WORD_SIZE;
  current_process->sp -= STACK_SIZE;
  kalloc_free((void*) current_process->sp, STACK_SIZE);
  kalloc_free(current_process, sizeof(struct sched_pcb_s));

  hw_set_tick_and_enable_timer();
  ENABLE_IRQ();
}

// Initializes a pcb struct
bool sched_init_pcb(struct sched_pcb_s* pcb, func_t entry_point, void* args, uint32 stack_size) {
  pcb->sp = ((unsigned int)kalloc_alloc(stack_size));
  if(pcb->sp == 0) {
    return false;
  }
  pcb->sp += STACK_SIZE;
  pcb->sp -= WORD_SIZE;
  *((int*) pcb->sp) = CPSR_INIT; // cpsr
  pcb->sp -= WORD_SIZE;
  *((int*) pcb->sp) = (unsigned int)&sched_start_current_process; // lr
  pcb->sp -= SAVED_REGISTERS * WORD_SIZE;

  pcb->entry_point = entry_point;
  pcb->args = args;
  pcb->state = NEW;
  pcb->sleepuntil = 0;

  return true;
}

// Add a PCB to the LIFO
void sched_add_pcb(struct sched_pcb_s* pcb) {
  if(last_pcb != NULL) {
    last_pcb->next_pcb = pcb;
  }
  else {
    first_pcb = pcb;
  }
  pcb->next_pcb = first_pcb;
  last_pcb = pcb;
}

// Update a PCB's timer
void sched_update(struct sched_pcb_s* pcb) {
  if(pcb->sleepuntil > 0) {
    pcb->sleepuntil--;
  }
}

// Update ALL PCB's timers
void sched_update_timers() {
  struct sched_pcb_s* pcb = first_pcb;
  while(pcb != last_pcb) {
    sched_update(pcb);
    pcb = pcb->next_pcb;
  }
  sched_update(pcb);
}

// Elects the next process
void sched_elect() {
  sched_update_timers();
  do {
    current_process = current_process->next_pcb;
  } while(current_process->state != READY && current_process->state != NEW && current_process->sleepuntil == 0);
}

// Starts by initializing the kmain's PCB
bool sched_start() {
  struct sched_pcb_s* kmain_pcb = kalloc_alloc(sizeof(struct sched_pcb_s));
  sched_init_pcb(kmain_pcb, NULL, NULL, STACK_SIZE);
  kmain_pcb->next_pcb = first_pcb;
  current_process = kmain_pcb;
  hw_set_tick_and_enable_timer();
  ENABLE_IRQ();
  return true;
}

// Symbol called when timer interrupt is triggered, does a context switch
void sched_ctx_switch_from_irq() {
  DISABLE_IRQ();

  __asm("sub lr, lr, #4");
  __asm("srsdb sp!, #0x13");
  __asm("cps #0x13");

  __asm("push {r0-r12}");
  __asm("mov %0, sp" : "=r"(current_process->sp));
  current_process->state = READY;
  
  sched_elect();
  
  current_process->state = RUNNING;
  __asm("mov sp, %0" : : "r"(current_process->sp));
  hw_set_tick_and_enable_timer();


  __asm("pop {r0-r12}");
  ENABLE_IRQ();

  __asm("rfeia sp!"); // we're writing back into the Rn registers so we use '!'
}

// Initializes a PCB and add it in the loop
bool sched_new_proc(func_t f, void *args, unsigned int stack_size) {
  struct sched_pcb_s* pcb = kalloc_alloc(sizeof(struct sched_pcb_s));

  if(pcb == 0 || !sched_init_pcb(pcb, f, args, stack_size)) {
    return false;
  }
  sched_add_pcb(pcb);
  return true;
}
