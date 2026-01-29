/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022-2026 Infineon Technologies AG
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

// std includes
#include <stdio.h>

// micropython includes
#include "py/runtime.h"
#include "py/mphal.h"

// mtb includes  
#include "cybsp.h"
#include "cy_pdl.h"
#include "ipc_communication.h"

// port-specific includes
#include "modipc.h"

/*******************************************************************************
* Macros
*******************************************************************************/
/* CM55 boot address - matches official BSP configuration */
#define CM55_APP_BOOT_ADDR          (CYMEM_CM33_0_m55_nvm_START + CYBSP_MCUBOOT_HEADER_SIZE)

/*******************************************************************************
* Global Variables
*******************************************************************************/
static bool cm55_enabled = false;
static bool cm55_ipc_initialized = false;
CY_SECTION_SHAREDMEM static ipc_msg_t cm33_msg_data;
static volatile uint32_t cm55_last_response = 0;
static volatile bool cm55_response_received = false;

/*******************************************************************************
* Function Name: ipc_enable
********************************************************************************
* Summary:
*  Initialize IPC with CM55 (CM55 is already booted by main.c)
*
* Return:
*  mp_obj_t: None
*
*******************************************************************************/
static mp_obj_t ipc_enable(void) {
    if (cm55_enabled && cm55_ipc_initialized) {
        mp_printf(&mp_plat_print, "CM55 already enabled and IPC initialized\n");
        return mp_const_none;
    }
    
    mp_printf(&mp_plat_print, "Initializing IPC with CM55 (CM55 already booted by main.c)...\n");
    
    // ToDo: Enable IPC from here instead from main.c
    /* CM55 is already booted by main.c, IPC and callback are already set up */
    if (!cm55_ipc_initialized) {
        mp_printf(&mp_plat_print, "IPC already set up by main.c\n");
        cm55_ipc_initialized = true;
    }

    cm55_enabled = true;
    
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(ipc_enable_obj, ipc_enable);


/*******************************************************************************
* Function Name: ipc_status
********************************************************************************
* Summary:
*  Returns whether CM55 is enabled
*
* Return:
*  mp_obj_t: True if CM55 is enabled, False otherwise
*
*******************************************************************************/
static mp_obj_t ipc_status(void) {
    return mp_obj_new_bool(cm55_enabled);
}
static MP_DEFINE_CONST_FUN_OBJ_0(ipc_status_obj, ipc_status);


/*******************************************************************************
* Function Name: ipc_led_init
********************************************************************************
* Summary:
*  Sends command to CM55 to initialize LED (runs on CM55)
*
* Return:
*  mp_obj_t: None
*
*******************************************************************************/
//static mp_obj_t ipc_led_init(void) {
//    /* Auto-initialize if not enabled */
//    if (!cm55_enabled) {
//        ipc_enable();
//    }
//    
//    mp_printf(&mp_plat_print, "[CM33] Sending LED_INIT command to CM55...\n");
//    
//    cm33_msg_data.client_id = CM55_IPC_PIPE_CLIENT_ID;  // Destination client ID
//    cm33_msg_data.intr_mask = CY_IPC_CYPIPE_INTR_MASK;
//    cm33_msg_data.cmd = IPC_CMD_LED_INIT;
//    cm33_msg_data.value = 0;
//    
//    cy_en_ipc_pipe_status_t status;
//    status = Cy_IPC_Pipe_SendMessage(CM55_IPC_PIPE_EP_ADDR,
//                                     CM33_IPC_PIPE_EP_ADDR,
//                                     (void *)&cm33_msg_data,
//                                     NULL);
//    
//    if (status != CY_IPC_PIPE_SUCCESS) {
//        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Failed to send LED init command"));
//    }
//    
//    mp_printf(&mp_plat_print, "[CM33] LED init command sent to CM55\n");
//    return mp_const_none;
//}
//static MP_DEFINE_CONST_FUN_OBJ_0(ipc_led_init_obj, ipc_led_init);

/*******************************************************************************
* Function Name: ipc_led_set_on
********************************************************************************
* Summary:
*  Sends command to CM55 to set LED constantly ON (runs on CM55)
*
* Return:
*  mp_obj_t: None
*
*******************************************************************************/
static mp_obj_t ipc_sendcmd_led_on(void) {
    /* Auto-initialize if not enabled */
    if (!cm55_enabled) {
        ipc_enable();
    }
    
    mp_printf(&mp_plat_print, "[CM33] Sending LED_SET_ON command to CM55...\n");
    
    cm33_msg_data.client_id = CM55_IPC_PIPE_CLIENT_ID;  // Destination client ID
    cm33_msg_data.intr_mask = CY_IPC_CYPIPE_INTR_MASK;
    cm33_msg_data.cmd = IPC_CMD_LED_SET_ON;
    cm33_msg_data.value = 0;
    
    mp_printf(&mp_plat_print, "[CM33] IPC message prepared: client_id=%d, cmd=0x93\n", 
              cm33_msg_data.client_id, cm33_msg_data.cmd);
    
    /* Use direct IPC channel write instead of IPC Pipe to avoid blocking */
    mp_printf(&mp_plat_print, "[CM33] Using direct IPC channel write (non-blocking)...\n");
    
    /* Check if IPC channel 15 (CM55's channel) is unlocked */
    bool is_locked = Cy_IPC_Drv_IsLockAcquired(Cy_IPC_Drv_GetIpcBaseAddress(CY_IPC_CHAN_CYPIPE_EP2));
    mp_printf(&mp_plat_print, "[CM33] IPC channel %d lock status: %s\n", 
              CY_IPC_CHAN_CYPIPE_EP2, is_locked ? "LOCKED" : "UNLOCKED");
    
    if (is_locked) {
        mp_printf(&mp_plat_print, "[CM33] Channel is locked, cannot send\n");
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("IPC channel locked"));
    }
    
    /* Write directly to IPC channel and trigger CM55 interrupt */
    cy_en_ipcdrv_status_t drv_status = Cy_IPC_Drv_SendMsgPtr(
        Cy_IPC_Drv_GetIpcBaseAddress(CY_IPC_CHAN_CYPIPE_EP2),
        CY_IPC_CYPIPE_INTR_MASK_EP2,  // Notify mask - trigger interrupt on CM55
        (void *)&cm33_msg_data
    );
    
    mp_printf(&mp_plat_print, "[CM33] Direct IPC send status: %d (0=success)\n", drv_status);
    
    if (drv_status != CY_IPC_DRV_SUCCESS) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("IPC direct send failed"));
    }
    
    mp_printf(&mp_plat_print, "[CM33] Message sent and CM55 interrupt triggered\n");
    
    mp_printf(&mp_plat_print, "[CM33] LED will be set constantly ON on CM55\n");
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(ipc_sendcmd_led_on_obj, ipc_sendcmd_led_on);

// Turn LED OFF (constantly) - runs on CM55
static mp_obj_t ipc_sendcmd_led_off(void) {
    mp_printf(&mp_plat_print, "[CM33] Sending LED_SET_OFF command to CM55...\n");
    
    // Prepare IPC message with LED_SET_OFF command
    cm33_msg_data.client_id = CM55_IPC_PIPE_CLIENT_ID;  // Destination is CM55
    cm33_msg_data.cmd = IPC_CMD_LED_SET_OFF;
    cm33_msg_data.intr_mask = 0;
    
    mp_printf(&mp_plat_print, "[CM33] IPC message prepared: client_id=%d, cmd=0x%02X\n",
              cm33_msg_data.client_id, cm33_msg_data.cmd);
    
    mp_printf(&mp_plat_print, "[CM33] Using direct IPC channel write (non-blocking)...\n");
    
    // Check if channel is locked
    bool is_locked = Cy_IPC_Drv_IsLockAcquired(Cy_IPC_Drv_GetIpcBaseAddress(CY_IPC_CHAN_CYPIPE_EP2));
    mp_printf(&mp_plat_print, "[CM33] IPC channel 15 lock status: %s\n", 
              is_locked ? "LOCKED" : "UNLOCKED");
    
    // Use low-level IPC driver API (non-blocking)
    cy_en_ipcdrv_status_t drv_status = Cy_IPC_Drv_SendMsgPtr(
        Cy_IPC_Drv_GetIpcBaseAddress(CY_IPC_CHAN_CYPIPE_EP2),
        CY_IPC_CYPIPE_INTR_MASK_EP2,  // Trigger CM55 interrupt
        &cm33_msg_data
    );
    
    mp_printf(&mp_plat_print, "[CM33] Direct IPC send status: %d (0=success)\n", drv_status);
    
    if (drv_status != CY_IPC_DRV_SUCCESS) {
        mp_printf(&mp_plat_print, "[CM33] Direct send failed with status: %d\n", drv_status);
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("IPC direct send failed"));
    }
    
    mp_printf(&mp_plat_print, "[CM33] Message sent and CM55 interrupt triggered\n");
    
    mp_printf(&mp_plat_print, "[CM33] LED will be set constantly OFF on CM55\n");
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(ipc_sendcmd_led_off_obj, ipc_sendcmd_led_off);

// Module globals table
static const mp_rom_map_elem_t ipc_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ipc) },
    { MP_ROM_QSTR(MP_QSTR_enable), MP_ROM_PTR(&ipc_enable_obj) },
    { MP_ROM_QSTR(MP_QSTR_status), MP_ROM_PTR(&ipc_status_obj) },
    //{ MP_ROM_QSTR(MP_QSTR_led_init), MP_ROM_PTR(&ipc_led_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_sendcmd_led_on), MP_ROM_PTR(&ipc_sendcmd_led_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_sendcmd_led_off), MP_ROM_PTR(&ipc_sendcmd_led_off_obj) },
};
static MP_DEFINE_CONST_DICT(ipc_module_globals, ipc_module_globals_table);

// Module definition
const mp_obj_module_t mp_module_ipc = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&ipc_module_globals,
};

// Register the module
MP_REGISTER_MODULE(MP_QSTR_ipc, mp_module_ipc);
