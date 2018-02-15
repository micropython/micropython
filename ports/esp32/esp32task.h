#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void mp_task(void *);
TaskHandle_t micropython_task(int);
void nlr_jump_fail(void *);
void mbedtls_debug_set_threshold(int);
