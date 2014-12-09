#ifndef VMEM_H
#define VMEM_H

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
#define PAGE_SIZE 4 * 1024 // 4KB
#define FIRST_LVL_TT_COUN 4096
#define FIRST_LVL_TT_SIZE FIRST_LVL_TT_COUN * TT_ENTRY_SIZE // 16384
#define SECON_LVL_TT_COUN 256
#define SECON_LVL_TT_SIZE SECON_LVL_TT_COUN * TT_ENTRY_SIZE // 1024
#define TOTAL_TT_SIZE FIRST_LVL_TT_SIZE + FIRST_LVL_TT_COUN * SECON_LVL_TT_SIZE
#define FIRST_LVL_TT_POS 0x48000
#define SECON_LVL_TT_POS FIRST_LVL_TT_POS + FIRST_LVL_TT_SIZE // 0x4C000

#define VMEM_ALLOC_T_START FIRST_LVL_TT_POS + TOTAL_TT_SIZE
#define VMEM_TOTAL_SIZE 0x20FFFFFF
#define VMEM_TOTAL_PAGES VMEM_TOTAL_SIZE/PAGE_SIZE
#define VMEM_FIRS_RESERVED_PAGES 1356 // Pages until the end of the lookup tables + Pages for the VMEM table
#define VMEM_LAST_RESERVED_PAGES 4095 // Devices pages

// 0x48000 + (4096 + 4096*256)*4

unsigned int init_kern_translation_table(void);

void start_mmu_C();
void configure_mmu_C();

unsigned char* vmem_alloc(unsigned int pages);
void vmem_free(unsigned char* ptr, unsigned int pages);

// unsigned int tool_translate(unsigned int va); // For debug purposes only

#endif
