#include "scheduler.h"
#ifndef FPP_H
#define FPP_H

#define NULL 0

#define TRUE 1
#define FALSE 0
#define WAITING_LIMIT 10

void init_pcb(struct pcb_s* pcb, func_t entry_point, void* args, int prio);
void add_pcb(struct pcb_s* pcb);

int create_process(func_t f, void *args, unsigned int stack_size, int prio);

void start_sched();
void start_current_process();
void elect();
void ctx_switch_from_irq();
void incremente_time();
char haveToChangeProcess();

#endif
