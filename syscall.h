#ifndef SYSCALL_H
#define SYSCALL_H

void sys_call(unsigned int code);
void sys_reboot();
inline void doSysCallReboot() __attribute__((always_inline));
void sys_wait(unsigned int quantums);
inline void doSysCallWait(unsigned int quantums) __attribute__((always_inline));
void SWIHandler(unsigned int code, unsigned int * params);

#endif
