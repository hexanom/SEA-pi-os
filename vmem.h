#ifndef VMEM_H
#define VMEM_H

#include "types.h"

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

/**
 * Initialize the Pagination system (1st & 2nd level Translation Tables)
 * Configure the MMU to lookup those translation tables
 * Start the MMU
 * Initialize the Virtual Memory table
 *
 * @return the success of those operations
 */
bool vmem_setup();

/**
 * Allocates pages in the non-memory management or device space
 *
 * @return a memory address at the allocated space or 0 if the allocation failed
 */
uint8* vmem_alloc(uint32 pages);

/**
 * Deallocate pages in the non-memory management or device space
 *
 * @return the success of the operation
 */
bool vmem_free(uint8* ptr, uint32 pages);

#endif
