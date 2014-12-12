#include "hw.h"
#include "scheduler.h"
#include "fpp.h"
#include <stdio.h>

void funcA() {
  int cptA = 0;
  while(1) {
    cptA ++;
  }
}

void funcB() {
  int cptB = 1;
  while(1) {
    cptB += 2 ;
  }
}

void funcC() {
  int cptC = 1;
  while(1) {
    cptC += 3 ;
  }
}

//------------------------------------------------------------------------

int kmain(void) {


   /*printf ("******* Choose your Scheduler ******\n");
   printf ("1) Round Robin ******\n");
   printf ("2) Fixed Priority Preemtive\n");
   printf ("3) Shortest Job First\n");*/
   
	
  init_hw();
  create_process(funcB, NULL, STACK_SIZE, 19);
  create_process(funcA, NULL, STACK_SIZE, 15);
  create_process(funcC, NULL, STACK_SIZE, 19);
  start_sched();
  /*Pas atteignable vues nos 2 fonctions */
  return 0;
}
