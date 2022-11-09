
#include "usbd.h"
#include "string.h"
#include "pico/unique_id.h"

int usbd_serialnumber(uint8_t *buf) {
    pico_unique_board_id_t id;
    const int len = 8;

    pico_get_unique_board_id(&id);
    memcpy(buf, id.id, len);

    return len;
}
