#ifndef VMEM_H
#define VMEM_H

#define PAGE_SIZE 4 * 1024
#define FIRST_LVL_TT_COUN 4096
#define FIRST_LVL_TT_SIZE FIRST_LVL_TT_COUN * 4
#define SECON_LVL_TT_COUN 256
#define SECON_LVL_TT_SIZE SECON_LVL_TT_COUN * 4
#define TOTAL_TT_SIZE FIRST_LVL_TT_SIZE + FIRST_LVL_TT_SIZE * SECON_LVL_TT_SIZE
#define FIRST_LVL_TT_POS 0x48000
#define SECON_LVL_TT_POS FIRST_LVL_TT_POS + FIRST_LVL_TT_COUN



unsigned int first_tt_flags =
  (0 << 9) | // P = 0
  (0 << 5) | // Domain = 0
  (0 << 4) | // SBZ = 0
  (0 << 3) | // NS = 0
  (0 << 2) | // SBZ = 0
  (0 << 1) | // Always 0
  (1 << 0); // Always 1

unsigned int device_flags =
  (0 << 11) | // nG = 0
  (0 << 10) | // S = 0
  (0 << 9) | // APX = 0
  (001 << 6) | // TEX = 001
  (00 << 4) | // AP = 00
  (1 << 3) | // C = 1
  (1 << 2) | // B = 1
  (1 << 1) | // Always 1
  (0 << 0); // XN = 0

unsigned int normal_flags =
  (0 << 11) | // nG = 0
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

unsigned int tool_translate(unsigned int va)

#endif
