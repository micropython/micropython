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

#include "py/runtime.h"
#include "supervisor/usb.h"
#include "supervisor/esp_port.h"
#include "lib/utils/interrupt_char.h"
#include "lib/mp-readline/readline.h"

#include "components/driver/include/driver/gpio.h"
#include "components/driver/include/driver/periph_ctrl.h"
#include "components/esp_rom/include/esp32s2/rom/gpio.h"
#include "components/hal/esp32s2/include/hal/gpio_ll.h"
#include "components/soc/esp32s2/include/soc/usb_periph.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "hal/usb_hal.h"

#include "tusb.h"

#ifdef CFG_TUSB_DEBUG
  #define USBD_STACK_SIZE     (3 * configMINIMAL_STACK_SIZE)
#else
  #define USBD_STACK_SIZE     (3 * configMINIMAL_STACK_SIZE / 2)
#endif

StackType_t usb_device_stack[USBD_STACK_SIZE];
StaticTask_t usb_device_taskdef;

// USB Device Driver task
// This top level thread process all usb events and invoke callbacks
void usb_device_task(void *param) {
    (void)param;

    // RTOS forever loop
    while (1) {
        // tinyusb device task
        if (tusb_inited()) {
            tud_task();
            tud_cdc_write_flush();
        }
        vTaskDelay(1);
    }
}

static void configure_pins(usb_hal_context_t *usb) {
    /* usb_periph_iopins currently configures USB_OTG as USB Device.
     * Introduce additional parameters in usb_hal_context_t when adding support
     * for USB Host.
     */
    for (const usb_iopin_dsc_t *iopin = usb_periph_iopins; iopin->pin != -1; ++iopin) {
        if ((usb->use_external_phy) || (iopin->ext_phy_only == 0)) {
            gpio_pad_select_gpio(iopin->pin);
            if (iopin->is_output) {
                gpio_matrix_out(iopin->pin, iopin->func, false, false);
            } else {
                gpio_matrix_in(iopin->pin, iopin->func, false);
                gpio_pad_input_enable(iopin->pin);
            }
            gpio_pad_unhold(iopin->pin);
        }
    }
    if (!usb->use_external_phy) {
        gpio_set_drive_capability(USBPHY_DP_NUM, GPIO_DRIVE_CAP_3);
        gpio_set_drive_capability(USBPHY_DP_NUM, GPIO_DRIVE_CAP_3);
    }
}

void init_usb_hardware(void) {
    periph_module_reset(PERIPH_USB_MODULE);
    periph_module_enable(PERIPH_USB_MODULE);
    usb_hal_context_t hal = {
        .use_external_phy = false // use built-in PHY
    };
    usb_hal_init(&hal);
    configure_pins(&hal);

    (void)xTaskCreateStatic(usb_device_task,
        "usbd",
        USBD_STACK_SIZE,
        NULL,
        5,
        usb_device_stack,
        &usb_device_taskdef);
}
/**
 * Callback invoked when received an "wanted" char.
 * @param itf           Interface index (for multiple cdc interfaces)
 * @param wanted_char   The wanted char (set previously)
 */
void tud_cdc_rx_wanted_cb(uint8_t itf, char wanted_char) {
    (void)itf;  // not used
    // Workaround for using lib/utils/interrupt_char.c
    // Compare mp_interrupt_char with wanted_char and ignore if not matched
    if (mp_interrupt_char == wanted_char) {
        tud_cdc_read_flush();    // flush read fifo
        mp_sched_keyboard_interrupt();
        // CircuitPython's VM is run in a separate FreeRTOS task from TinyUSB.
        // So, we must notify the other task when a CTRL-C is received.
        xTaskNotifyGive(circuitpython_task);
    }
}
