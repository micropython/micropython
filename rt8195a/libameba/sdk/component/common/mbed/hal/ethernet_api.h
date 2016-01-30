/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MBED_ETHERNET_API_H
#define MBED_ETHERNET_API_H

#include "device.h"

#if DEVICE_ETHERNET

#ifdef __cplusplus
extern "C" {
#endif

// Connection constants

int ethernet_init(void);
void ethernet_free(void);

// write size bytes from data to ethernet buffer
// return num bytes written
// or -1 if size is too big
int ethernet_write(const char *data, int size);

// send ethernet write buffer, returning the packet size sent
int ethernet_send(void);

// recieve from ethernet buffer, returning packet size, or 0 if no packet
int ethernet_receive(void);

// read size bytes in to data, return actual num bytes read (0..size)
// if data == NULL, throw the bytes away
int ethernet_read(char *data, int size);

// get the ethernet address
void ethernet_address(char *mac);

// see if the link is up
int ethernet_link(void);

// force link settings
void ethernet_set_link(int speed, int duplex);

#ifdef __cplusplus
}
#endif

#endif

#endif

