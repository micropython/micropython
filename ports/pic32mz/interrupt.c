#include <stdint.h>
#include <xc.h>
#include "pic32_config.h"

#define IFSbits_TIF(x, y) IFS##y##bits.T##x##IF

#define ipltmr1AUTO iplAUTO(TIMER1_INTERRUPT_PERIORITY)
#define ipltmr2AUTO iplAUTO(TIMER2_INTERRUPT_PERIORITY)
#define ipltmr3AUTO iplAUTO(TIMER3_INTERRUPT_PERIORITY)
#define ipltmr4AUTO iplAUTO(TIMER4_INTERRUPT_PERIORITY)
#define ipltmr5AUTO iplAUTO(TIMER5_INTERRUPT_PERIORITY)
#define ipltmr6AUTO iplAUTO(TIMER6_INTERRUPT_PERIORITY)
#define ipltmr7AUTO iplAUTO(TIMER7_INTERRUPT_PERIORITY)
#define ipltmr8AUTO iplAUTO(TIMER8_INTERRUPT_PERIORITY)
#define ipltmr9AUTO iplAUTO(TIMER9_INTERRUPT_PERIORITY)

#define iplAUTO(x) ipl(x)
#define ipl(x) ipl##x##AUTO

uint32_t global_tick = 0;

void __attribute__((vector(_TIMER_1_VECTOR), interrupt(ipltmr1AUTO), nomips16)) _timer1_interrupt(void) {
    IFS0bits.T1IF = 0;
    global_tick += 1;
}

#if (TIMER_2_ENABLED == 1)
void __attribute__((weak)) timer_2_callback(void) {
}

void void __attribute__((vector(_TIMER_2_VECTOR), interrupt(ipltmr2AUTO), nomips16)) _timer2_interrupt(void) {
    IFSbits_TIF(2, 0) = 0;
    timer_2_callback();
}
#endif

#if (TIMER_3_ENABLED == 1)
void __attribute__((weak)) timer_3_callback(void) {
}

void void __attribute__((vector(_TIMER_3_VECTOR), interrupt(ipltmr3AUTO), nomips16)) _timer3_interrupt(void) {
    IFSbits_TIF(3, 0) = 0;
    timer_3_callback();
}
#endif

#if (TIMER_4_ENABLED == 1)
void __attribute__((weak)) timer_4_callback(void) {
}

void void __attribute__((vector(_TIMER_4_VECTOR), interrupt(ipltmr4AUTO), nomips16)) _timer4_interrupt(void) {
    IFSbits_TIF(4, 0) = 0;
    timer_4_callback();
}
#endif

#if (TIMER_5_ENABLED == 1)
void __attribute__((weak)) timer_5_callback(void) {
}

void void __attribute__((vector(_TIMER_5_VECTOR), interrupt(ipltmr5AUTO), nomips16)) _timer5_interrupt(void) {
    IFSbits_TIF(5, 0) = 0;
    timer_5_callback();
}
#endif

#if (TIMER_6_ENABLED == 1)
void __attribute__((weak)) timer_6_callback(void) {
}

void void __attribute__((vector(_TIMER_6_VECTOR), interrupt(ipltmr6AUTO), nomips16)) _timer6_interrupt(void) {
    IFSbits_TIF(6, 0) = 0;
    timer_6_callback();
}
#endif

#if (TIMER_7_ENABLED == 1)
void __attribute__((weak)) timer_7_callback(void) {
}

void void __attribute__((vector(_TIMER_7_VECTOR), interrupt(ipltmr7AUTO), nomips16)) _timer7_interrupt(void) {
    IFSbits_TIF(7, 1) = 0;
    timer_7_callback();
}
#endif

#if (TIMER_8_ENABLED == 1)
void __attribute__((weak)) timer_8_callback(void) {
}

void void __attribute__((vector(_TIMER_8_VECTOR), interrupt(ipltmr8AUTO), nomips16)) _timer8_interrupt(void) {
    IFSbits_TIF(8, 1) = 0;
    timer_8_callback();
}
#endif

#if (TIMER_9_ENABLED == 1)
void __attribute__((weak)) timer_9_callback(void) {
}

void void __attribute__((vector(_TIMER_9_VECTOR), interrupt(ipltmr9AUTO), nomips16)) _timer9_interrupt(void) {
    IFSbits_TIF(9, 1) = 0;
    timer_9_callback();
}
#endif


void interrupt_init(void) {
    // enable interrupts

    INTCONbits.MVEC = 1; // this enables the multi Vectored interrupt to tell the microprocessor
    // to allow different handlers for each different type of interrupt (timer2, timer 3)

    asm volatile ("ei");
}

uint32_t interrupt_tick_get(void) {
    return global_tick;
}