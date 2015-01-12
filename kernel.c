#include "hw.h"
#include "vmem.h"
#include "kalloc.h"
#include "sched.h"
#include "syscall.h"

void funcA() {
  int cptA = 0;
  while(1) {
    cptA++;
    syscall_wait(1000);
  }
}

void funcB() {
  int cptB = 0;
  while(1) {
    cptB+=2;
    syscall_wait(10000);
  }
}


bool kinit() {
  return hw_init() &&
    vmem_setup() &&
    kalloc_setup() &&
    sched_new_proc(funcA, NULL, STACK_SIZE, 99) &&
    sched_new_proc(funcB, NULL, STACK_SIZE, 95) &&
    sched_start();
}

int kmain(void) {
  if(kinit()) {
    return 0;
  }
  return 1;
}
