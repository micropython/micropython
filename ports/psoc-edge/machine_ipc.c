/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Infineon Technologies AG
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
#include "py/runtime.h"
#include "py/mphal.h"
#include "cybsp.h"
#include "cy_pdl.h"
#include "cy_ipc_pipe.h"


#if MULTI_CORE
// port-specific includes
#include "machine_ipc.h"

/*******************************************************************************
* Macros
*******************************************************************************/
#define MICROPY_MAX_IPC_CH_INSTANCES    (8UL)
#define MICROPY_MAX_IPC_ENDPOINTS       (5UL)
#define CY_IPC_MAX_ENDPOINTS            (5UL) // Are fixed and pre-defined for platform based on hardware capabilities
#define CY_IPC_CYPIPE_CLIENT_CNT        (8UL)
#define CM55_APP_BOOT_ADDR          (CYMEM_CM33_0_m55_nvm_START + CYBSP_MCUBOOT_HEADER_SIZE)
#define CM55_BOOT_WAIT_TIME_USEC    (10U)
#define CM55_APP_DELAY_MS           (50U)

#define CM33_APP_DELAY_MS           (50U)

/* Core ID constants exposed to user */
#define IPC_CORE_CM33               (0U)
#define IPC_CORE_CM55               (1U)

/* LED command constants exposed to user */
#define IPC_CMD_LED_INIT            (0x92)
#define IPC_CMD_LED_SET_ON          (0x93)
#define IPC_CMD_LED_SET_OFF         (0x94)

/*******************************************************************************
* Global Variables
*******************************************************************************/
static bool cm55_enabled = false;
CY_SECTION_SHAREDMEM static ipc_msg_t ipc_msg_buffer;
machine_ipc_obj_t *machine_ipc_obj[MICROPY_MAX_IPC_CH_INSTANCES] = {NULL};
// static cy_stc_ipc_pipe_ep_t ipc_endpoints[CY_IPC_MAX_ENDPOINTS]; //ToDo: Should be inside the IPC object struct
// static cy_ipc_pipe_callback_ptr_t ep2_callbacks[CY_IPC_CYPIPE_CLIENT_CNT]; //ToDo: Should be inside the IPC object struct


// Helper functions
/*******************************************************************************
* Dynamic IPC Configuration Structure Builder
*******************************************************************************/

/**
 * Get endpoint configuration based on core ID
 * Returns endpoint address, channel, interrupt, and other config params
 */
static void get_endpoint_config_for_core(uint8_t core_id,
    uint32_t *ep_addr,
    uint32_t *ep_channel,
    uint32_t *ep_intr,
    uint32_t *ep_intr_mux,
    uint32_t *ep_intr_priority) {
    switch (core_id) {
        case IPC_CORE_CM33:
            *ep_addr = CM33_IPC_PIPE_EP_ADDR;
            *ep_channel = CY_IPC_CHAN_CYPIPE_EP1;
            *ep_intr = CY_IPC_INTR_CYPIPE_EP1;
            *ep_intr_mux = CY_IPC_INTR_CYPIPE_MUX_EP1;
            *ep_intr_priority = CY_IPC_INTR_CYPIPE_PRIOR_EP1;
            break;

        case IPC_CORE_CM55:
            *ep_addr = CM55_IPC_PIPE_EP_ADDR;
            *ep_channel = CY_IPC_CHAN_CYPIPE_EP2;
            *ep_intr = CY_IPC_INTR_CYPIPE_EP2;
            *ep_intr_mux = CY_IPC_INTR_CYPIPE_MUX_EP2;
            *ep_intr_priority = CY_IPC_INTR_CYPIPE_PRIOR_EP2;
            break;

        default:
            // Invalid core - set to zero
            *ep_addr = 0;
            *ep_channel = 0;
            *ep_intr = 0;
            *ep_intr_mux = 0;
            *ep_intr_priority = 0;
            break;
    }
}

static bool build_ipc_pipe_config(cy_stc_ipc_pipe_config_t *config_out,
    uint8_t src_core,
    uint8_t target_core,
    cy_ipc_pipe_callback_ptr_t *callbacks_array,
    void (*isr_handler)(void)) {

    // Validate cores
    if (src_core >= IPC_CORE_CM55 + 1 || target_core >= IPC_CORE_CM55 + 1) {
        printf("Error: Invalid core IDs (src=%d, target=%d)\r\n", src_core, target_core);
        return false;
    }

    if (src_core == target_core) {
        printf("Error: Source and target cores cannot be the same\r\n");
        return false;
    }

    // Get source endpoint configuration
    uint32_t src_addr, src_channel, src_intr, src_intr_mux, src_intr_priority;
    get_endpoint_config_for_core(src_core, &src_addr, &src_channel, &src_intr,
        &src_intr_mux, &src_intr_priority);

    // Get target endpoint configuration
    uint32_t tgt_addr, tgt_channel, tgt_intr, tgt_intr_mux, tgt_intr_priority;
    get_endpoint_config_for_core(target_core, &tgt_addr, &tgt_channel, &tgt_intr,
        &tgt_intr_mux, &tgt_intr_priority);

    // Build configuration structure
    // Endpoint 0: Source (receiver) endpoint
    config_out->ep0ConfigData.ipcNotifierNumber = src_intr;
    config_out->ep0ConfigData.ipcNotifierPriority = src_intr_priority;
    config_out->ep0ConfigData.ipcNotifierMuxNumber = src_intr_mux;
    config_out->ep0ConfigData.epAddress = src_addr;
    config_out->ep0ConfigData.epConfig.epChannel = src_channel;
    config_out->ep0ConfigData.epConfig.epIntr = src_intr;
    config_out->ep0ConfigData.epConfig.epIntrmask = CY_IPC_CYPIPE_INTR_MASK;

    // Endpoint 1: Target (sender) endpoint
    config_out->ep1ConfigData.ipcNotifierNumber = tgt_intr;
    config_out->ep1ConfigData.ipcNotifierPriority = tgt_intr_priority;
    config_out->ep1ConfigData.ipcNotifierMuxNumber = tgt_intr_mux;
    config_out->ep1ConfigData.epAddress = tgt_addr;
    config_out->ep1ConfigData.epConfig.epChannel = tgt_channel;
    config_out->ep1ConfigData.epConfig.epIntr = tgt_intr;
    config_out->ep1ConfigData.epConfig.epIntrmask = CY_IPC_CYPIPE_INTR_MASK;

    // Common configuration
    config_out->endpointClientsCount = CY_IPC_CYPIPE_CLIENT_CNT;
    config_out->endpointsCallbacksArray = callbacks_array;
    config_out->userPipeIsrHandler = isr_handler;

    return true;
}

/*******************************************************************************
* CM33 Message Callback - receives messages from CM55
*******************************************************************************/
/*static uint32_t machine_ipc_obj_find_ch_index(uint8_t target_core, uint8_t client_id) {
    uint32_t idx = 0;
    for (idx = 0; idx < MICROPY_MAX_IPC_CH_INSTANCES; idx++) {
        machine_ipc_obj_t *ipc = machine_ipc_obj[idx];
        if (ipc != NULL) {
            if (ipc->target_core == target_core && ipc->client_id == client_id) {
                return idx;
            }
        }
    }
    return -1; // Not found - should not happen if called correctly
}*/

/*******************************************************************************
* CM33 Message Callback - receives messages from CM55
* This callback is invoked by the IPC pipe ISR when a message arrives
*
* @param msgData Pointer to the received message data
*******************************************************************************/
/*void cm33_msg_callback(uint32_t *msgData) {
    if (msgData == NULL) {
        return;
    }

    ipc_msg_t *ipc_recv_msg = (ipc_msg_t *)msgData;

}*/

void cm33_msg_callback(uint32_t *msg_data) {
    // ipc_msg_t *ipc_recv_msg;

    if (msg_data != NULL) {
        /* Cast the message received to the IPC structure */
        // ipc_recv_msg = (ipc_msg_t *) msg_data;

        /* Extract the command to be processed in the main loop */
        // msg_cmd = ipc_recv_msg->cmd;
    }
}



/*******************************************************************************
* IPC Helper Functions
*******************************************************************************/

/**
 * Register a client callback for receiving IPC messages
 *
 * @param client_id Client ID (0-7)
 * @param callback_handler Python callback function object
 * @param endpoint_addr Endpoint address to register with (e.g., CM33_IPC_PIPE_EP_ADDR)
 * @return true if registration successful, false otherwise
 */
static bool ipc_register_client_callback(uint8_t client_id,
    mp_obj_t callback_handler,
    uint32_t endpoint_addr) {
    // Validate client_id
    if (client_id >= CY_IPC_CYPIPE_CLIENT_CNT) {
        printf("[CM33] Error: Invalid client_id %d (max %ld)\r\n",
            client_id, CY_IPC_CYPIPE_CLIENT_CNT - 1);
        return false;
    }

    // Validate callback
    if (callback_handler != mp_const_none && !mp_obj_is_callable(callback_handler)) {
        printf("[CM33] Error: Callback is not callable\r\n");
        return false;
    }

    // Store the Python callback in the global client array
    sender_clients_arr[client_id].client_id = client_id;
    sender_clients_arr[client_id].cback_handler = callback_handler;

    printf("[CM33] Registering callback for client %d at endpoint 0x%08lX\r\n",
        client_id, endpoint_addr);

    // Register the C callback with IPC pipe framework
    // The client_id parameter is the index in the callback array where the function pointer is saved
    cy_en_ipc_pipe_status_t status = Cy_IPC_Pipe_RegisterCallback(
        endpoint_addr,           // Endpoint address (CM33_IPC_PIPE_EP_ADDR)
        &cm33_msg_callback,       // Callback function pointer
        (uint32_t)client_id      // Client ID - index in callback array
        );

    if (status != CY_IPC_PIPE_SUCCESS) {
        printf("[CM33] Error: Cy_IPC_Pipe_RegisterCallback failed with status %d\r\n", status);
        return false;
    }

    printf("[CM33] Client %d callback registered successfully\r\n", client_id);
    return true;
}

/**
 * Send an IPC message to target core
 *
 * @param src_endpoint_addr Source endpoint address (e.g., CM33_IPC_PIPE_EP_ADDR)
 * @param target_endpoint_addr Target endpoint address (e.g., CM55_IPC_PIPE_EP_ADDR)
 * @param target_client_id Target client ID (0-7)
 * @param cmd Command byte
 * @param value Data value
 * @return true if send successful, false otherwise
 */
static bool ipc_send_message(uint32_t src_endpoint_addr,
    uint32_t target_endpoint_addr,
    uint8_t target_client_id,
    uint8_t cmd,
    uint32_t value) {
    // Validate client_id
    if (target_client_id >= CY_IPC_CYPIPE_CLIENT_CNT) {
        printf("[CM33] Error: Invalid target_client_id %d\r\n", target_client_id);
        return false;
    }

    printf("[CM33] Sending: client=%d, cmd=0x%02X, value=0x%08lX\r\n",
        target_client_id, cmd, value);

    // Prepare message in shared memory buffer
    ipc_msg_buffer.client_id = target_client_id;
    ipc_msg_buffer.intr_mask = CY_IPC_CYPIPE_INTR_MASK;
    ipc_msg_buffer.cmd = cmd;
    ipc_msg_buffer.value = value;

    // Wait for channel to be free, then send
    cy_en_ipc_pipe_status_t status;
    uint32_t retry_count = 0;
    const uint32_t MAX_RETRIES = 100000;  // Allow more retries

    do {
        status = Cy_IPC_Pipe_SendMessage(
            src_endpoint_addr,
            target_endpoint_addr,
            (void *)&ipc_msg_buffer,
            NULL  // No release callback
            );

        if (status == CY_IPC_PIPE_SUCCESS) {
            break;
        }

        // If channel is busy, wait a bit and retry
        if (status == CY_IPC_PIPE_ERROR_SEND_BUSY) {
            Cy_SysLib_DelayUs(100);  // 100us delay
            retry_count++;
        } else {
            // Other error - fail immediately
            printf("[CM33] Error: Cy_IPC_Pipe_SendMessage failed with status %d\r\n", status);
            return false;
        }
    } while (retry_count < MAX_RETRIES);

    if (status != CY_IPC_PIPE_SUCCESS) {
        printf("[CM33] Error: Channel busy after %lu retries, status %d\r\n", retry_count, status);
        return false;
    }

    printf("[CM33] Message sent successfully\r\n");
    return true;
}

/**
 * Unregister a client callback
 *
 * @param client_id Client ID to unregister (0-7)
 * @return true if successful, false otherwise
 */
static bool ipc_unregister_client_callback(uint8_t client_id) {
    if (client_id >= CY_IPC_CYPIPE_CLIENT_CNT) {
        printf("[CM33] Error: Invalid client_id %d\r\n", client_id);
        return false;
    }

    // Clear the callback
    sender_clients_arr[client_id].cback_handler = mp_const_none;

    printf("[CM33] Client %d callback unregistered\r\n", client_id);
    return true;
}

/*******************************************************************************
* Setup Functions
*******************************************************************************/

/* Create an array of endpoint structures */
static cy_stc_ipc_pipe_ep_t cm33_ipc_pipe_ep_array[CY_IPC_MAX_ENDPOINTS];

/* CB Array for EP1 */
static cy_ipc_pipe_callback_ptr_t ep1_cb_array[CY_IPC_CYPIPE_CLIENT_CNT];

/* Allocate and initialize semaphores for the system operations. */
CY_SECTION_SHAREDMEM
static uint32_t ipc_sema_array[CY_IPC_SEMA_COUNT / CY_IPC_SEMA_PER_WORD];


void cm33_ipc_pipe_isr(void) {
    Cy_IPC_Pipe_ExecuteCallback(CM33_IPC_PIPE_EP_ADDR);
}

/**
 * Initialize IPC pipe with dynamic configuration
 *
 * @param src_core Source core ID
 * @param target_core Target core ID
 * @return true if initialization successful, false otherwise
 */
static bool ipc_pipe_init_dynamic(uint8_t src_core, uint8_t target_core) {

    // Allocate configuration structure
    cy_stc_ipc_pipe_config_t pipe_config;

    // Build configuration dynamically
    bool config_ok = build_ipc_pipe_config(&pipe_config,
        src_core,
        target_core,
        ep1_cb_array,
        &cm33_ipc_pipe_isr);

    if (!config_ok) {
        printf("Error: Failed to build IPC pipe configuration\r\n");
        return false;
    }

    // Initialize semaphores (always needed)
    Cy_IPC_Sema_Init(IPC0_SEMA_CH_NUM, CY_IPC_SEMA_COUNT, ipc_sema_array);

    // Configure endpoint array
    Cy_IPC_Pipe_Config(cm33_ipc_pipe_ep_array);

    // Initialize pipe with dynamic configuration
    Cy_IPC_Pipe_Init(&pipe_config);

    printf("IPC pipe initialized successfully\r\n");
    return true;
}

void ipc_pipe_init(void) {
    /* IPC pipe endpoint-1 and endpoint-2. CM33 <--> CM55 */
    static const cy_stc_ipc_pipe_config_t cm33_ipc_pipe_config =
    {
        /* receiver endpoint CM33 */
        {
            .ipcNotifierNumber = CY_IPC_INTR_CYPIPE_EP1,
            .ipcNotifierPriority = CY_IPC_INTR_CYPIPE_PRIOR_EP1,
            .ipcNotifierMuxNumber = CY_IPC_INTR_CYPIPE_MUX_EP1,
            .epAddress = CM33_IPC_PIPE_EP_ADDR,
            {
                .epChannel = CY_IPC_CHAN_CYPIPE_EP1,
                .epIntr = CY_IPC_INTR_CYPIPE_EP1,
                .epIntrmask = CY_IPC_CYPIPE_INTR_MASK
            }
        },
        /* sender endpoint CM55 */
        {
            .ipcNotifierNumber = CY_IPC_INTR_CYPIPE_EP2,
            .ipcNotifierPriority = CY_IPC_INTR_CYPIPE_PRIOR_EP2,
            .ipcNotifierMuxNumber = CY_IPC_INTR_CYPIPE_MUX_EP2,
            .epAddress = CM55_IPC_PIPE_EP_ADDR,
            {
                .epChannel = CY_IPC_CHAN_CYPIPE_EP2,
                .epIntr = CY_IPC_INTR_CYPIPE_EP2,
                .epIntrmask = CY_IPC_CYPIPE_INTR_MASK
            }
        },
        .endpointClientsCount = CY_IPC_CYPIPE_CLIENT_CNT,
        .endpointsCallbacksArray = ep1_cb_array,
        .userPipeIsrHandler = &cm33_ipc_pipe_isr
    };

    Cy_IPC_Sema_Init(IPC0_SEMA_CH_NUM, CY_IPC_SEMA_COUNT, ipc_sema_array);

    Cy_IPC_Pipe_Config(cm33_ipc_pipe_ep_array);

    Cy_IPC_Pipe_Init(&cm33_ipc_pipe_config);
}

/********************************************************************************************************
* MicroPython Constructor:
*  IPC(name, src_core, target_core) - Create an IPC object with for a specific service.
*  Max of 5 IPC objects can be created (based on hardware capabilities) and each object can represent
*  an endpoint/channel to communicate with the target core. The src_core and target_core
*  parameters can be used to set up the endpoint based on the cores involved in communication. Currently,
*  only CM33 to CM55 communication is supported and the endpoint configuration is fixed based on this use case but in
*  the future, this can be extended to support dynamic endpoint configuration based on the cores selected during
*  object creation.
********************************************************************************************************/
static mp_obj_t machine_ipc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_src_core, ARG_target_core };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_src_core, MP_ARG_INT, {.u_int = IPC_CORE_CM33} },
        { MP_QSTR_target_core, MP_ARG_INT, {.u_int = IPC_CORE_CM55} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    machine_ipc_obj_t *self = NULL;
    for (uint8_t i = 0; i < MICROPY_MAX_IPC_ENDPOINTS; i++) {
        if (MP_STATE_PORT(machine_ipc_obj[i]) == NULL) {
            self = mp_obj_malloc(machine_ipc_obj_t, &machine_ipc_type);
            MP_STATE_PORT(machine_ipc_obj[i]) = self;
            break;
        }
    }
    if (self == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("all available IPC instances are allocated"));
    }
    self->base.type = &machine_ipc_type;
    // Check if sender is not None and Set all sender endpoint details
    if (args[ARG_src_core].u_int != IPC_CORE_CM33) {
        mp_raise_ValueError(MP_ERROR_TEXT("Invalid src_core value, only CM33 is supported as source core for now"));
    }

    self->src_core = args[ARG_src_core].u_int;

    self->sender_endpoint = mp_obj_malloc(ipc_sender_endpoint_t, NULL);
    self->sender_endpoint->ep_sender_id = args[ARG_src_core].u_int;
    self->sender_endpoint->ep_sender_addr = CM33_IPC_PIPE_EP_ADDR;
    // Point to NULL for now and later in register_client function allocate to point to sender_clients_arr[id]
    self->sender_endpoint->sender_client = NULL;

    if (args[ARG_target_core].u_int != IPC_CORE_CM55) {
        mp_raise_ValueError(MP_ERROR_TEXT("Invalid target_core value, only CM55 is supported as target core for now"));
    }

    self->target_core = args[ARG_target_core].u_int;
    return MP_OBJ_FROM_PTR(self);
}

/*static mp_obj_t machine_ipc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_core };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_core, MP_ARG_INT, {.u_int = IPC_CORE_CM55} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    machine_ipc_obj_t *self = NULL;
    for (uint8_t i = 0; i < MICROPY_MAX_IPC_CH_INSTANCES; i++) {
        if (MP_STATE_PORT(machine_ipc_obj[i]) == NULL) {
            self = mp_obj_malloc(machine_ipc_obj_t, &machine_ipc_type);
            MP_STATE_PORT(machine_ipc_obj[i]) = self;
            self->client_id = i;
            break;
        }
    }
    if (self == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("all available IPC instances are allocated"));
    }
    self->base.type = &machine_ipc_type;

    self->target_core = args[ARG_core].u_int; // Should help fix the endpoint related structures based on the core selected. Currently only CM55 is supported so this is not being used but it is added with a future vision to support multiple cores and dynamic endpoint allocation based on core selection.
    // Set endpoint addresses based on target_core selected and current core. Should there be a function to get current core? Let's hardcore now to CM33 for this use case


    self->is_initialized = false;
    self->client_info = NULL; // No client information registered initially

    return MP_OBJ_FROM_PTR(self);
}*/

/*******************************************************************************
* IPC.init() - Initialize IPC communication
*******************************************************************************/
static mp_obj_t machine_ipc_init(mp_obj_t self_in) {
    machine_ipc_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_printf(&mp_plat_print, "Initializing IPC pipe framework\r\n");

    // Initialize IPC pipe with dynamic configuration based on object's cores
    bool init_ok = ipc_pipe_init_dynamic(self->src_core, self->target_core);

    if (!init_ok) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("IPC pipe initialization failed"));
    }


    // Initialize IPC pipe
    // ipc_pipe_init();

    Cy_SysLib_Delay(CM33_APP_DELAY_MS);

    // Register callback for CM33 endpoint to receive messages from CM55
    /*cy_en_ipc_pipe_status_t status = Cy_IPC_Pipe_RegisterCallback(
        CM33_IPC_PIPE_EP_ADDR,
        &cm33_msg_callback,
        (uint32_t)CM33_IPC_PIPE_CLIENT_ID
    );

    if (status != CY_IPC_PIPE_SUCCESS) {
        mp_raise_msg(&mp_type_RuntimeError,  MP_ERROR_TEXT("IPC callback registration failed"));
    }*/

    mp_printf(&mp_plat_print, "IPC initialized successfully\r\n");

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_ipc_init_obj, machine_ipc_init);

/*******************************************************************************
* IPC.enable_core(core_id) - Enable target core
*******************************************************************************/
static mp_obj_t machine_ipc_enable_core(size_t n_args, const mp_obj_t *args) {
    // Default to CM55 (IPC_CORE_CM55) if no argument provided
    uint8_t core_id = (n_args > 1) ? mp_obj_get_int(args[1]) : IPC_CORE_CM55;

    // Currently only CM55 (IPC_CORE_CM55) is supported
    if (core_id == IPC_CORE_CM55) {
        if (cm55_enabled) {
            mp_printf(&mp_plat_print, "CM55 already enabled\r\n");
            return mp_const_true;
        }
        Cy_SysEnableCM55(MXCM55, CM55_APP_BOOT_ADDR, CM55_BOOT_WAIT_TIME_USEC);
        cm55_enabled = true;
        Cy_SysLib_Delay(CM33_APP_DELAY_MS);

        mp_printf(&mp_plat_print, "Enabling CM55 core at boot address: 0x%08X\r\n", CM55_APP_BOOT_ADDR);

        return mp_const_true;
    } else if (core_id == IPC_CORE_CM33) {
        mp_raise_ValueError(MP_ERROR_TEXT("Invalid operation: CM33 core is already active and cannot be re-enabled"));
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("Unsupported Core. Currently only CM55 is supported")); // ToDo: This should be generalised
    }

    return mp_const_false;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_ipc_enable_core_obj, 1, 2, machine_ipc_enable_core);

/*******************************************************************************
* IPC.send(cmd, value, client_id) - Send message to target core
*
* @param args[0] self - IPC object
* @param args[1] cmd - Command byte
* @param args[2] value - Optional data value (default: 0)
* @param args[3] client_id - Optional target client ID (default: 0)
*******************************************************************************/
static mp_obj_t machine_ipc_send(size_t n_args, const mp_obj_t *args) {
    // machine_ipc_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    // Parse arguments
    uint32_t cmd = mp_obj_get_int(args[1]);
    uint32_t value = (n_args > 2) ? mp_obj_get_int(args[2]) : 0;
    uint8_t client_id = (n_args > 3) ? mp_obj_get_int(args[3]) : CM55_IPC_PIPE_CLIENT_ID;

    // Use helper function to send message
    bool success = ipc_send_message(
        CM33_IPC_PIPE_EP_ADDR,       // Source endpoint
        CM55_IPC_PIPE_EP_ADDR,       // Target endpoint (based on self->target_core)
        client_id,                    // Target client
        (uint8_t)cmd,                 // Command
        value                         // Value
        );

    if (!success) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("IPC send failed"));
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_ipc_send_obj, 2, 4, machine_ipc_send);

/*******************************************************************************
* IPC.is_busy() - Check if channel is busy
*******************************************************************************/
static mp_obj_t machine_ipc_is_busy(mp_obj_t self_in) {
    bool locked = Cy_IPC_Drv_IsLockAcquired(Cy_IPC_Drv_GetIpcBaseAddress(CY_IPC_CHAN_CYPIPE_EP2));
    return mp_obj_new_bool(locked);
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_ipc_is_busy_obj, machine_ipc_is_busy);

/*******************************************************************************
* IPC.irq(handler, client_id) - Register Python callback for received messages
*
* @param self_in IPC object
* @param handler Python callback function or None to clear
* @param client_id Optional client ID (default: 0)
*******************************************************************************/
static mp_obj_t machine_ipc_irq(size_t n_args, const mp_obj_t *args) {
    mp_printf(&mp_plat_print, "IRQ\r\n");

    // machine_ipc_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_obj_t handler = args[1];
    uint8_t client_id = (n_args > 2) ? mp_obj_get_int(args[2]) : CM33_IPC_PIPE_CLIENT_ID;

    printf("[IRQ] client_id=%d\r\n", client_id);

    // Validate handler
    if (handler != mp_const_none && !mp_obj_is_callable(handler)) {
        mp_raise_ValueError(MP_ERROR_TEXT("callback must be callable or None"));
    }

    printf("[IRQ] Handler validated\r\n");

    bool success;

    if (handler == mp_const_none) {
        printf("[IRQ] Clearing callback\r\n");
        // Unregister callback
        success = ipc_unregister_client_callback(client_id);
        if (!success) {
            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Failed to unregister callback"));
        }
        printf("IPC callback cleared for client %d\r\n", client_id);
    } else {
        printf("[IRQ] Registering callback\r\n");
        // Register callback using helper function
        success = ipc_register_client_callback(
            client_id,
            handler,
            CM33_IPC_PIPE_EP_ADDR
            );

        printf("[IRQ] Registration result=%d\r\n", success);

        if (!success) {
            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("IPC callback registration failed"));
        }

        printf("IPC callback registered for client %d\r\n", client_id);
    }

    printf("[IRQ] Returning\r\n");
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_ipc_irq_obj, 2, 3, machine_ipc_irq);

/*******************************************************************************
* Class Definition
*******************************************************************************/
static const mp_rom_map_elem_t machine_ipc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_ipc_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_core), MP_ROM_PTR(&machine_ipc_enable_core_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&machine_ipc_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_busy), MP_ROM_PTR(&machine_ipc_is_busy_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq), MP_ROM_PTR(&machine_ipc_irq_obj) },

    // ToDo: API encapsulation for model fns - also similar to openamp?
    // { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&machine_ipc_start_obj) },
    // { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&machine_ipc_stop_obj) },

    // Core ID constants
    { MP_ROM_QSTR(MP_QSTR_CM33), MP_ROM_INT(IPC_CORE_CM33) },
    { MP_ROM_QSTR(MP_QSTR_CM55), MP_ROM_INT(IPC_CORE_CM55) },

    // LED command constants : #ToDo: Not needed once start stop enabled
    { MP_ROM_QSTR(MP_QSTR_CMD_LED_INIT), MP_ROM_INT(IPC_CMD_LED_INIT) },
    { MP_ROM_QSTR(MP_QSTR_CMD_LED_ON), MP_ROM_INT(IPC_CMD_LED_SET_ON) },
    { MP_ROM_QSTR(MP_QSTR_CMD_LED_OFF), MP_ROM_INT(IPC_CMD_LED_SET_OFF) },
};
static MP_DEFINE_CONST_DICT(machine_ipc_locals_dict, machine_ipc_locals_dict_table);

MP_REGISTER_ROOT_POINTER(struct _machine_ipc_obj_t *machine_ipc_obj[MICROPY_MAX_IPC_CH_INSTANCES]);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_ipc_type,
    MP_QSTR_IPC,
    MP_TYPE_FLAG_NONE,
    make_new, machine_ipc_make_new,
    locals_dict, &machine_ipc_locals_dict
    );

#endif // MULTI_CORE
