#include "syscall.h"
#define SYS_REBOOT 1
#define SYS_WAIT 2

void sys_reboot() {
  __asm("mov r0, %0" : : "r"(SYS_REBOOT) : "r0");
  __asm("SWI 0" : : : "lr");
}

void sys_wait(unsigned int quantums) {
  __asm("mov r0, %0" : : "r"(SYS_WAIT) : "r0");
  __asm("mov r1, sp");
  __asm("SWI 0" : : : "lr");
}

void SWIHandler(unsigned int code, unsigned int * params) {
  __asm("mov lr, %0" : : "r"(params[3] + 0x4) : "lr");
  switch (code) {
    case 1:
      doSysCallReboot();
    break;
    case 2:
      doSysCallWait(params[1]);
    break;
  }
}
