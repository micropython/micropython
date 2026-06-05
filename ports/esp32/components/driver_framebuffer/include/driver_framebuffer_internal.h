#pragma once

#include "sdkconfig.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_system.h"

#include "driver_framebuffer_compositor.h"
#include "driver_framebuffer_font.h"
#include "driver_framebuffer_devices.h"
#include "driver_framebuffer_dirty.h"
#include "driver_framebuffer_orientation.h"
#include "driver_framebuffer_drawing.h"
#include "driver_framebuffer_text.h"
#include "driver_framebuffer.h"
#include "driver_framebuffer_matrix.h"

//PNG library
#include "mem_reader.h"
#include "file_reader.h"
#include "png_reader.h"
