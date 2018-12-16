/**
 * @file mouse.h
 *
 */

#ifndef MOUSE_H
#define MOUSE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_drv_conf.h"
#else
#include "lv_drv_conf.h"
#endif

#if USE_MOUSE
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "lv_hal/lv_hal_indev.h"


#ifndef MONITOR_SDL_INCLUDE_PATH
#define MONITOR_SDL_INCLUDE_PATH <SDL2/SDL.h>
#endif

#include MONITOR_SDL_INCLUDE_PATH

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the mouse
 */
void mouse_init(void);
/**
 * Get the current position and state of the mouse
 * @param data store the mouse data here
 * @return false: because the points are not buffered, so no more data to be read
 */
bool mouse_read(lv_indev_data_t * data);

/**
 * It will be called from the main SDL thread
 */
void mouse_handler(SDL_Event *event);

/**********************
 *      MACROS
 **********************/

#endif /* USE_MOUSE */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MOUSE_H */
