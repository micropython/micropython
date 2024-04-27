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
#include "supervisor/port.h"
#include "shared/runtime/interrupt_char.h"
#include "shared/readline/readline.h"

#include "hal/gpio_ll.h"

#include "esp_err.h"
#include "esp_private/usb_phy.h"
#include "soc/usb_periph.h"

#include "driver/gpio.h"
#include "esp_private/periph_ctrl.h"

#ifdef CONFIG_IDF_TARGET_ESP32C3
#include "components/esp_rom/include/esp32c3/rom/gpio.h"
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
#include "components/esp_rom/include/esp32s2/rom/gpio.h"
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
#include "components/esp_rom/include/esp32s3/rom/gpio.h"
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "tusb.h"

#if CIRCUITPY_USB_DEVICE
#ifdef CFG_TUSB_DEBUG
  #define USBD_STACK_SIZE     (3 * configMINIMAL_STACK_SIZE)
#else
  #define USBD_STACK_SIZE     (3 * configMINIMAL_STACK_SIZE / 2)
#endif

StackType_t usb_device_stack[USBD_STACK_SIZE];
StaticTask_t usb_device_taskdef;

static usb_phy_handle_t phy_hdl;

// USB Device Driver task
// This top level thread process all usb events and invoke callbacks
STATIC void usb_device_task(void *param) {
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

/**
 * Callback invoked when received an "wanted" char.
 * @param itf           Interface index (for multiple cdc interfaces)
 * @param wanted_char   The wanted char (set previously)
 */
void tud_cdc_rx_wanted_cb(uint8_t itf, char wanted_char) {
    (void)itf;  // not used
    // CircuitPython's VM is run in a separate FreeRTOS task from TinyUSB.
    // So, we must notify the other task when a CTRL-C is received.
    port_wake_main_task();
    // Workaround for using shared/runtime/interrupt_char.c
    // Compare mp_interrupt_char with wanted_char and ignore if not matched
    if (mp_interrupt_char == wanted_char) {
        tud_cdc_read_flush();    // flush read fifo
        mp_sched_keyboard_interrupt();
    }
}

void tud_cdc_rx_cb(uint8_t itf) {
    (void)itf;
    // Workaround for "press any key to enter REPL" response being delayed on espressif.
    // Wake main task when any key is pressed.
    port_wake_main_task();
}
#endif // CIRCUITPY_USB_DEVICE

void init_usb_hardware(void) {
    #if CIRCUITPY_USB_DEVICE
    // Configure USB PHY
    usb_phy_config_t phy_conf = {
        .controller = USB_PHY_CTRL_OTG,
        .otg_mode = USB_OTG_MODE_DEVICE,
    };
    usb_new_phy(&phy_conf, &phy_hdl);

    // Pin the USB task to the same core as CircuitPython. This way we leave
    // the other core for networking.
    (void)xTaskCreateStaticPinnedToCore(usb_device_task,
        "usbd",
        USBD_STACK_SIZE,
        NULL,
        5,
        usb_device_stack,
        &usb_device_taskdef,
        xPortGetCoreID());
    #endif
}
