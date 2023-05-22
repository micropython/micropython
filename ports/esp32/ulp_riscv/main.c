/* ULP-RISC-V example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.

   This code runs on ULP-RISC-V  coprocessor
*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "esp32s3/ulp_riscv.h"
#include "ulp_riscv/ulp_riscv_utils.h"
#include "ulp_riscv/ulp_riscv_gpio.h"

/* this variable will be exported as a public symbol, visible from main CPU: */
bool gpio_level = false;

int main (void)
{
   ulp_riscv_gpio_init(14);
   ulp_riscv_gpio_set_output_mode(14, RTCIO_MODE_OUTPUT);
   ulp_riscv_gpio_output_enable(14);

   while(1) {
        // Turn on GPIO
        ulp_riscv_gpio_output_level(14, 1);
        gpio_level = true;
        // Delay
        ulp_riscv_delay_cycles(1000 * 1000);
        // Turn off GPIO
        ulp_riscv_gpio_output_level(14, 0);
        gpio_level = false;
        // Delay
        ulp_riscv_delay_cycles(1000 * 1000);
   }

    /* ulp_riscv_halt() is called automatically when main exits */
    return 0;
}