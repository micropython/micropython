/*******************************************************************************
 * File Name:   retarget_io_init.c
 *
 * Description: This file contains the initialization routine for the
 *              retarget-io middleware
 *
 * Related Document: See README.md
 *
 *******************************************************************************
* Copyright 2023-2025, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/

/*******************************************************************************
* Header Files
*******************************************************************************/
#include "retarget_io_init.h"

/*******************************************************************************
* Global Variables
*******************************************************************************/
/* For the RetargetIO (Debug UART) usage. */
static cy_stc_scb_uart_context_t DEBUG_UART_context;
mtb_hal_uart_t DEBUG_UART_hal_obj;

/* Retarget-io deepsleep callback parameters  */
#if (CY_CFG_PWR_SYS_IDLE_MODE == CY_CFG_PWR_MODE_DEEPSLEEP)

/* Context reference structure for Debug UART */
static mtb_syspm_uart_deepsleep_context_t retarget_io_syspm_ds_context =
{
    .uart_context = &DEBUG_UART_context,
    .async_context = NULL,
    .tx_pin =
    {
        .port = CYBSP_DEBUG_UART_TX_PORT,
        .pinNum = CYBSP_DEBUG_UART_TX_PIN,
        .hsiom = CYBSP_DEBUG_UART_TX_HSIOM
    },
    .rts_pin =
    {
        .port = DEBUG_UART_RTS_PORT,
        .pinNum = DEBUG_UART_RTS_PIN,
        .hsiom = HSIOM_SEL_GPIO
    }
};

/* SysPm callback parameter structure for Debug UART */
static cy_stc_syspm_callback_params_t retarget_io_syspm_cb_params =
{
    .context = &retarget_io_syspm_ds_context,
    .base = CYBSP_DEBUG_UART_HW
};

/* SysPm callback structure for Debug UART */
static cy_stc_syspm_callback_t retarget_io_syspm_cb =
{
    .callback = &mtb_syspm_scb_uart_deepsleep_callback,
    .skipMode = SYSPM_SKIP_MODE,
    .type = CY_SYSPM_DEEPSLEEP,
    .callbackParams = &retarget_io_syspm_cb_params,
    .prevItm = NULL,
    .nextItm = NULL,
    .order = SYSPM_CALLBACK_ORDER
};
#endif /* (CY_CFG_PWR_SYS_IDLE_MODE == CY_CFG_PWR_MODE_DEEPSLEEP) */

/*******************************************************************************
* Function Name: init_retarget_io
********************************************************************************
* Summary:
* User defined function to initialize the debug UART.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void init_retarget_io(void) {
    cy_rslt_t result = CY_RSLT_SUCCESS;

    /* Initialize the SCB UART */
    result = (cy_rslt_t)Cy_SCB_UART_Init(CYBSP_DEBUG_UART_HW,
        &CYBSP_DEBUG_UART_config,
        &DEBUG_UART_context);

    /* UART initialization failed. Stop program execution. */
    if (CY_RSLT_SUCCESS != result) {
        handle_app_error();
    }

    /* Enable the SCB UART */
    Cy_SCB_UART_Enable(CYBSP_DEBUG_UART_HW);

    result = mtb_hal_uart_setup(&DEBUG_UART_hal_obj,
        &CYBSP_DEBUG_UART_hal_config,
        &DEBUG_UART_context, NULL);

    /* UART setup failed. Stop program execution. */
    if (CY_RSLT_SUCCESS != result) {
        handle_app_error();
    }

    /* Initialize retarget-io to use the debug UART port. */
    result = cy_retarget_io_init(&DEBUG_UART_hal_obj);

    /* retarget-io initialization failed. Stop program execution. */
    if (CY_RSLT_SUCCESS != result) {
        handle_app_error();
    }

    #if (CY_CFG_PWR_SYS_IDLE_MODE == CY_CFG_PWR_MODE_DEEPSLEEP)
    /* UART SysPm callback registration for retarget-io */
    Cy_SysPm_RegisterCallback(&retarget_io_syspm_cb);
    #endif /* (CY_CFG_PWR_SYS_IDLE_MODE == CY_CFG_PWR_MODE_DEEPSLEEP) */
}

/* [] END OF FILE */
