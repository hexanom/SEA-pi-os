#ifndef HW_H
#define HW_H

#include "types.h"

/** @macro instruction to enable interrupts */
#define ENABLE_IRQ()				\
  asm("\tpush {r0}\n\t"				\
      "mrs r0,cpsr\n\t"				\
      "bic r0,r0,#0x80\n\t"			\
      "msr cpsr_c,r0\n\t"			\
      "pop {r0}"				\
      :						\
      :						\
      : "r0");

/** @macro instruction to disable interrupts */
#define DISABLE_IRQ()				\
  asm("\tpush {r0}\n\t"				\
      "mrs r0,cpsr\n\t"				\
      "orr r0,r0,#0x80\n\t"			\
      "msr cpsr_c,r0\n\t"			\
      "pop {r0}"				\
      :						\
      :						\
      : "r0");

/**
 * Set a delay of 0x80000 and enable the IRQ timer
 */
void hw_set_tick_and_enable_timer();

/**
 * Shut the led
 */
void hw_led_off();

/**
 * Light the led
 */
void hw_led_on();

/**
 * Init the hardware
 *
 * @return the success of the operation
 */
bool hw_init();

#endif
