#include <stdint.h>
#include <stdio.h>
#include <xc.h>

#include "pic32_config.h"
#include "timer.h"
#include "sysclk.h"

#define TCON(x) T##x##CON
#define TCONbits_TCKPS(x) T##x##CONbits.TCKPS
#define TCONbits_ON(x) T##x##CONbits.ON
#define IECbits_TIE(x, y) IEC##y##bits.T##x##IE
#define IFSbits_TIF(x, y) IFS##y##bits.T##x##IF
#define IPCbits_TIP(x, y) IPC##y##bits.T##x##IP
#define IPCbits_TIS(x, y) IPC##y##bits.T##x##IS
#define PR(x) PR##x
#define TMR(x) TMR##x

static uint16_t tmr1_prescale_value[] = {1, 8, 64, 256};

#if (TIMER_2_ENABLED == 1) || (TIMER_3_ENABLED == 1) || \
    (TIMER_4_ENABLED == 1) || (TIMER_5_ENABLED == 1) || \
    (TIMER_6_ENABLED == 1) || (TIMER_7_ENABLED == 1) || \
    (TIMER_8_ENABLED == 1) || (TIMER_9_ENABLED == 1)
static uint16_t tmrs_prescale_value[] = {1, 2, 4, 8, 16, 32, 64, 256};
#endif

void init_timer1(uint32_t freq, timer1_prescaler_t prescaler, uint8_t sub_periority) {
    uint32_t tmr_clk = sysclk_timerfreq_get();

    TCON(1) = 0x0;
    TMR(1) = 0;
    TCONbits_TCKPS(1) = prescaler;

    IECbits_TIE(1, 0) = 0;

    PR(1) = tmr_clk / freq / tmr1_prescale_value[prescaler];

    IFSbits_TIF(1, 0) = 0;
    IPCbits_TIP(1, 1) = TIMER1_INTERRUPT_PERIORITY;
    IPCbits_TIS(1, 1) = sub_periority;

    IECbits_TIE(1, 0) = 1;


    TCONbits_ON(1) = 1;
}

#if (TIMER_2_ENABLED == 1)
void init_timer2(uint32_t freq, timers_prescaler_t prescaler, uint8_t sub_periority) {
    uint32_t tmr_clk = sysclk_timerfreq_get();

    TCON(2) = 0x0;
    TMR(2) = 0;
    TCONbits_TCKPS(2) = prescaler;

    IECbits_TIE(2, 0) = 0;

    PR(2) = tmr_clk / freq / tmrs_prescale_value[prescaler];

    IFSbits_TIF(2, 0) = 0;
    IPCbits_TIP(2, 2) = TIMER2_INTERRUPT_PERIORITY;
    IPCbits_TIS(2, 2) = sub_periority;

    IECbits_TIE(2, 0) = 1;


    TCONbits_ON(2) = 1;
}
#endif

#if (TIMER_3_ENABLED == 1)
void init_timer3(uint32_t freq, timers_prescaler_t prescaler, uint8_t sub_periority) {
    uint32_t tmr_clk = sysclk_timerfreq_get();

    TCON(3) = 0x0;
    TMR(3) = 0;
    TCONbits_TCKPS(3) = prescaler;

    IECbits_TIE(3, 0) = 0;

    PR(3) = tmr_clk / freq / tmrs_prescale_value[prescaler];

    IFSbits_TIF(3, 0) = 0;
    IPCbits_TIP(3, 3) = TIMER3_INTERRUPT_PERIORITY;
    IPCbits_TIS(3, 3) = sub_periority;

    IECbits_TIE(3, 0) = 1;


    TCONbits_ON(3) = 1;
}
#endif

#if (TIMER_4_ENABLED == 1)
void init_timer4(uint32_t freq, timers_prescaler_t prescaler, uint8_t sub_periority) {
    uint32_t tmr_clk = sysclk_timerfreq_get();

    TCON(4) = 0x0;
    TMR(4) = 0;
    TCONbits_TCKPS(4) = prescaler;

    IECbits_TIE(4, 0) = 0;

    PR(4) = tmr_clk / freq / tmrs_prescale_value[prescaler];

    IFSbits_TIF(4, 0) = 0;
    IPCbits_TIP(4, 4) = TIMER4_INTERRUPT_PERIORITY;
    IPCbits_TIS(4, 4) = sub_periority;

    IECbits_TIE(4, 0) = 1;


    TCONbits_ON(4) = 1;
}
#endif

#if (TIMER_5_ENABLED == 1)
void init_timer5(uint32_t freq, timers_prescaler_t prescaler, uint8_t sub_periority) {
    uint32_t tmr_clk = sysclk_timerfreq_get();

    TCON(5) = 0x0;
    TMR(5) = 0;
    TCONbits_TCKPS(5) = prescaler;

    IECbits_TIE(5, 0) = 0;

    PR(5) = tmr_clk / freq / tmrs_prescale_value[prescaler];

    IFSbits_TIF(5, 0) = 0;
    IPCbits_TIP(5, 6) = TIMER5_INTERRUPT_PERIORITY;
    IPCbits_TIS(5, 6) = sub_periority;

    IECbits_TIE(5, 0) = 1;


    TCONbits_ON(5) = 1;
}
#endif

#if (TIMER_6_ENABLED == 1)
void init_timer6(uint32_t freq, timers_prescaler_t prescaler, uint8_t sub_periority) {
    uint32_t tmr_clk = sysclk_timerfreq_get();

    TCON(6) = 0x0;
    TMR(6) = 0;
    TCONbits_TCKPS(6) = prescaler;

    IECbits_TIE(6, 0) = 0;

    PR(6) = tmr_clk / freq / tmrs_prescale_value[prescaler];

    IFSbits_TIF(6, 0) = 0;
    IPCbits_TIP(6, 7) = TIMER6_INTERRUPT_PERIORITY;
    IPCbits_TIS(6, 7) = sub_periority;

    IECbits_TIE(6, 0) = 1;


    TCONbits_ON(6) = 1;
}
#endif

#if (TIMER_7_ENABLED == 1)
void init_timer7(uint32_t freq, timers_prescaler_t prescaler, uint8_t sub_periority) {
    uint32_t tmr_clk = sysclk_timerfreq_get();

    TCON(7) = 0x0;
    TMR(7) = 0;
    TCONbits_TCKPS(7) = prescaler;

    IECbits_TIE(7, 1) = 0;

    PR(7) = tmr_clk / freq / tmrs_prescale_value[prescaler];

    IFSbits_TIF(7, 1) = 0;
    IPCbits_TIP(7, 8) = TIMER7_INTERRUPT_PERIORITY;
    IPCbits_TIS(7, 8) = sub_periority;

    IECbits_TIE(7, 1) = 1;


    TCONbits_ON(7) = 1;
}
#endif

#if (TIMER_8_ENABLED == 1)
void init_timer8(uint32_t freq, timers_prescaler_t prescaler, uint8_t sub_periority) {
    uint32_t tmr_clk = sysclk_timerfreq_get();

    TCON(8) = 0x0;
    TMR(8) = 0;
    TCONbits_TCKPS(8) = prescaler;

    IECbits_TIE(8, 1) = 0;

    PR(8) = tmr_clk / freq / tmrs_prescale_value[prescaler];

    IFSbits_TIF(8, 1) = 0;
    IPCbits_TIP(8, 9) = TIMER8_INTERRUPT_PERIORITY;
    IPCbits_TIS(8, 9) = sub_periority;

    IECbits_TIE(8, 1) = 1;


    TCONbits_ON(8) = 1;
}
#endif

#if (TIMER_9_ENABLED == 1)
void init_timer9(uint32_t freq, timers_prescaler_t prescaler, uint8_t sub_periority) {
    uint32_t tmr_clk = sysclk_timerfreq_get();

    TCON(9) = 0x0;
    TMR(9) = 0;
    TCONbits_TCKPS(9) = prescaler;

    IECbits_TIE(9, 1) = 0;

    PR(9) = tmr_clk / freq / tmrs_prescale_value[prescaler];

    IFSbits_TIF(9, 1) = 0;
    IPCbits_TIP(9, 10) = TIMER9_INTERRUPT_PERIORITY;
    IPCbits_TIS(9, 10) = sub_periority;

    IECbits_TIE(9, 1) = 1;


    TCONbits_ON(9) = 1;
}
#endif