#include "irq.h"

/** @brief The BCM2835 Interupt controller peripheral at it's base address */
static struct rpi_irq_controller_t* rpiIRQController = (struct rpi_irq_controller_t*)IRQ_BASE;

volatile int b = 0;

/**
    @brief Return the IRQ Controller register set
*/
struct rpi_irq_controller_t* RPI_GetIrqController( void )
{
    return rpiIRQController;
}

void enable_timer_irq ()
{
    RPI_GetIrqController()->Enable_Basic_IRQs = RPI_BASIC_ARM_TIMER_IRQ;
}

static rpi_arm_timer_t* rpiArmTimer = (rpi_arm_timer_t*)RPI_ARMTIMER_BASE;

rpi_arm_timer_t* RPI_GetArmTimer(void)
{
    return rpiArmTimer;
}

void __attribute__((interrupt("UNDEF"))) undef_vector(void)
{
    printf ("UNDEF :(\n");
    while( 1 )
    {
        /* Do Nothing! */
    }
}
void __attribute__((interrupt("IRQ"))) irq_vector(void)
{
    //void* lr, *sp;
    /*asm volatile ( "mov %0, lr\n"
                   "mov %1, sp\n"
                   : "=r" (lr), "=r" ( sp ) );*/
    //printf (" -> ! IRQ :( \n");
    //b = 1;
    rpiArmTimer->IRQClear = 1;
    //rpiArmTimer->Load = 0x4000;
    //printf ("SP: 0x%08x\nLR: 0x%08x\n", sp, lr);
 
    /**while( 1 )
    {
    }*/
}
void __attribute__((interrupt("FIQ"))) fiq_vector(void)
{
    printf ("FIQ :( \n");
    while( 1 )
    {
        /* Do Nothing! */
    }
}
void __attribute__((interrupt("SWI"))) swi_vector(void)
{
    void* lr, *sp;
    asm volatile ( "mov %0, lr\n"
                   "mov %1, sp\n"
                   : "=r" (lr), "=r" ( sp ) );
    printf (" -> ! SWI :( \n");
    printf ("SP: 0x%08x\nLR: 0x%08x\n", sp, lr);
    while( 1 )
    {
        /* Do Nothing! */
    }
}
void __attribute__((interrupt("ABORT"))) abort_vector(void)
{
    printf ("ABORT :(\n");
    while( 1 )
    {
        /* Do Nothing! */
    }
}

void timer_init () 
{
    /** TIMER INIT **/
    /* Setup the system timer interrupt */
    /* Timer frequency = Clk/256 * 0x400 */
    RPI_GetArmTimer()->Load = 0x4000;

    /* Setup the ARM Timer */
    RPI_GetArmTimer()->Control =
            RPI_ARMTIMER_CTRL_23BIT |
            RPI_ARMTIMER_CTRL_ENABLE |
            RPI_ARMTIMER_CTRL_INT_ENABLE |
            RPI_ARMTIMER_CTRL_PRESCALE_256;
}
