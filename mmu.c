#include "mmu.h"
#include "phyAlloc.c"

// note: this may be wrong:
#define TABLE_SIZE 32

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
  register unsigned int pt_addr = ((unsigned int)phyAlloc_alloc(TABLE_SIZE));
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
