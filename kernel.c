#include "hw.h"
#include "vmem.h"
#include "kalloc.h"
#include "sched.h"
#include "fpp.h"
#include "userspace.h"

bool kinit() {
  return hw_init() &&
    vmem_setup() &&
    kalloc_setup() &&
    sched_new_proc(simple_init_system, NULL, STACK_SIZE) &&
    sched_start();
}

int kmain(void) {
  if(kinit()) {
    return 0;
  }
  return 1;
}
