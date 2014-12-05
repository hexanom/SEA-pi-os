#include "fpp.h"
#include "phyAlloc.h"
#include "hw.h"

#define WORD_SIZE 4
#define INIT_PRIORITY -10
#define SAVED_REGISTERS 13
#define CPSR_INIT 0x13

struct pcb_s* first_pcb = NULL;
struct pcb_s* last_pcb = NULL;
struct pcb_s* current_process = NULL;
unsigned int process_counter = 1;

void init_pcb(struct pcb_s* pcb, func_t entry_point, void* args, int prio) {
  pcb->sp = ((unsigned int)phyAlloc_alloc(STACK_SIZE));
  pcb->sp += STACK_SIZE;
  pcb->sp -= WORD_SIZE;
  *((int*) pcb->sp) = CPSR_INIT; // cpsr
  pcb->sp -= WORD_SIZE;
  *((int*) pcb->sp) = (unsigned int)&start_current_process; // lr
  pcb->sp -= SAVED_REGISTERS * WORD_SIZE;
  
  pcb->priorityValue = prio;
  
  pcb->entry_point = entry_point;
  pcb->args = args;
  pcb->state = NEW;
  pcb->pid = process_counter++;
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
  phyAlloc_free((void*) current_process->sp, STACK_SIZE);
  phyAlloc_free(current_process, sizeof(struct pcb_s));

  set_tick_and_enable_timer();
  ENABLE_IRQ();
}

int getHighestPriority() {
	
	if(current_process->priorityValue == INIT_PRIORITY)
	{
		return current_process->next_pcb->priorityValue;
	}
	 int currentHP = current_process->priorityValue;
	 struct pcb_s* temp_pcb = current_process;
	 
	do {
		temp_pcb = temp_pcb->next_pcb;
		if(temp_pcb->priorityValue >= currentHP) {
			currentHP = temp_pcb->priorityValue;
		}
	} while(temp_pcb->next_pcb->pid != current_process->pid);
	
	return currentHP;
}

void elect() {
	if(current_process->priorityValue == INIT_PRIORITY)
	{
		current_process = current_process->next_pcb;
		return;
	}
	int currentHP = current_process->priorityValue;
  	struct pcb_s* temp_pcb = current_process;
  	unsigned int entry_pid = current_process->pid;	
	do {
		temp_pcb = temp_pcb->next_pcb;
		if(temp_pcb->priorityValue >= currentHP) {
			currentHP = temp_pcb->priorityValue;
			current_process = temp_pcb;
		}
	} while(temp_pcb->next_pcb->pid != entry_pid);
}

void start_sched() {
  struct pcb_s* kmain_pcb = phyAlloc_alloc(sizeof(struct pcb_s));
  init_pcb(kmain_pcb, NULL, NULL, INIT_PRIORITY);
  kmain_pcb->next_pcb = first_pcb;
  current_process = kmain_pcb;
  set_tick_and_enable_timer();
  ENABLE_IRQ();
}

void ctx_switch_from_irq() {
	
	DISABLE_IRQ();

	__asm("sub lr, lr, #4");
	__asm("srsdb sp!, #0x13");
	__asm("cps #0x13");
	
	int currentHP = getHighestPriority();
	
	if(currentHP >= current_process->priorityValue)
	{
	
	 /*
	DISABLE_IRQ();
	__asm("sub lr, lr, #4");
	__asm("srsdb sp!, #0x13");
	__asm("cps #0x13");
	*/

	__asm("push {r0-r12}");
	__asm("mov %0, sp" : "=r"(current_process->sp));
	current_process->state = READY;

	elect();

	current_process->state = RUNNING;
	__asm("mov sp, %0" : : "r"(current_process->sp));
	set_tick_and_enable_timer();


	__asm("pop {r0-r12}");
	
	/*
	ENABLE_IRQ();
	__asm("rfeia sp!"); // we're writing back into the Rn registers so we use '!'
	*/
	}
	
	ENABLE_IRQ();
	__asm("rfeia sp!");
}

// create process
int create_process(func_t f, void *args, unsigned int stack_size, int prio) {
  struct pcb_s* pcb = phyAlloc_alloc(sizeof(struct pcb_s));
  
  init_pcb(pcb, f, args, prio);
  add_pcb(pcb);
  
  return 0; // ?
}
