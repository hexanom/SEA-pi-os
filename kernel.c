#include "vmem.h"

int kmain(void) {
  init_kern_translation_table();
  configure_mmu_C();
  start_mmu_C();
  return 0;
}
