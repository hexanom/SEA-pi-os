#ifndef VMEM_H
#define VMEM_H

#define PAGE_SIZE 4 * 1024
#define FIRST_LVL_TT_COUN 4096
#define FIRST_LVL_TT_SIZE FIRST_LVL_TT_COUN * 4
#define SECON_LVL_TT_COUN 256
#define SECON_LVL_TT_SIZE SECON_LVL_TT_COUN * 4
#define TOTAL_TT_SIZE FIRST_LVL_TT_SIZE + FIRST_LVL_TT_SIZE * SECON_LVL_TT_SIZE
#define FIRST_LVL_TT_POS 0x48000
#define SECON_LVL_TT_POS FIRST_LVL_TT_POS + FIRST_LVL_TT_COUNT

uint32_t device_flags =
  (1 << 11) | // nG = 0
  (0 << 10) | // S = 0
  (0 << 9) | // APX = 0
  (001 << 6) | // TEX = 001
  (00 << 4) | // AP = 00
  (1 << 3) | // C = 1
  (1 << 2) | // B = 1
  (1 << 1) | // Always 1
  (0 << 0); // XN = 0

unsigned int init_kern_translation_table(void);

void start_mmu_C();
void configure_mmu_C();

#endif
