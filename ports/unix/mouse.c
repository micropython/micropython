/**
 * @file mouse.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "mouse.h"
#if USE_MOUSE != 0

/*********************
 *      DEFINES
 *********************/
#ifndef MONITOR_ZOOM
#define MONITOR_ZOOM    1
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static bool left_button_down = false;
static int16_t last_x = 0;
static int16_t last_y = 0;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the mouse
 */
void mouse_init(void)
{

}

/**
 * Get the current position and state of the mouse
 * @param data store the mouse data here
 * @return false: because the points are not buffered, so no more data to be read
 */
bool mouse_read(lv_indev_data_t * data)
{
    /*Store the collected data*/
    data->point.x = last_x;
    data->point.y = last_y;
    data->state = left_button_down ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;

    return false;
}

/**
 * It will be called from the main SDL thread
 */
void mouse_handler(SDL_Event * event)
{
    switch(event->type) {
        case SDL_MOUSEBUTTONUP:
            if(event->button.button == SDL_BUTTON_LEFT)
                left_button_down = false;
            break;
        case SDL_MOUSEBUTTONDOWN:
            if(event->button.button == SDL_BUTTON_LEFT) {
                left_button_down = true;
                last_x = event->motion.x / MONITOR_ZOOM;
                last_y = event->motion.y / MONITOR_ZOOM;
            }
            break;
        case SDL_MOUSEMOTION:
            last_x = event->motion.x / MONITOR_ZOOM;
            last_y = event->motion.y / MONITOR_ZOOM;

            break;
    }

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif
