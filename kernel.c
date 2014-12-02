#include "hw.h"
#include "sched.h"

void funcA() {
  int cptA = 0;
  while(1) {
    cptA ++;
  }
}

void funcB() {
  int cptB = 1;
  while(0) {
    cptB += 2 ;
  }
}

//------------------------------------------------------------------------

int kmain(void) {
  init_hw();
  create_process(funcB, NULL, STACK_SIZE);
  create_process(funcA, NULL, STACK_SIZE);
  start_sched();
  /*Pas atteignable vues nos 2 fonctions */
  return 0;
}
