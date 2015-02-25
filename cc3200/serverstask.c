/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Daniel Campora
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdint.h>
#include <std.h>

#include "py/mpconfig.h"
#include MICROPY_HAL_H
#include "py/misc.h"
#include "simplelink.h"
#include "serverstask.h"
#include "modwlan.h"
#include "debug.h"
#include "mpexception.h"
#include "telnet.h"
#include "ftp.h"
#include "pybwdt.h"


/******************************************************************************
 DECLARE PRIVATE DEFINITIONS
 ******************************************************************************/

#define SERVERS_DEF_USER            "micro"
#define SERVERS_DEF_PASS            "python"

/******************************************************************************
 DEFINE PRIVATE TYPES
 ******************************************************************************/
typedef struct {
    volatile bool enabled;
    volatile bool do_disable;
    volatile bool do_enable;
}servers_Data_t;

/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
static servers_Data_t servers_data = {.enabled = false, .do_disable = false, .do_enable = false};

/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/

/******************************************************************************
 DECLARE PUBLIC DATA
 ******************************************************************************/
char *servers_user;
char *servers_pass;

/******************************************************************************
 DECLARE PUBLIC FUNCTIONS
 ******************************************************************************/

void TASK_Servers (void *pvParameters) {

    bool cycle = false;

    ASSERT ((servers_user = mem_Malloc(SERVERS_USER_LEN_MAX + 1)) != NULL);
    ASSERT ((servers_pass = mem_Malloc(SERVERS_PASS_LEN_MAX + 1)) != NULL);
    strcpy (servers_user, SERVERS_DEF_USER);
    strcpy (servers_pass, SERVERS_DEF_PASS);

    telnet_init();
    ftp_init();

    for ( ;; ) {

        if (servers_data.enabled) {
            if (servers_data.do_disable) {
                servers_data.do_disable = false;
                // disable all net processes
                telnet_disable();
                ftp_disable();

                // now clear the flag
                servers_data.enabled = false;
            }
            else {
                if (cycle) {
                    telnet_run();
                }
                else {
                    ftp_run();
                }
            }
        }
        else if (servers_data.do_enable) {
            servers_data.do_enable = false;

            telnet_enable();
            ftp_enable();

            // now set the flag
            servers_data.enabled = true;
        }

        cycle = cycle ? false : true;
        HAL_Delay(SERVERS_CYCLE_TIME_MS);
        // set the alive flag for the wdt
        pybwdt_srv_alive();
    }
}

void servers_enable (void) {
    servers_data.do_disable = false;
    servers_data.do_enable = true;
}

void servers_disable (void) {
    servers_data.do_enable = false;
    servers_data.do_disable = true;
}

bool servers_are_enabled (void) {
    return servers_data.enabled;
}

void servers_close_socket (_i16 *sd) {
    if (*sd > 0) {
        sl_Close(*sd);
        *sd = -1;
    }
}

void servers_set_user_pass (char *user, char *pass) {
    memcpy(servers_user, user, SERVERS_USER_LEN_MAX);
    memcpy(servers_pass, pass, SERVERS_PASS_LEN_MAX);
}

/******************************************************************************
 DEFINE PRIVATE FUNCTIONS
 ******************************************************************************/
