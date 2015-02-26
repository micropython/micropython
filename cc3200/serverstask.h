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

#ifndef SERVERSTASK_H_
#define SERVERSTASK_H_

/******************************************************************************
 DEFINE CONSTANTS
 ******************************************************************************/
#define SERVERS_PRIORITY                        2
#define SERVERS_STACK_SIZE                      960

#define SERVERS_SSID_LEN_MAX                    16
#define SERVERS_KEY_LEN_MAX                     16

#define SERVERS_USER_LEN_MAX                    16
#define SERVERS_PASS_LEN_MAX                    16

#define SERVERS_DEF_AP_SSID                     "micropy-wlan"
#define SERVERS_DEF_AP_SECURITY                 SL_SEC_TYPE_WPA_WPA2
#define SERVERS_DEF_AP_KEY                      "micropython"
#define SERVERS_DEF_AP_CHANNEL                  6

#define SERVERS_CYCLE_TIME_MS                   5

/******************************************************************************
 DEFINE TYPES
 ******************************************************************************/

/******************************************************************************
 EXPORTED DATA
 ******************************************************************************/
extern char *servers_user;
extern char *servers_pass;

/******************************************************************************
 DECLARE PUBLIC FUNCTIONS
 ******************************************************************************/
extern void TASK_Servers (void *pvParameters);
extern void servers_enable (void);
extern void servers_disable (void);
extern bool servers_are_enabled (void);
extern void servers_close_socket (_i16 *sd);
extern void servers_set_user_pass (char *user, char *pass);

#endif /* SERVERSTASK_H_ */
