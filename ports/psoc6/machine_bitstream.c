#include "py/mpconfig.h"
#include "py/mphal.h"
#include "FreeRTOS.h"
//#include "atomic.h"
#include "task.h"

//#include "portmacro.h"
//#include "modmachine.c"

//#include "cypdl.h"

#if MICROPY_PY_MACHINE_BITSTREAM

volatile bool gpio_intr_flag = false;
cyhal_gpio_callback_data_t gpio_btn_callback_data;
static void gpio_interrupt_handler(void *handler_arg, cyhal_gpio_event_t event);
#define GPIO_INTERRUPT_PRIORITY (7u)

#define NS_TICKS_OVERHEAD (0)

// #define FAST_CLOCK_HZ    150000000u
// #define SLOW_CLOCK_HZ    75000000u
//#define PLL_CLOCK_HZ     150000000u

void machine_bitstream_high_low(mp_hal_pin_obj_t pin, uint32_t *timing_ns, const uint8_t *buf, size_t len) {
    
//     cyhal_clock_t clock_fast;
//     cyhal_clock_t clock_slow;
//     cyhal_clock_t clock_pll;
//     cyhal_clock_t clock_hf0;
//     cyhal_clock_t clock_peri;
//    // cy_rslt_t result;


//     //PLL
//     /* Initialize, take ownership of PLL0/PLL */
//     //result = 
//     cyhal_clock_reserve(&clock_pll, &CYHAL_CLOCK_PLL[0]);

//     /* Set the PLL0/PLL frequency to PLL_CLOCK_HZ */
//     //result = 
//     cyhal_clock_set_frequency(&clock_pll,  PLL_CLOCK_HZ, NULL);

//     /* If the PLL0/PLL clock is not already enabled, enable it */
//     if (!cyhal_clock_is_enabled(&clock_pll)) {
//         //result = 
//         cyhal_clock_set_enabled(&clock_pll, true, true);
//     }
//     //PLL

//     //HF0
//     /* Initialize, take ownership of CLK_HF0 */
//     //result = 
//     cyhal_clock_reserve(&clock_hf0, &CYHAL_CLOCK_HF[0]);


//     /* Source the (CLK_HF0) from PLL0/PLL */
//     //result = 
//     cyhal_clock_set_source(&clock_hf0, &clock_pll);

//     /* Set the divider for (CLK_HF0) */
//     //result = 
//     cyhal_clock_set_divider(&clock_hf0, 1);
 

//     /* If the audio subsystem clock (CLK_HF0) is not already enabled, enable it */
//     if (!cyhal_clock_is_enabled(&clock_hf0)) {
//         //result = 
//         cyhal_clock_set_enabled(&clock_hf0, true, true);
//     }
//     //HF0
    
//     //Fast clock
//     //result =
//      cyhal_clock_reserve(&clock_fast, &CYHAL_CLOCK_FAST);
    
//     //result = 
//     cyhal_clock_set_divider(&clock_fast, 1);


//     if (!cyhal_clock_is_enabled(&clock_fast)) {
//         //result = 
//         cyhal_clock_set_enabled(&clock_fast, true, true);
//     }
//     //Fast clock

//     //Peri clock
//    // result = 
//     cyhal_clock_reserve(&clock_peri, &CYHAL_CLOCK_PERI);

    
//     //result =
//      cyhal_clock_set_divider(&clock_peri, 2);


//     if (!cyhal_clock_is_enabled(&clock_peri)) {
//       //  result = 
//         cyhal_clock_set_enabled(&clock_peri, true, true);
//     }
//     //peri clock


//     //slow        
//     //result = 
//     cyhal_clock_reserve(&clock_slow, &CYHAL_CLOCK_SLOW);

//     //result = 
//     cyhal_clock_set_divider(&clock_slow, 1);


//     if (!cyhal_clock_is_enabled(&clock_slow)) {
//       //  result = 
//         cyhal_clock_set_enabled(&clock_slow, true, true);
//     }
//   //  slow


    uint32_t fcpu_mhz =  mp_hal_get_cpu_freq() / 1000000;

    mp_printf(&mp_plat_print, "freq%lu", fcpu_mhz);

    // Convert ns to cycles [high_time_0, low_time_0, high_time_1, low_time_1].
    for (size_t i = 0; i < 4; ++i) {
        timing_ns[i] = fcpu_mhz * timing_ns[i] / 1000;
        // if (timing_ns[i] > NS_TICKS_OVERHEAD) {
        //     timing_ns[i] -= NS_TICKS_OVERHEAD;
        // }
        // if (i % 2 == 1) {
        //     // Convert low_time to period (i.e. add high_time).
        //     timing_ns[i] += timing_ns[i - 1];
        // }
    }

  // uint32_t irq_state = 
 // portSET_INTERRUPT_MASK_FROM_ISR();
  taskENTER_CRITICAL() ;
  //uint32_t irq_state = cyhal_system_critical_section_enter();
 //  mp_hal_quiet_timing_enter();

    cyhal_gpio_init(P0_4, CYHAL_GPIO_DIR_INPUT,
                    CYHAL_GPIO_DRIVE_PULLUP, 1);
    /* User button init failed. Stop program execution */

    /* Configure GPIO interrupt */
    gpio_btn_callback_data.callback = gpio_interrupt_handler;
    cyhal_gpio_register_callback(P0_4,
                                 &gpio_btn_callback_data);
    cyhal_gpio_enable_event(P0_4, CYHAL_GPIO_IRQ_FALL,
                                 GPIO_INTERRUPT_PRIORITY, true);

   for (size_t i = 0; i < len; ++i) {
       //uint8_t b = buf[i];
      // for (size_t j = 0; j < 8; ++j) {
           mp_hal_pin_od_high(pin);
         //  uint32_t *t = &timing_ns[b >> 6 & 2];
           //cyhal_gpio_write(P13_6, true);
         //  Cy_SysLib_DelayCycles(t[0]);
           mp_hal_pin_od_low(pin);
          //cyhal_gpio_write(P13_6, false);
          //cyhal_gpio_toggle (P13_6);
         // Cy_SysLib_DelayCycles(t[1]);
         // b <<= 1;
    //}
   }
//mp_hal_quiet_timing_exit(irq_state);
 //cyhal_system_critical_section_exit(irq_state);
//portCLEAR_INTERRUPT_MASK_FROM_ISR(irq_state);
taskEXIT_CRITICAL ();

}

static void gpio_interrupt_handler(void *handler_arg, cyhal_gpio_event_t event)
{
    cyhal_gpio_init(P13_7, CYHAL_GPIO_DIR_OUTPUT,
                    CYHAL_GPIO_DRIVE_STRONG, 0);

            cyhal_gpio_write(P13_7, 0);
            cyhal_system_delay_ms(10);
            cyhal_gpio_write(P13_7, 1);
                cyhal_system_delay_ms(10);
}


#endif // MICROPY_PY_MACHINE_BITSTREAM
