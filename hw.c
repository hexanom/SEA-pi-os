#include "hw.h"

#define HEAP_START 0x50000

#define IRQ_MODE 0x12
#define SVC_MODE 0x13
#define SYS_MODE 0x1F

#define CS      0x20003000
#define CLO     0x20003004
#define C0      0x2000300C
#define C1      0x20003010
#define C2      0x20003014
#define C3      0x20003018

#define GPFSEL1 0x20200004
#define GPSET0  0x2020001C
#define GPCLR0  0x20200028

#define INTERVAL 0x00080000

extern void PUT32 ( unsigned int, unsigned int );
extern uint32 GET32 ( unsigned int );

#define ENABLE_TIMER_IRQ() PUT32(CS,2)
#define DISABLE_TIMER_IRQ() PUT32(CS,~2);

void hw_set_tick_and_enable_timer() {
  uint32 rx = GET32(CLO);
  rx += INTERVAL;
  PUT32(C1,rx);

  ENABLE_TIMER_IRQ();
}

void hw_led_off() {
  PUT32(GPSET0,1<<16); //led off
}

void hw_led_on() {
  PUT32(GPCLR0,1<<16); //led on
}

bool hw_init() {
    uint32 ra;
    uint32 rx;

    /* Make gpio pin tied to the led an output */
    ra=GET32(GPFSEL1);
    ra&=~(7<<18);
    ra|=1<<18;
    PUT32(GPFSEL1,ra);

    //led off
    PUT32(GPSET0,1<<16);

    /* Set up delay before timer interrupt (we use CM1) */
    rx=GET32(CLO);
    rx += INTERVAL;
    PUT32(C1,rx);

    /* Enable irq triggering by the *system timer* peripheral */
    /*   - we use the compare module CM1 */
    ENABLE_TIMER_IRQ();

    /* Enable interrupt *line* */
    PUT32(0x2000B210, 0x00000002);

    DISABLE_IRQ();

    return true;
}
