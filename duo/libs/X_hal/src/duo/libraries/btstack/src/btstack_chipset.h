/*
 * Copyright (C) 2016 BlueKitchen GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 4. Any redistribution, use, or modification is done solely for
 *    personal benefit and not for any commercial purpose or for
 *    monetary gain.
 *
 * THIS SOFTWARE IS PROVIDED BY BLUEKITCHEN GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MATTHIAS
 * RINGWALD OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Please inquire about commercial licensing options at 
 * contact@bluekitchen-gmbh.com
 *
 */

/*
 *  btstack_chipset.h
 *
 *  Chipset Driver - implements custom chipset initializtion and support proprietary extensions
 *  to set UART baud rate, Bluetooth Address, and similar.
 */

#ifndef __BTSTACK_CHIPSET_H
#define __BTSTACK_CHIPSET_H

#include <stdint.h>
#include "btstack_util.h"

#if defined __cplusplus
extern "C" {
#endif

typedef enum {
  BTSTACK_CHIPSET_DONE = 0,
  BTSTACK_CHIPSET_VALID_COMMAND,
  BTSTACK_CHIPSET_WARMSTART_REQUIRED,
} btstack_chipset_result_t;


typedef struct {
    /**
     * chipset driver name
     */
    const char * name;

    /**
     * init driver
     * allows to reset init script index
     * @param transport_config
     */
    void (*init)(const void * transport_config);

    /**
     * support custom init sequences after RESET command
     * @param  hci_cmd_buffer to store generated command
     * @return result see btstack_chipset_result_t
     */
    btstack_chipset_result_t (*next_command)(uint8_t * hci_cmd_buffer); 

    /**
     * provide UART Baud Rate change command.
     * @param baudrate
     * @param hci_cmd_buffer to store generated command
     */
    void (*set_baudrate_command)(uint32_t baudrate, uint8_t *hci_cmd_buffer); 
    
    /** provide Set BD Addr command
     * @param baudrate
     * @param hci_cmd_buffer to store generated command
     */
    void (*set_bd_addr_command)(bd_addr_t addr, uint8_t *hci_cmd_buffer); 

} btstack_chipset_t;

#if defined __cplusplus
}
#endif

#endif // __BTSTACK_CHIPSET_H
