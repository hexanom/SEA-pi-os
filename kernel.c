#include <stdio.h>
#include "hw.h"
//#include "mmu.h"
#include "sched.h"
#include "syscall.h"

void funcA() {
  int cptA = 0;
  while(1) {
    cptA ++;
    sys_wait(1);
  }
}

void funcB() {
  int cptB = 1;
  while(1) {
    cptB += 2 ;
  }
}

//------------------------------------------------------------------------

int kmain(void) {
  init_hw();
  //start_mmu_C();
  //configure_mmu_C();
  create_process(funcB, NULL, STACK_SIZE);
  create_process(funcA, NULL, STACK_SIZE);
  start_sched();
  /*Pas atteignable vues nos 2 fonctions */
  return 0;
}
