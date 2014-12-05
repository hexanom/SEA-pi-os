#include "syscall.h"
#define SYS_REBOOT 1
#define SYS_WAIT 2

void sys_reboot() {
  __asm("mov r0, %0" : : "r"(SYS_REBOOT) : "r0");
  __asm("SWI 0" : : : "lr");
}

void doSysCallReboot() {
  const int PM_RSTC = 0x2010001c;
  const int PM_WDOG = 0x20100024;
  const int PM_PASSWORD = 0x5a000000;
  const int PM_RSTC_WRCFG_FULL_RESET = 0x00000020;
  PUT32(PM_WDOG, PM_PASSWORD | 1);
  PUT32(PM_RSTC, PM_PASSWORD | PM_RSTC_WRCFG_FULL_RESET);
  while(1);
}

void sys_wait(unsigned int nbQuantums){
  __asm("mov r0, %0" : : "r"(SYS_WAIT) : "r0");
  __asm("mov r1, sp");
  __asm("SWI 0" : : : "lr");
}

void doSysCallWait(unsigned int nbQuantums) {
  
}

void SWIHandler(unsigned int code, unsigned int * params) {
  switch (code) {
    case 1:
      doSysCallReboot();
    break;
    case 2:
      doSysCallWait(params[1]);
    break;
  }
}
