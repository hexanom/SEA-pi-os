#ifndef SYSCALL_H
#define SYSCALL_H

#include "sched.h"

/**
 * Asks for a system reboot
 */
void syscall_reboot();

/**
 * Asks to put the current process on wait
 *
 * @param The amount of quantums to wait
 */
void syscall_wait(unsigned int quantums);

/*
 * Handles a syscall
 *
 * @param The syscall's code
 * @param The parameters
 */
void syscall_swi_handler(unsigned int code, unsigned int * params);

#endif
