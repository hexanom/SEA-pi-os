#include "vmem.h"

unsigned int init_kern_translation_table(void) {
  unsigned int* ftt_i = (unsigned int *)FIRST_LVL_TT_POS;
  unsigned int* stt_i = (unsigned int *)SECON_LVL_TT_POS;
  for(int i = 0; i < FIRST_LVL_TT_COUN; i ++) {
    ftt_i[i * SECON_LVL_TT_SIZE] = (unsigned int)
      first_tt_flags |
      ((SECON_LVL_TT_POS + i * SECON_LVL_TT_SIZE) << 10);
  }
  for(int i = 0; i < FIRST_LVL_TT_COUN * SECON_LVL_TT_COUN; i ++) {
    if(i < 0x20000000) {
      stt_i[i] = (unsigned int)
        normal_flags |
        (i << 12);
    } else if(i < 0x20FFFFFF) {
      stt_i[i] = (unsigned int)
        device_flags |
        (i << 12);
    } else {
      stt_i[i] = (unsigned int) 0; // Page fault
    }
  }
  return 0;
}

void start_mmu_C() {
  register unsigned int control;

  __asm("mcr p15, 0, %[zero], c1, c0, 0" : : [zero] "r"(0)); //Disable cache
  __asm("mcr p15, 0, r0, c7, c7, 0"); //Invalidate cache (data and instructions) */
  __asm("mcr p15, 0, r0, c8, c7, 0"); //Invalidate TLB entries

  /* Enable ARMv6 MMU features (disable sub-page AP) */
  control = (1<<23) | (1 << 15) | (1 << 4) | 1;
  /* Invalidate the translation lookaside buffer (TLB) */
  __asm volatile("mcr p15, 0, %[data], c8, c7, 0" : : [data] "r" (0));
  /* Write control register */
  __asm volatile("mcr p15, 0, %[control], c1, c0, 0" : : [control] "r" (control));
}

void configure_mmu_C() {
  register unsigned int pt_addr = ((unsigned int)FIRST_LVL_TT_POS);
  /* Translation table 0 */
  __asm volatile("mcr p15, 0, %[addr], c2, c0, 0" : : [addr] "r" (pt_addr));
  /* Translation table 1 */
  __asm volatile("mcr p15, 0, %[addr], c2, c0, 1" : : [addr] "r" (pt_addr));
  /* Use translation table 0 for everything */
  __asm volatile("mcr p15, 0, %[n], c2, c0, 2" : : [n] "r" (0));
  /* Set Domain 0 ACL to "Manager", not enforcing memory permissions
   * Every mapped section/page is in domain 0
   */
  __asm volatile("mcr p15, 0, %[r], c3, c0, 0" : : [r] "r" (0x3));
}

unsigned int tool_translate(unsigned int va) {
  unsigned int pa; /* the result */

  /* 1st and 2nd table addresses */
  unsigned int table_base;
  unsigned int second_level_table;

  /* Indexes */
  unsigned int first_level_index;
  unsigned int second_level_index;
  unsigned int page_index;

  /* Descriptors */
  unsigned int first_level_descriptor;
  unsigned int* first_level_descriptor_address;
  unsigned int second_level_descriptor;
  unsigned int* second_level_descriptor_address;

  __asm("mrc p15, 0, %[tb], c2, c0, 0" : [tb] "=r"(table_base));

  table_base = table_base & 0xFFFFC000;

  /* Indexes*/
  first_level_index = (va >> 20);
  second_level_index = ((va << 12) >> 24);
  page_index = (va & 0x00000FFF);

  /* First level descriptor */
  first_level_descriptor_address = (unsigned int*) (table_base |
    (first_level_index << 2));
    first_level_descriptor = *(first_level_descriptor_address);

  /* Second level descriptor */
  second_level_table = first_level_descriptor & 0xFFFFFC00;
  second_level_descriptor_address = (unsigned int*) (second_level_table
    | (second_level_index << 2));
  second_level_descriptor = *((unsigned int*)
    second_level_descriptor_address);

  /* Physical address */
  pa = (second_level_descriptor & 0xFFFFF000) | page_index;

  return pa;
}
