#ifndef SYSCALL_H
#define SYSCALL_H

void sys_call(unsigned int code);
void sys_reboot();
void doSysCallReboot();
void sys_wait(unsigned int nbQuantums);
void doSysCallWait(unsigned int nbQuantums);
void SWIHandler(unsigned int code, unsigned int * params);

#endif
