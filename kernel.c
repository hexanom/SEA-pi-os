#include "vmem.h"

int kmain(void) {
  if(vmem_setup()) {
    unsigned char* p1, * p2, * p3;
    p1 = vmem_alloc(10);
    p2 = vmem_alloc(5);
    vmem_free(p1, 10);
    p3 = vmem_alloc(10);
    return 0;
  }
  return 1;
}
