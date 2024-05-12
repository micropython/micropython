/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Scott Shawcroft for Adafruit Industries
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

#include "shared-module/max3421e/Max3421E.h"

#include "bindings/espidf/__init__.h"

#include "esp_heap_caps.h"
#include "hal/gpio_types.h"
#include "lib/tinyusb/src/host/usbh.h"
#include "py/runtime.h"
#include "shared-bindings/busio/SPI.h"
#include "supervisor/usb.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "components/driver/gpio/include/driver/gpio.h"

#ifdef CFG_TUSB_DEBUG
  #define USBH_STACK_SIZE     (3 * configMINIMAL_STACK_SIZE)
#else
  #define USBH_STACK_SIZE     (3 * configMINIMAL_STACK_SIZE / 2)
#endif

// Setup a separate FreeRTOS task for host because the device task blocks while
// waiting for more device tasks. The stack is allocated on first use when the
// task is started.
TaskHandle_t usb_host_task_handle;

STATIC void usb_host_task(void *param) {
    (void)param;
    // RTOS forever loop
    while (1) {
        tuh_task();
        vTaskDelay(1);
    }
}

static void _interrupt_wrapper(void *arg) {
    max3421e_interrupt_handler((max3421e_max3421e_obj_t *)arg);
}

// Setup irq on self->irq to call tuh_int_handler(rhport, true) on falling edge
void common_hal_max3421e_max3421e_init_irq(max3421e_max3421e_obj_t *self) {

    // Pin the USB task to the same core as CircuitPython. This way we leave
    // the other core for networking.
    BaseType_t base_type = xTaskCreatePinnedToCore(usb_host_task,
        "usbh",
        USBH_STACK_SIZE,
        NULL,
        5,
        &usb_host_task_handle,
        xPortGetCoreID());

    if (base_type != pdPASS) {
        if (base_type == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
            mp_raise_espidf_MemoryError();
        } else {
            mp_raise_RuntimeError_varg(MP_ERROR_TEXT("Unknown error code %d"), base_type);
        }
    }

    const mcu_pin_obj_t *pin = self->irq.pin;
    esp_err_t result = gpio_install_isr_service(ESP_INTR_FLAG_SHARED);
    if (result != ESP_OK) {
        vTaskDelete(usb_host_task_handle);
        CHECK_ESP_RESULT(result);
    }
    result = gpio_isr_handler_add(pin->number, _interrupt_wrapper, self);
    if (result != ESP_OK) {
        vTaskDelete(usb_host_task_handle);
        CHECK_ESP_RESULT(result);
    }
    gpio_set_intr_type(pin->number, GPIO_INTR_LOW_LEVEL);
    gpio_intr_enable(pin->number);
}

void common_hal_max3421e_max3421e_deinit_irq(max3421e_max3421e_obj_t *self) {
    const mcu_pin_obj_t *pin = self->irq.pin;
    gpio_isr_handler_remove(pin->number);
    gpio_uninstall_isr_service();

    vTaskDelete(usb_host_task_handle);
}

// Enable or disable the irq interrupt.
void common_hal_max3421e_max3421e_irq_enabled(max3421e_max3421e_obj_t *self, bool enabled) {
    const mcu_pin_obj_t *pin = self->irq.pin;
    if (!enabled) {
        gpio_intr_disable(pin->number);
    } else {
        gpio_intr_enable(pin->number);
    }
}
