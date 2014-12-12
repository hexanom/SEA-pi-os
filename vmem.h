#ifndef VMEM_H
#define VMEM_H

#include "types.h"

bool vmem_setup();
uint8* vmem_alloc(uint32 pages);
bool vmem_free(uint8* ptr, uint32 pages);

#endif
