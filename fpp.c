#include "fpp.h"

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
  
  // init start time
  pcb->waiting_time = 0;
  
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

// Incremente the waiting time of waiting processes
void incremente_time()
{
	if(current_process->priorityValue == INIT_PRIORITY)
	{
		return;
	}
	unsigned int entry_pid = current_process->pid;
	struct pcb_s* temp_pcb = current_process;
	do {
		temp_pcb = temp_pcb->next_pcb;
		temp_pcb->waiting_time++;
	} while(temp_pcb->next_pcb->pid != entry_pid);
}

// Check if the scheduler have to change current process
char haveToChangeProcess()
{
	if(current_process->priorityValue == INIT_PRIORITY)
	{
		return TRUE;
	}

	// ini temp_pcb at next of current process
	struct pcb_s* temp_pcb = current_process->next_pcb;
	// save priority of current process
	int temp_prio = current_process->priorityValue;

    do {
    	if(temp_pcb->waiting_time >= WAITING_LIMIT || temp_pcb->priorityValue >= temp_prio) {
    		return TRUE;
    	}
   		temp_pcb = temp_pcb->next_pcb;
   	} while(temp_pcb->pid != current_process->pid);

   	return FALSE;
}


void elect() {
	if(current_process->priorityValue == INIT_PRIORITY)
	{
		current_process = current_process->next_pcb;
		return;
	}
	
	// initial values
	int highestPriority = current_process->priorityValue;
  	struct pcb_s* temp_pcb = current_process->next_pcb; // theoretically useless
  	unsigned int entry_pid = current_process->pid;	
	
	do {
		if(temp_pcb->waiting_time >= WAITING_LIMIT) {
			current_process = temp_pcb;
			return;
		}
		else if(temp_pcb->priorityValue >= highestPriority) {
			highestPriority = temp_pcb->priorityValue;
			current_process = temp_pcb;
		}
		temp_pcb = temp_pcb->next_pcb;
	} while(temp_pcb->pid != entry_pid);
		
	return;
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
	
	// incremente waiting time
	incremente_time();

	// test priority & famine
	if(haveToChangeProcess())
	{

		__asm("push {r0-r12}");
		__asm("mov %0, sp" : "=r"(current_process->sp));
		current_process->state = READY;

		elect();
		// reinit waiting time
		current_process->waiting_time = 0;
		
		// determine execution time
		/*int execute_time = 42;
		
	   __asm("MCR p15, 0, %0, c15, c12, 0": "=r" (execute_time)); //Write Performance Monitor Control Register
	   
		  int result = 10;
		 __asm("MRC p15, 0, %0, c15, c12, 0": "=r" (result));// read*/
	
	  
	    
		current_process->state = RUNNING;
		__asm("mov sp, %0" : : "r"(current_process->sp));
		set_tick_and_enable_timer();

		__asm("pop {r0-r12}");
	
	}
	
	ENABLE_IRQ();
	__asm("rfeia sp!");
}

// create process
int create_process(func_t f, void *args, unsigned int stack_size, int prio) {
  struct pcb_s* pcb = phyAlloc_alloc(sizeof(struct pcb_s));
  
  init_pcb(pcb, f, args, prio);
  add_pcb(pcb);
  
  return 0;
}
