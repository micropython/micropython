/*******************************************************************************
* File Name        : main.c
*
* Description      : This source file contains the code for setting up IPC
*                    communication for CM55 CPU.
*
* Related Document : See README.md
*
********************************************************************************
* (c) 2025, Infineon Technologies AG, or an affiliate of Infineon
* Technologies AG. All rights reserved.
* This software, associated documentation and materials ("Software") is
* owned by Infineon Technologies AG or one of its affiliates ("Infineon")
* and is protected by and subject to worldwide patent protection, worldwide
* copyright laws, and international treaty provisions. Therefore, you may use
* this Software only as provided in the license agreement accompanying the
* software package from which you obtained this Software. If no license
* agreement applies, then any use, reproduction, modification, translation, or
* compilation of this Software is prohibited without the express written
* permission of Infineon.
*
* Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
* IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
* THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
* SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
* Infineon reserves the right to make changes to the Software without notice.
* You are responsible for properly designing, programming, and testing the
* functionality and safety of your intended application of the Software, as
* well as complying with any legal requirements related to its use. Infineon
* does not guarantee that the Software will be free from intrusion, data theft
* or loss, or other breaches ("Security Breaches"), and Infineon shall have
* no liability arising out of any Security Breaches. Unless otherwise
* explicitly approved by Infineon, the Software may not be used in any
* application where a failure of the Product or any consequences of the use
* thereof can reasonably be expected to result in personal injury.
*******************************************************************************/

#include "ipc_communication.h"


/*******************************************************************************
* Global Variable(s)
*******************************************************************************/
/* Create an array of endpoint structures */
static cy_stc_ipc_pipe_ep_t cm55_ipc_pipe_array[CY_IPC_MAX_ENDPOINTS];

/* CB Array for EP2 */
static cy_ipc_pipe_callback_ptr_t ep2_cb_array[CY_IPC_CYPIPE_CLIENT_CNT];


/*******************************************************************************
* Function Name: Cy_SysIpcPipeIsrCm55
********************************************************************************
* Summary:
* This is the interrupt service routine for the system pipe.
*
* Parameters:
*  none
*
* Return :
*  void
*
*******************************************************************************/
void Cy_SysIpcPipeIsrCm55(void) {
    Cy_IPC_Pipe_ExecuteCallback(CM55_IPC_PIPE_EP_ADDR);
}


/*******************************************************************************
* Function Name: cm55_ipc_communication_setup
********************************************************************************
* Summary:
*  This function configures IPC Pipe for CM55 to CM33 communication.
*
* Parameters:
*  none
*
* Return :
*  void
*
*******************************************************************************/
void cm55_ipc_communication_setup(void) {

    /* IPC pipe endpoint-1 and endpoint-2. CM55 <--> CM33 */
    static const cy_stc_ipc_pipe_config_t cm55_ipc_pipe_config =
    {
        /* receiver endpoint CM55 */
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
        /* sender endpoint CM33 */
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
        .endpointClientsCount = CY_IPC_CYPIPE_CLIENT_CNT,
        .endpointsCallbacksArray = ep2_cb_array,
        .userPipeIsrHandler = &Cy_SysIpcPipeIsrCm55
    };

    Cy_IPC_Pipe_Config(cm55_ipc_pipe_array);

    Cy_IPC_Pipe_Init(&cm55_ipc_pipe_config);
}
