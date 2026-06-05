//
// Created by Tom on 16/03/2020. This file is used when the framebuffer device is disabled,
// to prevent lengthy compilation of all fonts and other large source files.
//

#pragma once

#include "sdkconfig.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_system.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t driver_framebuffer_init();
/* Initialize the framebuffer driver (called once at system boot from platform.c) */

#ifdef __cplusplus
}
#endif