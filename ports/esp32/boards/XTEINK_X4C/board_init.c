#include "py/mpconfig.h"

#include "esp_attr.h"
#include "esp_private/startup_internal.h"
#include "hal/gpio_ll.h"
#include "hal/rtc_io_ll.h"
#include "soc/gpio_struct.h"
#include "soc/io_mux_reg.h"

#define XTEINK_X4C_POWER_LATCH (1)

ESP_SYSTEM_INIT_FN(xteink_x4c_power_latch, CORE, BIT(0), 0, IRAM_ATTR) {
    rtcio_ll_force_hold_disable(XTEINK_X4C_POWER_LATCH);
    gpio_ll_func_sel(&GPIO, XTEINK_X4C_POWER_LATCH, FUNC_GPIO1_GPIO1);
    gpio_ll_matrix_out_default(&GPIO, XTEINK_X4C_POWER_LATCH);
    gpio_ll_set_level(&GPIO, XTEINK_X4C_POWER_LATCH, 1);
    gpio_ll_output_enable(&GPIO, XTEINK_X4C_POWER_LATCH);
    return ESP_OK;
}

void xteink_x4c_board_startup(void) {
    gpio_ll_set_level(&GPIO, XTEINK_X4C_POWER_LATCH, 1);
    boardctrl_startup();
}
