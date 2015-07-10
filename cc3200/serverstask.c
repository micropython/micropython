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
#include <string.h>

#include "py/mpconfig.h"
#include MICROPY_HAL_H
#include "py/misc.h"
#include "serverstask.h"
#include "simplelink.h"
#include "debug.h"
#include "telnet.h"
#include "ftp.h"
#include "pybwdt.h"
#include "modusocket.h"


/******************************************************************************
 DECLARE PRIVATE DEFINITIONS
 ******************************************************************************/

#define SERVERS_DEF_USER                "micro"
#define SERVERS_DEF_PASS                "python"
#define SERVERS_DEF_TIMEOUT_MS          300000        // 5 minutes
#define SERVERS_MIN_TIMEOUT_MS          5000          // 5 seconds

/******************************************************************************
 DEFINE PRIVATE TYPES
 ******************************************************************************/
typedef struct {
    uint32_t timeout;
    bool enabled;
    bool do_disable;
    bool do_enable;
    bool do_reset;
} servers_data_t;

/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
static servers_data_t servers_data = {.timeout = SERVERS_DEF_TIMEOUT_MS, .enabled = false, .do_disable = false,
                                      .do_enable = false, .do_reset = false};
static volatile bool sleep_sockets = false;

/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/

/******************************************************************************
 DECLARE PUBLIC DATA
 ******************************************************************************/
char servers_user[SERVERS_USER_PASS_LEN_MAX + 1];
char servers_pass[SERVERS_USER_PASS_LEN_MAX + 1];

/******************************************************************************
 DECLARE PUBLIC FUNCTIONS
 ******************************************************************************/
void TASK_Servers (void *pvParameters) {

    bool cycle = false;

    strcpy (servers_user, SERVERS_DEF_USER);
    strcpy (servers_pass, SERVERS_DEF_PASS);

    telnet_init();
    ftp_init();

    for ( ;; ) {

        if (servers_data.do_enable) {
            // enable network services
            telnet_enable();
            ftp_enable();
            // now set/clear the flags
            servers_data.enabled = true;
            servers_data.do_enable = false;
        }
        else if (servers_data.do_disable) {
            // disable network services
            telnet_disable();
            ftp_disable();
            // now clear the flags
            servers_data.do_disable = false;
            servers_data.enabled = false;
        }
        else if (servers_data.do_reset) {
            // resetting the servers is needed to prevent half-open sockets
            servers_data.do_reset = false;
            if (servers_data.enabled) {
                telnet_reset();
                ftp_reset();
            }
            // and we should also close all user sockets. We do it here
            // for convinience and to save on code size.
            modusocket_close_all_user_sockets();
        }

        if (cycle) {
            telnet_run();
        }
        else {
            ftp_run();
        }

        if (sleep_sockets) {
            sleep_sockets = false;
            pybwdt_srv_sleeping(true);
            modusocket_enter_sleep();
            pybwdt_srv_sleeping(false);
        }

        // set the alive flag for the wdt
        pybwdt_srv_alive();

        // move to the next cycle
        cycle = cycle ? false : true;
        HAL_Delay(SERVERS_CYCLE_TIME_MS);
    }
}

void servers_start (void) {
    servers_data.do_enable = true;
    HAL_Delay (SERVERS_CYCLE_TIME_MS * 5);
}

void servers_stop (void) {
    servers_data.do_disable = true;
    do {
        HAL_Delay (SERVERS_CYCLE_TIME_MS);
    } while (servers_are_enabled());
    HAL_Delay (SERVERS_CYCLE_TIME_MS * 5);
}

void servers_reset (void) {
    servers_data.do_reset = true;
}

bool servers_are_enabled (void) {
    return servers_data.enabled;
}

void server_sleep_sockets (void) {
    sleep_sockets = true;
    HAL_Delay (SERVERS_CYCLE_TIME_MS + 1);
}

void servers_close_socket (int16_t *sd) {
    if (*sd > 0) {
        modusocket_socket_delete(*sd);
        sl_Close(*sd);
        *sd = -1;
    }
}

void servers_set_login (char *user, char *pass) {
    memcpy(servers_user, user, SERVERS_USER_PASS_LEN_MAX);
    memcpy(servers_pass, pass, SERVERS_USER_PASS_LEN_MAX);
}

bool servers_set_timeout (uint32_t timeout) {
    if (timeout < SERVERS_MIN_TIMEOUT_MS) {
        return false;
    }
    servers_data.timeout = timeout;
    return true;
}

uint32_t servers_get_timeout (void) {
    return servers_data.timeout;
}

/******************************************************************************
 DEFINE PRIVATE FUNCTIONS
 ******************************************************************************/
