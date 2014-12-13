#ifndef KALLOC_H
#define KALLOC_H

#include "types.h"

/**
 * Allocates a few pages for the kernel and initializes the kalloc's heap
 *
 * Note: you must enable the vmem before calling this
 * @return the success of the operation
 */
bool kalloc_setup();

/**
 * Allocate memory for the kernel
 *
 * @param the size of the zone to allocate
 * @return a pointer to the start of the zone
 */
uint8* kalloc_alloc(uint32 size);

/**
 * Free an allocated part of the memory
 *
 * @param the pointer to the start of the zone to free
 * @param the size of the zone to free
 * @return the success of the operation
 */
bool kalloc_free(uint8* ptr, uint32 size);

#endif
