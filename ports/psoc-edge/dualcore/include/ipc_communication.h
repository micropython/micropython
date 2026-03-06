/*******************************************************************************
* File Name        : ipc_communication.h
*
* Description      : This file contains the headers and structures
*                    used to setup the IPC Pipes between CM33 and CM55
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

#ifndef SOURCE_IPC_COMMUNICATION_H
#define SOURCE_IPC_COMMUNICATION_H

/*******************************************************************************
* Header Files
*******************************************************************************/
#include "cybsp.h"
#include "cy_pdl.h"
#include "cy_ipc_pipe.h"

/*******************************************************************************
* Macros
*******************************************************************************/
#define CY_IPC_MAX_ENDPOINTS            (5UL)
#define CY_IPC_CYPIPE_CLIENT_CNT        (8UL)

#define CY_IPC_CHAN_CYPIPE_EP1          (4UL)
#define CY_IPC_INTR_CYPIPE_EP1          (4UL)
#define CY_IPC_CHAN_CYPIPE_EP2          (15UL)
#define CY_IPC_INTR_CYPIPE_EP2          (5UL)

/* IPC Pipe Endpoint-1 config */
#define CY_IPC_CYPIPE_CHAN_MASK_EP1     CY_IPC_CH_MASK(CY_IPC_CHAN_CYPIPE_EP1)
#define CY_IPC_CYPIPE_INTR_MASK_EP1     CY_IPC_INTR_MASK(CY_IPC_INTR_CYPIPE_EP1)
#define CY_IPC_INTR_CYPIPE_PRIOR_EP1    (1UL)
#define CY_IPC_INTR_CYPIPE_MUX_EP1      (CY_IPC0_INTR_MUX(CY_IPC_INTR_CYPIPE_EP1))
#define CM33_IPC_PIPE_EP_ADDR           (1UL)
#define CM33_IPC_PIPE_CLIENT_ID         (3UL)

/* IPC Pipe Endpoint-2 config */
#define CY_IPC_CYPIPE_CHAN_MASK_EP2     CY_IPC_CH_MASK(CY_IPC_CHAN_CYPIPE_EP2)
#define CY_IPC_CYPIPE_INTR_MASK_EP2     CY_IPC_INTR_MASK(CY_IPC_INTR_CYPIPE_EP2)
#define CY_IPC_INTR_CYPIPE_PRIOR_EP2    (1UL)
#define CY_IPC_INTR_CYPIPE_MUX_EP2      (CY_IPC0_INTR_MUX(CY_IPC_INTR_CYPIPE_EP2))
#define CM55_IPC_PIPE_EP_ADDR           (2UL)
#define CM55_IPC_PIPE_CLIENT_ID         (5UL)

/* Combined Interrupt Mask */
#define CY_IPC_CYPIPE_INTR_MASK         (CY_IPC_CYPIPE_CHAN_MASK_EP1 | CY_IPC_CYPIPE_CHAN_MASK_EP2)

/*******************************************************************************
* Enumeration
*******************************************************************************/
/* IPC Message structure */
/* Pointer to this structure will be shared through IPC Pipe */


/*******************************************************************************
* Function prototypes
*******************************************************************************/
void cm33_ipc_communication_setup(void);
void cm33_ipc_pipe_isr(void);
void cm55_ipc_communication_setup(void);
void cm55_ipc_pipe_isr(void);

#endif /* SOURCE_IPC_COMMUNICATION_H */
