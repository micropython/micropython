/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 hathach for Adafruit Industries
 * Copyright (c) 2019 Lucian Copeland for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "supervisor/usb.h"
#include "lib/utils/interrupt_char.h"
#include "lib/mp-readline/readline.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "hal/usb_hal.h"

#include "tusb.h"

#ifdef CFG_TUSB_DEBUG
  #define USBD_STACK_SIZE     (3*configMINIMAL_STACK_SIZE)
#else
  #define USBD_STACK_SIZE     (3*configMINIMAL_STACK_SIZE/2)
#endif

StackType_t  usb_device_stack[USBD_STACK_SIZE];
StaticTask_t usb_device_taskdef;

// USB Device Driver task
// This top level thread process all usb events and invoke callbacks
void usb_device_task(void* param)
{
  (void) param;

  // RTOS forever loop
  while (1)
  {
    // tinyusb device task
    if (tusb_inited()) {
        tud_task();
        tud_cdc_write_flush();
    }
    vTaskDelay(10);
  }
}

void init_usb_hardware(void) {
    usb_hal_context_t hal = {
        .use_external_phy = false // use built-in PHY
    };
    usb_hal_init(&hal);

    (void) xTaskCreateStatic(usb_device_task,
                             "usbd",
                             USBD_STACK_SIZE,
                             NULL,
                             configMAX_PRIORITIES-1,
                             usb_device_stack,
                             &usb_device_taskdef);
}

