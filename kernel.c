#include "hw.h"
#include "vmem.h"
#include "sched.h"
#include "syscall.h"

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
    sys_wait(1);
  }
}

int kmain(void) {
  init_kern_translation_table();
  configure_mmu_C();
  start_mmu_C();
  vmem_init();
  unsigned char* p1, * p2, * p3;
  p1 = vmem_alloc(10);
  p2 = vmem_alloc(5);
  vmem_free(p1, 10);
  p3 = vmem_alloc(10);
  return 0;
}
