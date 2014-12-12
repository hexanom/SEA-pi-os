#include "vmem.h"
#include "errno.h"


/*
 * MEMORY SCHEMA (not proportional)
 *
 *           _________________
 * 0x20FFFFFF | <reserved>  | ^
 *            | I/O Devices | 4095 pages
 *            |             | |
 * 0x20000000_+_____________+_v
 * 0x1FFFFFFF | <free>      | ^
 *            | Programs    | 1043125 pages
 *            |   heaps     | |
 *            | &           | |
 *            | Other       | |
 *            |   kernel    | |
 *            |   parts's   | |
 *            |   memory    | |
 *            |    ex:      | |
 *            |     PCBs    | |
 *            |             | |
 *            |             | |
 *            |             | |
 *            |             | |
 * 0x54C000  _+_____________+_v
 * 0x54BFFF   | <reserved>  | ^
 *            | vmem table  | 1356 pages
 * 0x44C000  _+_____________+ |
 * 0x44BFFF   | <reserved>  | |
 *            | 2nd lvl tt  | |
 * 0x4C000   _+_____________+ |
 * 0x4BFFF    | <reserved>  | |
 *            | 1st lvl tt  | |
 * 0x48000   _+_____________+ |
 * 0x47FFF    | <reserved>  | |
 *            | SVC Stack   | |
 * 0x40000   _+_____________+ |
 * 0x3FFFF    | <reserved>  | |
 *            | IRQ Stack   | |
 * 0x8000    _+_____________+ |
 * 0x7FFF     | <reserved>  | |
 *            | Inter. Vec. | |
 * 0x0       _+_____________+_v
 */

#define TT_ENTRY_SIZE 4
#define PAGE_SIZE 4096 // 4KB
#define FIRST_LVL_TT_COUN 4096
#define FIRST_LVL_TT_SIZE FIRST_LVL_TT_COUN * TT_ENTRY_SIZE // 16384
#define SECON_LVL_TT_COUN 256
#define SECON_LVL_TT_SIZE SECON_LVL_TT_COUN * TT_ENTRY_SIZE // 1024
#define TOTAL_TT_SIZE FIRST_LVL_TT_SIZE + FIRST_LVL_TT_COUN * SECON_LVL_TT_SIZE
#define FIRST_LVL_TT_POS 0x48000
#define SECON_LVL_TT_POS FIRST_LVL_TT_POS + FIRST_LVL_TT_SIZE // 0x4C000

#define VMEM_ALLOC_T_START FIRST_LVL_TT_POS + TOTAL_TT_SIZE // 0x54C000
#define VMEM_TOTAL_SIZE 0x20FFFFFF
#define VMEM_TOTAL_PAGES VMEM_TOTAL_SIZE/PAGE_SIZE // 0xFFFFF
#define VMEM_FIRS_RESERVED_PAGES 1356 // Pages until the end of the lookup tables + Pages for the VMEM table
#define VMEM_LAST_RESERVED_PAGES 4095 // Devices pages

uint32 first_tt_flags =
  (0 << 9) | // P = 0
  (0 << 5) | // Domain = 0
  (0 << 4) | // SBZ = 0
  (0 << 3) | // NS = 0
  (0 << 2) | // SBZ = 0
  (0 << 1) | // Always 0
  (1 << 0); // Always 1

uint32 device_flags =
  (0 << 11) | // nG = 0
  (0 << 10) | // S = 0
  (0 << 9) | // APX = 0
  (001 << 6) | // TEX = 001
  (00 << 4) | // AP = 00
  (1 << 3) | // C = 1
  (1 << 2) | // B = 1
  (1 << 1) | // Always 1
  (0 << 0); // XN = 0

uint32 normal_flags =
  (0 << 11) | // nG = 0
  (0 << 10) | // S = 0
  (0 << 9) | // APX = 0
  (001 << 6) | // TEX = 001
  (00 << 4) | // AP = 00
  (1 << 3) | // C = 1
  (1 << 2) | // B = 1
  (1 << 1) | // Always 1
  (0 << 0); // XN = 0

uint8* vmem_table = (uint8*) VMEM_ALLOC_T_START;

bool tt_init(void) {
  uint32* ftt_a = (uint32 *)FIRST_LVL_TT_POS;
  uint32 page_i = 0;
  for(uint64 i = 0; i < FIRST_LVL_TT_COUN; i ++) {
    uint32* stt_a = (uint32 *)(SECON_LVL_TT_POS + (i << 10));
    ftt_a[i] = (uint32)
      first_tt_flags |
      ((uint32)stt_a & 0xFFFFFC00); // [31…second_lvl_table_addr(22MSBs)…10|9…flags…0]
    for(uint32 j = 0;j < SECON_LVL_TT_COUN; j ++) {
      uint32 val = 0; // Page Fault
      if(page_i < 0x20000000) {
        val = normal_flags |
          (page_i << 12); // [31…page_address(20MSBs)…12|11…flags…0]
      } else if(i < 0x20FFFFFF) {
        val = device_flags |
          (page_i << 12);
      }
      stt_a[j] = val;
      page_i ++;
    }
  }
  return true;
}

bool mmuc_start() {
  register uint32 control;

  __asm("mcr p15, 0, %[zero], c1, c0, 0" : : [zero] "r"(0)); //Disable cache
  __asm("mcr p15, 0, r0, c7, c7, 0"); //Invalidate cache (data and instructions) */
  __asm("mcr p15, 0, r0, c8, c7, 0"); //Invalidate TLB entries

  /* Enable ARMv6 MMU features (disable sub-page AP) */
  control = (1<<23) | (1 << 15) | (1 << 4) | 1;
  /* Invalidate the translation lookaside buffer (TLB) */
  __asm volatile("mcr p15, 0, %[data], c8, c7, 0" : : [data] "r" (0));
  /* Write control register */
  __asm volatile("mcr p15, 0, %[control], c1, c0, 0" : : [control] "r" (control));
  return true;
}

bool mmuc_config() {
  register uint32 pt_addr = ((unsigned int)FIRST_LVL_TT_POS);
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
  return true;
}

bool vmem_init() {
  for(uint32 i = 0; i < VMEM_TOTAL_PAGES; i ++) {
    if(i <= VMEM_FIRS_RESERVED_PAGES ||
       i >= VMEM_TOTAL_PAGES - VMEM_LAST_RESERVED_PAGES) {
      vmem_table[i] = 1;
    } else {
      vmem_table[i] = 0;
    }
  }
  return true;
}

bool vmem_setup() {
  return tt_init() &&
    mmuc_config() &&
    mmuc_start() &&
    vmem_init();
}

// first fit
uint8* vmem_alloc(uint32 pages) {
  for(uint64 i = 0; i < VMEM_TOTAL_PAGES; i ++) {
    if(vmem_table[i] == 0) {
      bool fit = true;
      for(uint64 j = 0; j < pages; j ++) {
        if(vmem_table[i + j] != 0) {
          fit = false;
          break;
        }
      }
      if(fit) {
        for(uint64 j = 0; j < pages; j ++) {
          vmem_table[i + j] = 1;
        }
        return (uint8 *)(i * PAGE_SIZE);
      }
    }
  }
  return false;
}

bool vmem_free(uint8* ptr, uint32 pages) {
  uint32 page = (uint32)(ptr)/PAGE_SIZE;
  if(page > VMEM_FIRS_RESERVED_PAGES &&
     page + pages < VMEM_TOTAL_PAGES - VMEM_LAST_RESERVED_PAGES) {
    for(uint32 i = 0; i < pages; i ++) {
      vmem_table[page + i] = 0;
    }
    return true;
  }
  return false;
}

unsigned int tool_translate(unsigned int va) {
  unsigned int pa; /* the result */

  /* 1st and 2nd table addresses */
  unsigned int table_base;
  unsigned int second_level_table;

  /* Indexes */
  unsigned long first_level_index;
  unsigned long second_level_index;
  unsigned long page_index;

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
  second_level_descriptor = *second_level_descriptor_address;

  /* Physical address */
  pa = (second_level_descriptor & 0xFFFFF000) | page_index;

  return pa;
}
