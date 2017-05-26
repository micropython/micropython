/**
 * \file irq.h
 * see http://www.valvers.com/open-software/raspberry-pi/step04-bare-metal-programming-in-c-pt4/
 **/

#ifndef RPI_IRQ
#define RPI_IRQ

#include "platform.h"
#include <stdint.h>
#include <stdio.h>

#define IRQ_BASE ( PBASE + 0xB200 ) 

struct rpi_irq_controller_t 
{
    volatile uint32_t IRQ_basic_pending;
    volatile uint32_t IRQ_pending_1;
    volatile uint32_t IRQ_pending_2;
    volatile uint32_t FIQ_control;
    volatile uint32_t Enable_IRQs_1;
    volatile uint32_t Enable_IRQs_2;
    volatile uint32_t Enable_Basic_IRQs;
    volatile uint32_t Disable_IRQs_1;
    volatile uint32_t Disable_IRQs_2;
    volatile uint32_t Disable_Basic_IRQs;
};

    /** @brief Bits in the Enable_Basic_IRQs register to enable various interrupts.
        See the BCM2835 ARM Peripherals manual, section 7.5 */
    #define RPI_BASIC_ARM_TIMER_IRQ         (1 << 0)
    #define RPI_BASIC_ARM_MAILBOX_IRQ       (1 << 1)
    #define RPI_BASIC_ARM_DOORBELL_0_IRQ    (1 << 2)
    #define RPI_BASIC_ARM_DOORBELL_1_IRQ    (1 << 3)
    #define RPI_BASIC_GPU_0_HALTED_IRQ      (1 << 4)
    #define RPI_BASIC_GPU_1_HALTED_IRQ      (1 << 5)
    #define RPI_BASIC_ACCESS_ERROR_1_IRQ    (1 << 6)
    #define RPI_BASIC_ACCESS_ERROR_0_IRQ    (1 << 7)

/**
 * Timer Stuff
 **/
  /** @brief See the documentation for the ARM side timer (Section 14 of the
        BCM2835 Peripherals PDF) */
    #define RPI_ARMTIMER_BASE               ( PBASE + 0xB400 )

    /** @brief 0 : 16-bit counters - 1 : 23-bit counter */
    #define RPI_ARMTIMER_CTRL_23BIT         ( 1 << 1 )

    #define RPI_ARMTIMER_CTRL_PRESCALE_1    ( 0 << 2 )
    #define RPI_ARMTIMER_CTRL_PRESCALE_16   ( 1 << 2 )
    #define RPI_ARMTIMER_CTRL_PRESCALE_256  ( 2 << 2 )

    /** @brief 0 : Timer interrupt disabled - 1 : Timer interrupt enabled */
    #define RPI_ARMTIMER_CTRL_INT_ENABLE    ( 1 << 5 )
    #define RPI_ARMTIMER_CTRL_INT_DISABLE   ( 0 << 5 )

    /** @brief 0 : Timer disabled - 1 : Timer enabled */
    #define RPI_ARMTIMER_CTRL_ENABLE        ( 1 << 7 )
    #define RPI_ARMTIMER_CTRL_DISABLE       ( 0 << 7 )

    /** @brief Section 14.2 of the BCM2835 Peripherals documentation details
        the register layout for the ARM side timer */
    typedef struct {

        /** The timer load register sets the time for the timer to count down.
            This value is loaded into the timer value register after the load
            register has been written or if the timer-value register has counted
            down to 0. */
        volatile uint32_t Load;

        /** This register holds the current timer value and is counted down when
            the counter is running. It is counted down each timer clock until the
            value 0 is reached. Then the value register is re-loaded from the
            timer load register and the interrupt pending bit is set. The timer
            count down speed is set by the timer pre-divide register. */
        volatile uint32_t Value;

        /** The standard SP804 timer control register consist of 8 bits but in the
            BCM implementation there are more control bits for the extra features.
            Control bits 0-7 are identical to the SP804 bits, albeit some
            functionality of the SP804 is not implemented. All new control bits
            start from bit 8 upwards. */
        volatile uint32_t Control;

        /** The timer IRQ clear register is write only. When writing this register
            the interrupt-pending bit is cleared. When reading this register it
            returns 0x544D5241 which is the ASCII reversed value for "ARMT". */
        volatile uint32_t IRQClear;

        /** The raw IRQ register is a read-only register. It shows the status of
            the interrupt pending bit. 0 : The interrupt pending bits is clear.
            1 : The interrupt pending bit is set.

            The interrupt pending bits is set each time the value register is
            counted down to zero. The interrupt pending bit can not by itself
            generates interrupts. Interrupts can only be generated if the
            interrupt enable bit is set. */
        volatile uint32_t RAWIRQ;

        /** The masked IRQ register is a read-only register. It shows the status
            of the interrupt signal. It is simply a logical AND of the interrupt
            pending bit and the interrupt enable bit. 0 : Interrupt line not
            asserted. 1 :Interrupt line is asserted, (the interrupt pending and
            the interrupt enable bit are set.)  */
        volatile uint32_t MaskedIRQ;

        /** This register is a copy of the timer load register. The difference is
            that a write to this register does not trigger an immediate reload of
            the timer value register. Instead the timer load register value is
            only accessed if the value register has finished counting down to
            zero. */
        volatile uint32_t Reload;

        /** The Pre-divider register is not present in the SP804. The pre-divider
            register is 10 bits wide and can be written or read from. This
            register has been added as the SP804 expects a 1MHz clock which we do
            not have. Instead the pre-divider takes the APB clock and divides it
            down according to:

            timer_clock = apb_clock/(pre_divider+1)

            The reset value of this register is 0x7D so gives a divide by 126. */
        volatile uint32_t PreDivider;

        /** The free running counter is not present in the SP804. The free running
            counter is a 32 bits wide read only register. The register is enabled
            by setting bit 9 of the Timer control register. The free running
            counter is incremented immediately after it is enabled. The timer can
            not be reset but when enabled, will always increment and roll-over.

            The free running counter is also running from the APB clock and has
            its own clock pre-divider controlled by bits 16-23 of the timer
            control register.

            This register will be halted too if bit 8 of the control register is
            set and the ARM is in Debug Halt mode. */
        volatile uint32_t FreeRunningCounter;

        } rpi_arm_timer_t;


        void timer_init();
#endif
