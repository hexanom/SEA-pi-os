#ifndef VMEM_H
#define VMEM_H

// v-- Ca deconne --v
#define PAGE_SIZE 4 * 1024
#define FIRST_LVL_TT_COUN 4096
#define FIRST_LVL_TT_SIZE FIRST_LVL_TT_COUN * 4
#define SECON_LVL_TT_COUN 256
#define SECON_LVL_TT_SIZE SECON_LVL_TT_COUN * 4
#define TOTAL_TT_SIZE FIRST_LVL_TT_SIZE + FIRST_LVL_TT_SIZE * SECON_LVL_TT_SIZE
#define FIRST_LVL_TT_POS 0x48000
#define SECON_LVL_TT_POS FIRST_LVL_TT_POS + FIRST_LVL_TT_COUN

unsigned int init_kern_translation_table(void);

void start_mmu_C();
void configure_mmu_C();

unsigned int tool_translate(unsigned int va);

#endif
