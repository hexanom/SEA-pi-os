#include "vmem.h"

int kmain(void) {
  init_kern_translation_table();
  configure_mmu_C();
  int tt = tool_translate(0x101001);
  return tt;
}
