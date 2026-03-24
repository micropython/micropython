#include "esp_log.h"
#include "py/obj.h"
#include "driver/gpio.h"
#include "py/runtime.h"
#include "py/obj.h"
#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"

extern mp_obj_t wl_blockdev_make_new(void);

void mch2022_board_startup(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << 19,
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&io_conf);

    gpio_set_level(19, 1);
}
