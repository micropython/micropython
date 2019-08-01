/*
 * This file is part of the MicroPython project, http://micropython.org/
 */

/**
  ******************************************************************************
  * @file    USB_Device/CDC_Standalone/Src/usbd_conf.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    26-February-2014
  * @brief   This file implements the USB Device library callbacks and MSP
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

#include "usbd_core.h"
#include "py/obj.h"
#include "py/mphal.h"
#include "irq.h"
#include "usb.h"

#if MICROPY_HW_USB_FS
PCD_HandleTypeDef pcd_fs_handle;

/*******************************************************************************
                       PCD BSP Routines
*******************************************************************************/

/**
  * @brief  Initializes the PCD MSP.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd) {
    if (hpcd->Instance == USB) {
        mp_hal_pin_config(pin_A11, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, GPIO_AF10_USB);
        mp_hal_pin_config_speed(pin_A11, GPIO_SPEED_FREQ_HIGH);
        mp_hal_pin_config(pin_A12, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, GPIO_AF10_USB);
        mp_hal_pin_config_speed(pin_A12, GPIO_SPEED_FREQ_HIGH);

        // Enable USB FS Clocks
        __HAL_RCC_USB_CLK_ENABLE();

        // Configure and enable USB FS interrupt
        NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, IRQ_PRI_USB);
        HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
    }
}

/**
  * @brief  DeInitializes the PCD MSP.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_MspDeInit(PCD_HandleTypeDef *hpcd) {
    if (hpcd->Instance == USB) {
        __HAL_RCC_USB_CLK_DISABLE();
    }
}

/*******************************************************************************
                       LL Driver Callbacks (PCD -> USB Device Library)
*******************************************************************************/

/**
  * @brief  Setup stage callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd) {
    USBD_LL_SetupStage(hpcd->pData, (uint8_t *)hpcd->Setup);
}

/**
  * @brief  Data Out stage callback.
  * @param  hpcd: PCD handle
  * @param  epnum: Endpoint Number
  * @retval None
  */
void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
    USBD_LL_DataOutStage(hpcd->pData, epnum, hpcd->OUT_ep[epnum].xfer_buff);
}

/**
  * @brief  Data In stage callback.
  * @param  hpcd: PCD handle
  * @param  epnum: Endpoint Number
  * @retval None
  */
void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
    USBD_LL_DataInStage(hpcd->pData, epnum, hpcd->IN_ep[epnum].xfer_buff);
}

/**
  * @brief  SOF callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
/*
This is now handled by the USB CDC interface.
void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd)
{
  USBD_LL_SOF(hpcd->pData);
}
*/

/**
  * @brief  Reset callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd) {
    USBD_LL_SetSpeed(hpcd->pData, USBD_SPEED_FULL);

    // Reset Device
    USBD_LL_Reset(hpcd->pData);
}

/**
  * @brief  Suspend callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd) {
    USBD_LL_Suspend(hpcd->pData);
}

/**
  * @brief  Resume callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd) {
    USBD_LL_Resume(hpcd->pData);
}

/**
  * @brief  ISOC Out Incomplete callback.
  * @param  hpcd: PCD handle
  * @param  epnum: Endpoint Number
  * @retval None
  */
void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
    USBD_LL_IsoOUTIncomplete(hpcd->pData, epnum);
}

/**
  * @brief  ISOC In Incomplete callback.
  * @param  hpcd: PCD handle
  * @param  epnum: Endpoint Number
  * @retval None
  */
void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
    USBD_LL_IsoINIncomplete(hpcd->pData, epnum);
}

/**
  * @brief  Connect callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd) {
    USBD_LL_DevConnected(hpcd->pData);
}

/**
  * @brief  Disconnect callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd) {
    USBD_LL_DevDisconnected(hpcd->pData);
}

/*******************************************************************************
                       LL Driver Interface (USB Device Library --> PCD)
*******************************************************************************/

/**
  * @brief  Initializes the Low Level portion of the Device driver.
  * @param  pdev: Device handle
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_Init(USBD_HandleTypeDef *pdev, int high_speed) {
    if (pdev->id ==  0) {
        // Set LL Driver parameters
        pcd_fs_handle.Instance = USB;
        #if MICROPY_HW_USB_CDC_NUM == 2
        pcd_fs_handle.Init.dev_endpoints = 6;
        #else
        pcd_fs_handle.Init.dev_endpoints = 4;
        #endif
        pcd_fs_handle.Init.ep0_mps = 0x40; // 64字节
        pcd_fs_handle.Init.low_power_enable = 0;
        pcd_fs_handle.Init.phy_itface = PCD_PHY_EMBEDDED;
        pcd_fs_handle.Init.Sof_enable = 0;
        pcd_fs_handle.Init.speed = PCD_SPEED_FULL;
        pcd_fs_handle.Init.lpm_enable = DISABLE;
        pcd_fs_handle.Init.battery_charging_enable = DISABLE;

        // Link The driver to the stack
        pcd_fs_handle.pData = pdev;
        pdev->pData = &pcd_fs_handle;

        // Initialize LL Driver
        HAL_PCD_Init(&pcd_fs_handle);

		// 按照以下方式分配EP:
		// CDC0: IN:0x83  OUT-0x03   CMD:0x82
		// CDC1: IN:0x85  OUT-0x05   CMD:0x84
		// CDC2: IN:0x87  OUT-0x07   CMD:0x86
		// MSC: IN: 0x81  OUT-0x01

        // We have 512 byte in total to use here
        #if MICROPY_HW_USB_CDC_NUM == 2
		// TODO: 这里没有计算， 暂时不能用
		HAL_PCDEx_PMAConfig(&pcd_fs_handle, 0x00 , PCD_SNG_BUF, 0x18);  // EP0,          32B
		HAL_PCDEx_PMAConfig(&pcd_fs_handle, 0x80 , PCD_SNG_BUF, 0x28);  // MSC / HID     64B
		HAL_PCDEx_PMAConfig(&pcd_fs_handle, 0x01 , PCD_SNG_BUF, 0x68);  // CDC CMD       16B
		HAL_PCDEx_PMAConfig(&pcd_fs_handle, 0x80 , PCD_SNG_BUF, 0x78);  // CDC DATA      32B
		HAL_PCDEx_PMAConfig(&pcd_fs_handle, 0x02 , PCD_SNG_BUF, 0x98);  // CDC2 CMD      16B
		HAL_PCDEx_PMAConfig(&pcd_fs_handle, 0x82 , PCD_SNG_BUF, 0x108);  // CDC2 DATA     32B
        #else

		// 使用到EP3, 4个EP * 8 = 32B = 0x20, 用掉了224字节

		HAL_PCDEx_PMAConfig(&pcd_fs_handle, 0x00 , PCD_SNG_BUF, 0x20 );  // EP0,      16B
		HAL_PCDEx_PMAConfig(&pcd_fs_handle, 0x80,  PCD_SNG_BUF, 0x30 );  // 16B 需要空出来

		HAL_PCDEx_PMAConfig(&pcd_fs_handle, 0x01,  PCD_SNG_BUF, 0x40 );  // MSC-OUT,  32B
		HAL_PCDEx_PMAConfig(&pcd_fs_handle, 0x81,  PCD_SNG_BUF, 0x60 );  // MSC-IN,   32B
		
		HAL_PCDEx_PMAConfig(&pcd_fs_handle, 0x82,  PCD_SNG_BUF, 0x80 );  // CDC0-CMD, 8B
		HAL_PCDEx_PMAConfig(&pcd_fs_handle, 0x02,  PCD_SNG_BUF, 0x88 );  // 8B 需要空出来
		HAL_PCDEx_PMAConfig(&pcd_fs_handle, 0x03 , PCD_SNG_BUF, 0xa0);   // CDC0-IN, 32B
		HAL_PCDEx_PMAConfig(&pcd_fs_handle, 0x83 , PCD_SNG_BUF, 0xc0);   // CDC0-OUT, 32B
        #endif
    }

    return USBD_OK;
}

/**
  * @brief  De-Initializes the Low Level portion of the Device driver.
  * @param  pdev: Device handle
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_DeInit(USBD_HandleTypeDef *pdev) {
    HAL_PCD_DeInit(pdev->pData);
    return USBD_OK;
}

/**
  * @brief  Starts the Low Level portion of the Device driver.
  * @param  pdev: Device handle
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_Start(USBD_HandleTypeDef *pdev) {
    HAL_PCD_Start(pdev->pData);
    return USBD_OK;
}

/**
  * @brief  Stops the Low Level portion of the Device driver.
  * @param  pdev: Device handle
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_Stop(USBD_HandleTypeDef *pdev) {
    HAL_PCD_Stop(pdev->pData);
    return USBD_OK;
}

/**
  * @brief  Opens an endpoint of the Low Level Driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @param  ep_type: Endpoint Type
  * @param  ep_mps: Endpoint Max Packet Size
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_OpenEP(USBD_HandleTypeDef *pdev,
    uint8_t ep_addr, uint8_t ep_type, uint16_t ep_mps) {
    HAL_PCD_EP_Open(pdev->pData, ep_addr, ep_mps, ep_type);
    return USBD_OK;
}

/**
  * @brief  Closes an endpoint of the Low Level Driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_CloseEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr) {
    HAL_PCD_EP_Close(pdev->pData, ep_addr);
    return USBD_OK;
}

/**
  * @brief  Flushes an endpoint of the Low Level Driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_FlushEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr) {
    HAL_PCD_EP_Flush(pdev->pData, ep_addr);
    return USBD_OK;
}

/**
  * @brief  Sets a Stall condition on an endpoint of the Low Level Driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_StallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr) {
    HAL_PCD_EP_SetStall(pdev->pData, ep_addr);
    return USBD_OK;
}

/**
  * @brief  Clears a Stall condition on an endpoint of the Low Level Driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_ClearStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr) {
    HAL_PCD_EP_ClrStall(pdev->pData, ep_addr);
    return USBD_OK;
}

/**
  * @brief  Returns Stall condition.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @retval Stall (1: yes, 0: No)
  */
uint8_t USBD_LL_IsStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr) {
    PCD_HandleTypeDef *hpcd = pdev->pData;

    if ((ep_addr & 0x80) == 0x80) {
        return hpcd->IN_ep[ep_addr & 0x7F].is_stall;
    } else {
        return hpcd->OUT_ep[ep_addr & 0x7F].is_stall;
    }
}

/**
  * @brief  Assigns an USB address to the device
  * @param  pdev: Device handle
  * @param  dev_addr: USB address
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_SetUSBAddress(USBD_HandleTypeDef *pdev, uint8_t dev_addr) {
    HAL_PCD_SetAddress(pdev->pData, dev_addr);
    return USBD_OK;
}

/**
  * @brief  Transmits data over an endpoint
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @param  pbuf: Pointer to data to be sent
  * @param  size: Data size
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_Transmit(USBD_HandleTypeDef *pdev,
    uint8_t ep_addr, uint8_t *pbuf, uint16_t size) {
    HAL_PCD_EP_Transmit(pdev->pData, ep_addr, pbuf, size);
    return USBD_OK;
}

/**
  * @brief  Prepares an endpoint for reception
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @param  pbuf:pointer to data to be received
  * @param  size: data size
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_PrepareReceive(USBD_HandleTypeDef *pdev,
    uint8_t ep_addr, uint8_t *pbuf, uint16_t size) {
    HAL_PCD_EP_Receive(pdev->pData, ep_addr, pbuf, size);
    return USBD_OK;
}

/**
  * @brief  Returns the last transfered packet size.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @retval Recived Data Size
  */
uint32_t USBD_LL_GetRxDataSize(USBD_HandleTypeDef *pdev, uint8_t  ep_addr) {
    return HAL_PCD_EP_GetRxCount(pdev->pData, ep_addr);
}

/**
  * @brief  Delay routine for the USB Device Library
  * @param  Delay: Delay in ms
  * @retval None
  */
void USBD_LL_Delay(uint32_t Delay) {
    HAL_Delay(Delay);
}

#endif // MICROPY_HW_USB_FS || MICROPY_HW_USB_HS

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
