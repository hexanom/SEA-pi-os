#include "hw.h"
#include "vmem.h"
#include "kalloc.h"
#include "sched.h"
#include "syscall.h"
#include "fb.h"
#include "pwm.h"

void playWav() {
  audio_test();
}

void ledOn() {
  int cptA = 0;
  while(1) {
    cptA++;
    hw_led_on();
    syscall_wait(5);
  }
}

void ledOff() {
  int cptB = 0;
  while(1) {
    syscall_wait(5);
    cptB++;
    hw_led_off();
  }
}

void red() {
  while(1) {
    drawRed();
    syscall_wait(5);
  }
}

void blue() {
  while(1) {
    syscall_wait(5);
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
    sched_new_proc(playWav, NULL, STACK_SIZE, 99) &&
    sched_new_proc(ledOn, NULL, STACK_SIZE, 99) &&
    sched_new_proc(ledOff, NULL, STACK_SIZE, 95) &&
    sched_new_proc(red, NULL, STACK_SIZE, 99) &&
    sched_new_proc(blue, NULL, STACK_SIZE, 95) &&
    sched_start();
}

int kmain(void) {
  if(kinit()) {
    return 0;
  }
  return 1;
}
