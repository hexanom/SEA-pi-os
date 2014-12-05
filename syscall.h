#ifndef SYSCALL_H
#define SYSCALL_H

void sys_call(unsigned int code);
void sys_reboot();
void doSysCallReboot();
void sys_wait(unsigned int quantums);
void doSysCallWait(unsigned int quantums);
void SWIHandler(unsigned int code, unsigned int * params);

#endif
