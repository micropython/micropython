/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MBED_SLEEP_EX_API_H
#define MBED_SLEEP_EX_API_H

#include "device.h"

#if DEVICE_SLEEP

#ifdef __cplusplus
extern "C" {
#endif

/* Sleep Eake Up event, the User application also 
   need to config the peripheral to trigger wake up event */
#define SLEEP_WAKEUP_BY_STIMER            (SLP_STIMER)    // wake up by system timer
#define SLEEP_WAKEUP_BY_GTIMER            (SLP_GTIMER)    // wake up by General purpose timer timeout
#define SLEEP_WAKEUP_BY_GPIO_INT          (SLP_GPIO)      // wake up by GPIO Port A[7:0] Interrupt
#define SLEEP_WAKEUP_BY_WLAN              (SLP_WL)        // wake up by WLan event
#define SLEEP_WAKEUP_BY_NFC               (SLP_NFC)       // wake up by NFC event
#define SLEEP_WAKEUP_BY_SDIO              (SLP_SDIO)      // wake up by SDIO event
#define SLEEP_WAKEUP_BY_USB               (SLP_USB)       // wake up by USB event

// Deep Standby Wakeup event
#define STANDBY_WAKEUP_BY_STIMER      (BIT0)     // wake up by system timer   
#define STANDBY_WAKEUP_BY_NFC         (BIT1)     // wake up by NFC event
//#define SLEEP_WAKEUP_BY_DS_TIMER    (BIT2)     // The timer to wakeup from Deep Sleep timer
// Do not modify these definition, or need to modify the code also.
#define STANDBY_WAKEUP_BY_PA5         (BIT4)     // GPIO Port A[5] 
#define STANDBY_WAKEUP_BY_PC7         (BIT5)     // GPIO Port C[7] 
#define STANDBY_WAKEUP_BY_PD5         (BIT6)     // GPIO Port D[5] 
#define STANDBY_WAKEUP_BY_PE3         (BIT7)     // GPIO Port E[3] 

// Deep Sleep Wakeup event
#define DSLEEP_WAKEUP_BY_TIMER     (DS_TIMER33)
#define DSLEEP_WAKEUP_BY_GPIO      (DS_GPIO)    // GPIO Port B[1] 

typedef struct _SLEEP_WKUP_EVENT_ {
    u8 wakeup_event;    // Wake up event: Timer, NFC, GPIO
    u8 gpio_option;     // GPIO Wakeup setting: [3:0]: Pin 3~0 enable, [7:4]: pin3~0 active high/low
    u32 timer_duration; // the sleep duration and then wakeup
} SLEEP_WAKEUP_EVENT, *PSLEEP_WAKEUP_EVENT;
/** Send the microcontroller to sleep
 *
 * The processor is setup ready for sleep, and sent to sleep using __WFI(). In this mode, the
 * system clock to the core is stopped until a reset or an interrupt occurs. This eliminates
 * dynamic power used by the processor, memory systems and buses. The processor, peripheral and
 * memory state are maintained, and the peripherals continue to work and can generate interrupts.
 *
 * The processor can be woken up by any internal peripheral interrupt or external pin interrupt.
 *
 * @note
 *  The mbed interface semihosting is disconnected as part of going to sleep, and can not be restored.
 * Flash re-programming and the USB serial port will remain active, but the mbed program will no longer be
 * able to access the LocalFileSystem
 */
void sleep_ex(uint32_t wakeup_event,  uint32_t sleep_duration);
void sleep_ex_selective(uint32_t wakeup_event,  uint32_t sleep_duration, uint32_t clk_sourec_enable, uint32_t sdr_enable);

void standby_wakeup_event_add(uint32_t wakeup_event, uint32_t sleep_duration_ms, uint32_t gpio_active);
void standby_wakeup_event_del(uint32_t wakeup_event);
void deepstandby_ex(void);

/** Send the microcontroller to deep sleep
 *
 * This processor is setup ready for deep sleep, and sent to sleep using __WFI(). This mode
 * has the same sleep features as sleep plus it powers down peripherals and clocks. All state
 * is still maintained.
 *
 * The processor can only be woken up by an external interrupt on a pin or a timer.
 *
 * @note
 *  The mbed interface semihosting is disconnected as part of going to sleep, and can not be restored.
 * Flash re-programming and the USB serial port will remain active, but the mbed program will no longer be
 * able to access the LocalFileSystem
 */
void deepsleep_ex(uint32_t wakeup_event, uint32_t sleep_duration);

#ifdef __cplusplus
}
#endif

#endif

#endif
