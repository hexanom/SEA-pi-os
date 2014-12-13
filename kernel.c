#include "hw.h"
#include "vmem.h"
#include "kalloc.h"

bool kinit() {
  return hw_init() &&
    vmem_setup() &&
    kalloc_setup();
}

int kmain(void) {
  if(kinit()) {
    return 0;
  }
  return 1;
}
