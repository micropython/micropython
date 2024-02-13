#include "py/obj.h"

#define CONNECTED_BIT BIT0
#define SMARTCONFIG_DONE_BIT BIT1

static const char *TAG = "smartconfig";

/* task function used for xTaskCreate() */
static void smartconfig_task(void *param);

STATIC mp_obj_t smartconfig_stop(void);
