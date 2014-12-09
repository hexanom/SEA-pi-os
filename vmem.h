#ifndef VMEM_H
#define VMEM_H

#define TT_ENTRY_SIZE 4
#define PAGE_SIZE 4 * 1024 // 4KB
#define FIRST_LVL_TT_COUN 4096
#define FIRST_LVL_TT_SIZE FIRST_LVL_TT_COUN * TT_ENTRY_SIZE // 16384
#define SECON_LVL_TT_COUN 256
#define SECON_LVL_TT_SIZE SECON_LVL_TT_COUN * TT_ENTRY_SIZE // 1024
#define TOTAL_TT_SIZE FIRST_LVL_TT_SIZE + FIRST_LVL_TT_SIZE * SECON_LVL_TT_SIZE // 16777216
#define FIRST_LVL_TT_POS 0x48000
#define SECON_LVL_TT_POS FIRST_LVL_TT_POS + FIRST_LVL_TT_COUN // 0x49000

unsigned int init_kern_translation_table(void);

void start_mmu_C();
void configure_mmu_C();

unsigned int tool_translate(unsigned int va);

#endif
