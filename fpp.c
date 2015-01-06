#include "fpp.h"
#include "kalloc.h"
#include "hw.h"

#define WORD_SIZE 4
#define SAVED_REGISTERS 13
#define CPSR_INIT 0x13

struct fpp_pcb_s* fpp_first_pcb = NULL;
struct fpp_pcb_s* fpp_last_pcb = NULL;
struct fpp_pcb_s* fpp_current_process = NULL;

// PID count
unsigned int process_counter = 1;

// Starts the current process in PCB and remove it when done (execution wrapper)
void fpp_start_current_process() {
  fpp_current_process->state = RUNNING;
  fpp_current_process->entry_point(fpp_current_process->args);
  DISABLE_IRQ();
  // set as done
  fpp_current_process->state = DONE;
  // close loop
  struct fpp_pcb_s* next_pcb = fpp_current_process->next_pcb;
  struct fpp_pcb_s* walk_pcb = next_pcb;
  while(walk_pcb->next_pcb != fpp_current_process) {
    walk_pcb = walk_pcb->next_pcb;
  }
  walk_pcb->next_pcb = next_pcb;
  // dealloc
  fpp_current_process->sp += (1 + 1 + SAVED_REGISTERS) * WORD_SIZE;
  fpp_current_process->sp -= STACK_SIZE;
  kalloc_free((void*) fpp_current_process->sp, STACK_SIZE);
  kalloc_free(fpp_current_process, sizeof(struct fpp_pcb_s));

  hw_set_tick_and_enable_timer();
  ENABLE_IRQ();
}

// Initializes a pcb struct
bool fpp_init_pcb(struct fpp_pcb_s* pcb, func_t entry_point, void* args,
 uint32 stack_size, int priority) {
  pcb->sp = ((unsigned int)kalloc_alloc(stack_size));
  if(pcb->sp == 0) {
    return false;
  }
  pcb->sp += STACK_SIZE;
  pcb->sp -= WORD_SIZE;
  *((int*) pcb->sp) = CPSR_INIT; // cpsr
  pcb->sp -= WORD_SIZE;
  *((int*) pcb->sp) = (unsigned int)&fpp_start_current_process; // lr
  pcb->sp -= SAVED_REGISTERS * WORD_SIZE;

  pcb->entry_point = entry_point;
  pcb->args = args;
  pcb->state = NEW;
  pcb->sleepuntil = 0;
  
  pcb->priority_value = priority;
  pcb->pid = process_counter++;
  pcb->waiting_time = 0;

  return true;
}

// Add a PCB to the LIFO
void fpp_add_pcb(struct fpp_pcb_s* pcb) {
  if(fpp_last_pcb != NULL) {
    fpp_last_pcb->next_pcb = pcb;
  }
  else {
    fpp_first_pcb = pcb;
  }
  pcb->next_pcb = fpp_first_pcb;
  fpp_last_pcb = pcb;
}

// Update a PCB's timer
void fpp_update(struct fpp_pcb_s* pcb) {
  if(pcb->sleepuntil > 0) {
    pcb->sleepuntil--;
  }
}

// Update ALL PCB's timers
void fpp_update_timers() {
  struct fpp_pcb_s* pcb = fpp_first_pcb;
  while(pcb != fpp_last_pcb) {
    fpp_update(pcb);
    pcb = pcb->next_pcb;
  }
  fpp_update(pcb);
}

// Incremente the waiting time of waiting processes
void fpp_incremente_time()
{
	if(fpp_current_process->priority_value == INIT_PRIORITY)
	{
		return;
	}
	unsigned int entry_pid = fpp_current_process->pid;
	struct fpp_pcb_s* temp_pcb = fpp_current_process;
	do {
		temp_pcb = temp_pcb->next_pcb;
		if (temp_pcb->sleepuntil == 0)
		{
			temp_pcb->waiting_time++;
		}
	} while(temp_pcb->next_pcb->pid != entry_pid);
}

// Check if the scheduler have to change current process
bool fpp_have_to_change_process()
{
	if(fpp_current_process->priority_value == INIT_PRIORITY)
	{
		return true;
	}

	// ini temp_pcb at next of current process
	struct fpp_pcb_s* temp_pcb = fpp_current_process->next_pcb;
	// save priority of current process
	int temp_prio = fpp_current_process->priority_value;

    do {
    	if((temp_pcb->waiting_time >= WAITING_LIMIT || temp_pcb->priority_value >= temp_prio) && temp_pcb->sleepuntil == 0) {
    		return true;
    	}
   		temp_pcb = temp_pcb->next_pcb;
   	} while(temp_pcb->pid != fpp_current_process->pid);

   	return false;
}

// Elects the next process
void fpp_elect() {
  fpp_update_timers();
  
	if(fpp_current_process->priority_value == INIT_PRIORITY)
	{
		fpp_current_process = fpp_current_process->next_pcb;
		return;
	}
	
	// initial values
	int highestPriority = fpp_current_process->priority_value;
  	struct fpp_pcb_s* temp_pcb = fpp_current_process->next_pcb; // theoretically useless
  	unsigned int entry_pid = fpp_current_process->pid;	
	
	do {
		if(temp_pcb->waiting_time >= WAITING_LIMIT) {
			fpp_current_process = temp_pcb;
			return;
		}
		else if(temp_pcb->priority_value >= highestPriority && temp_pcb->sleepuntil == 0) {
			highestPriority = temp_pcb->priority_value;
			fpp_current_process = temp_pcb;
		}
		temp_pcb = temp_pcb->next_pcb;
	} while(temp_pcb->pid != entry_pid);
		
	return;
}

// Starts by initializing the kmain's PCB
bool fpp_start() {
  struct fpp_pcb_s* kmain_pcb = kalloc_alloc(sizeof(struct fpp_pcb_s));
  fpp_init_pcb(kmain_pcb, NULL, NULL, STACK_SIZE, INIT_PRIORITY);
  kmain_pcb->next_pcb = fpp_first_pcb;
  fpp_current_process = kmain_pcb;
  hw_set_tick_and_enable_timer();
  ENABLE_IRQ();
  return true;
}

// Symbol called when timer interrupt is triggered, does a context switch
void fpp_ctx_switch_from_irq() {
  DISABLE_IRQ();

  __asm("sub lr, lr, #4");
  __asm("srsdb sp!, #0x13");
  __asm("cps #0x13");

	// incremente waiting time
	fpp_incremente_time();

	// test priority & famine
	if(fpp_have_to_change_process())
	{

	  __asm("push {r0-r12}");
	  __asm("mov %0, sp" : "=r"(fpp_current_process->sp));
	  fpp_current_process->state = READY;
	  
	  fpp_elect();
	  // reinit waiting time
	  fpp_current_process->waiting_time = 0;
	  
	  fpp_current_process->state = RUNNING;
	  __asm("mov sp, %0" : : "r"(fpp_current_process->sp));
	  hw_set_tick_and_enable_timer();


	  __asm("pop {r0-r12}");
  
	}

  ENABLE_IRQ();

  __asm("rfeia sp!"); // we're writing back into the Rn registers so we use '!'
}

// Initializes a PCB and add it in the loop
bool fpp_new_proc(func_t f, void *args, unsigned int stack_size, int priority) {
  struct fpp_pcb_s* pcb = kalloc_alloc(sizeof(struct fpp_pcb_s));

  if(pcb == 0 || !fpp_init_pcb(pcb, f, args, stack_size, priority)) {
    return false;
  }
  fpp_add_pcb(pcb);
  return true;
}
