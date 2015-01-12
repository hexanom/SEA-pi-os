#include "hw.h"
#include "vmem.h"
#include "kalloc.h"
#include "sched.h"
#include "syscall.h"
#include "fb.h"


void funcA() {
  int cptA = 0;
  while(1) {
    cptA++;
    hw_led_on();
    drawRed();
  }
}

void funcB() {
  int cptB = 0;
  while(1) {
    cptB++;
    hw_led_off();
    drawBlue();
  }
}

bool fb_init() {
  FramebufferInitialize();
  return true;
}



bool kinit() {
  return hw_init() &&
    vmem_setup() &&
    kalloc_setup() &&
    fb_init() &&
    sched_new_proc(funcA, NULL, STACK_SIZE, 99) &&
    sched_new_proc(funcB, NULL, STACK_SIZE, 95) &&
    sched_start();
}

int kmain(void) {
  if(kinit()) {
    return 0;
  }
  return 1;
}
