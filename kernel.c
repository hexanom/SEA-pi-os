#include "hw.h"
#include "vmem.h"
#include "kalloc.h"
#include "sched.h"

void funcA() {
  int cptA = 0;
  while(1) {
    cptA++;
  }
}

void funcB() {
  int cptB = 0;
  while(1) {
    cptB+=2;
  }
}

void funcC() {
  int cptC = 0;
  while(1) {
    cptC+=3;
  }
}


bool kinit() {
  return hw_init() &&
    vmem_setup() &&
    kalloc_setup() &&
    sched_new_proc(funcA, NULL, STACK_SIZE, 10) &&
    sched_new_proc(funcB, NULL, STACK_SIZE, 7) &&
    sched_new_proc(funcC, NULL, STACK_SIZE, 5) &&
    sched_start();
}

int kmain(void) {
  if(kinit()) {
    return 0;
  }
  return 1;
}
