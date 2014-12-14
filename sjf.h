#include "scheduler.h"
#ifndef SJF_H
#define SJF_H

void init_pcb(struct pcb_s* pcb, func_t entry_point, void* args);
void add_pcb(struct pcb_s* pcb);

int create_process(func_t f, void *args, unsigned int stack_size);

void start_sched();
void start_current_process();
void elect();
void ctx_switch_from_irq();
void incremente_time();
char haveToChangeProcess();

#endif
