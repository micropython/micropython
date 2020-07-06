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

#if MICROPY_HW_USB_FS || MICROPY_HW_USB_HS

#if MICROPY_HW_USB_FS
PCD_HandleTypeDef pcd_fs_handle;
#endif
#if MICROPY_HW_USB_HS
PCD_HandleTypeDef pcd_hs_handle;
#endif

#if !MICROPY_HW_USB_IS_MULTI_OTG
// The MCU has a single USB device-only instance
#define USB_OTG_FS USB
#endif

/*******************************************************************************
                       PCD BSP Routines
*******************************************************************************/

/**
  * @brief  Initializes the PCD MSP.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd) {
    if (hpcd->Instance == USB_OTG_FS) {
        #if defined(STM32H7)
        const uint32_t otg_alt = GPIO_AF10_OTG1_FS;
        #elif defined(STM32L0)
        const uint32_t otg_alt = GPIO_AF0_USB;
        #elif defined(STM32L432xx)
        const uint32_t otg_alt = GPIO_AF10_USB_FS;
        #elif defined(STM32WB)
        const uint32_t otg_alt = GPIO_AF10_USB;
        #else
        const uint32_t otg_alt = GPIO_AF10_OTG_FS;
        #endif

        mp_hal_pin_config(pin_A11, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, otg_alt);
        mp_hal_pin_config_speed(pin_A11, GPIO_SPEED_FREQ_VERY_HIGH);
        mp_hal_pin_config(pin_A12, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, otg_alt);
        mp_hal_pin_config_speed(pin_A12, GPIO_SPEED_FREQ_VERY_HIGH);

        #if defined(MICROPY_HW_USB_VBUS_DETECT_PIN)
        // USB VBUS detect pin is always A9
        mp_hal_pin_config(MICROPY_HW_USB_VBUS_DETECT_PIN, MP_HAL_PIN_MODE_INPUT, MP_HAL_PIN_PULL_NONE, 0);
        #endif

        #if defined(MICROPY_HW_USB_OTG_ID_PIN)
        // USB ID pin is always A10
        mp_hal_pin_config(MICROPY_HW_USB_OTG_ID_PIN, MP_HAL_PIN_MODE_ALT_OPEN_DRAIN, MP_HAL_PIN_PULL_UP, otg_alt);
        #endif

        #if defined(STM32H7)
        // Keep USB clock running during sleep or else __WFI() will disable the USB
        __HAL_RCC_USB2_OTG_FS_CLK_SLEEP_ENABLE();
        __HAL_RCC_USB2_OTG_FS_ULPI_CLK_SLEEP_DISABLE();
        #endif

        // Enable USB FS Clocks
        #if !MICROPY_HW_USB_IS_MULTI_OTG
        __HAL_RCC_USB_CLK_ENABLE();
        #else
        __USB_OTG_FS_CLK_ENABLE();
        #endif

        #if defined(STM32L4)
        // Enable VDDUSB
        if (__HAL_RCC_PWR_IS_CLK_DISABLED()) {
            __HAL_RCC_PWR_CLK_ENABLE();
            HAL_PWREx_EnableVddUSB();
            __HAL_RCC_PWR_CLK_DISABLE();
        } else {
            HAL_PWREx_EnableVddUSB();
        }
        #endif

        // Configure and enable USB FS interrupt
        #if defined(STM32L0)
        NVIC_SetPriority(USB_IRQn, IRQ_PRI_OTG_FS);
        HAL_NVIC_EnableIRQ(USB_IRQn);
        #elif defined(STM32L432xx)
        NVIC_SetPriority(USB_FS_IRQn, IRQ_PRI_OTG_FS);
        HAL_NVIC_EnableIRQ(USB_FS_IRQn);
        #elif defined(STM32WB)
        NVIC_SetPriority(USB_LP_IRQn, IRQ_PRI_OTG_FS);
        HAL_NVIC_EnableIRQ(USB_LP_IRQn);
        #else
        NVIC_SetPriority(OTG_FS_IRQn, IRQ_PRI_OTG_FS);
        HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
        #endif
    }
    #if MICROPY_HW_USB_HS
    else if (hpcd->Instance == USB_OTG_HS) {
        #if MICROPY_HW_USB_HS_IN_FS

        #if defined(STM32H7)
        const uint32_t otg_alt = GPIO_AF12_OTG2_FS;
        #else
        const uint32_t otg_alt = GPIO_AF12_OTG_HS_FS;
        #endif

        // Configure USB FS GPIOs
        mp_hal_pin_config(pin_B14, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, otg_alt);
        mp_hal_pin_config_speed(pin_B14, GPIO_SPEED_FREQ_VERY_HIGH);
        mp_hal_pin_config(pin_B15, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, otg_alt);
        mp_hal_pin_config_speed(pin_B15, GPIO_SPEED_FREQ_VERY_HIGH);

        #if defined(MICROPY_HW_USB_VBUS_DETECT_PIN)
        // Configure VBUS Pin
        mp_hal_pin_config(MICROPY_HW_USB_VBUS_DETECT_PIN, MP_HAL_PIN_MODE_INPUT, MP_HAL_PIN_PULL_NONE, 0);
        #endif

        #if defined(MICROPY_HW_USB_OTG_ID_PIN)
        // Configure ID pin
        mp_hal_pin_config(MICROPY_HW_USB_OTG_ID_PIN, MP_HAL_PIN_MODE_ALT_OPEN_DRAIN, MP_HAL_PIN_PULL_UP, otg_alt);
        #endif

        // Enable calling WFI and correct function of the embedded USB_FS_IN_HS phy
        __HAL_RCC_USB_OTG_HS_ULPI_CLK_SLEEP_DISABLE();
        __HAL_RCC_USB_OTG_HS_CLK_SLEEP_ENABLE();

        // Enable USB HS Clocks

        #if defined(STM32F723xx) || defined(STM32F733xx)
        // Needs to remain awake during sleep or else __WFI() will disable the USB
        __HAL_RCC_USB_OTG_HS_ULPI_CLK_SLEEP_ENABLE();
        __HAL_RCC_OTGPHYC_CLK_ENABLE();
        __HAL_RCC_USB_OTG_HS_ULPI_CLK_ENABLE();
        #endif

        __HAL_RCC_USB_OTG_HS_CLK_ENABLE();

        #else // !MICROPY_HW_USB_HS_IN_FS

        // Configure USB HS GPIOs
        static const mp_hal_pin_obj_t usb_pins[] = {
            pin_A5, pin_C0, MICROPY_HW_USB_HS_ULPI_NXT, MICROPY_HW_USB_HS_ULPI_DIR, // CLK, STP, NXT, DIR
            pin_A3, pin_B0, pin_B1, pin_B5, pin_B10, pin_B11, pin_B12, pin_B13, // D0-D7
        };
        for (size_t i = 0; i < MP_ARRAY_SIZE(usb_pins); ++i) {
            mp_hal_pin_config(usb_pins[i], MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, GPIO_AF10_OTG_HS);
            mp_hal_pin_config_speed(usb_pins[i], GPIO_SPEED_FREQ_VERY_HIGH);
        }

        // Enable USB HS Clocks
        __HAL_RCC_USB_OTG_HS_CLK_ENABLE();
        __HAL_RCC_USB_OTG_HS_CLK_SLEEP_ENABLE();
        __HAL_RCC_USB_OTG_HS_ULPI_CLK_ENABLE();
        __HAL_RCC_USB_OTG_HS_ULPI_CLK_SLEEP_ENABLE();

        #endif // !MICROPY_HW_USB_HS_IN_FS

        // Configure and enable USB HS interrupt
        NVIC_SetPriority(OTG_HS_IRQn, IRQ_PRI_OTG_HS);
        HAL_NVIC_EnableIRQ(OTG_HS_IRQn);
    }
    #endif // MICROPY_HW_USB_HS
}

/**
  * @brief  DeInitializes the PCD MSP.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_MspDeInit(PCD_HandleTypeDef *hpcd) {
    #if !MICROPY_HW_USB_IS_MULTI_OTG
    __HAL_RCC_USB_CLK_DISABLE();
    #else

    if (hpcd->Instance == USB_OTG_FS) {
        /* Disable USB FS Clocks */
        __USB_OTG_FS_CLK_DISABLE();
        __SYSCFG_CLK_DISABLE();
    }
    #if MICROPY_HW_USB_HS
    else if (hpcd->Instance == USB_OTG_HS) {
        /* Disable USB FS Clocks */
        __USB_OTG_HS_CLK_DISABLE();
        __SYSCFG_CLK_DISABLE();
    }
    #endif

    #endif
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
    USBD_SpeedTypeDef speed = USBD_SPEED_FULL;

    // Set USB Current Speed
    switch (hpcd->Init.speed) {
        #if defined(PCD_SPEED_HIGH)
        case PCD_SPEED_HIGH:
            speed = USBD_SPEED_HIGH;
            break;
        #endif

        case PCD_SPEED_FULL:
            speed = USBD_SPEED_FULL;
            break;

        default:
            speed = USBD_SPEED_FULL;
            break;
    }
    USBD_LL_SetSpeed(hpcd->pData, speed);

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
USBD_StatusTypeDef USBD_LL_Init(USBD_HandleTypeDef *pdev, int high_speed, const uint8_t *fifo_size) {
    #if MICROPY_HW_USB_FS
    if (pdev->id == USB_PHY_FS_ID) {
        #if defined(STM32WB)
        PWR->CR2 |= PWR_CR2_USV; // USB supply is valid
        #endif

        // Set LL Driver parameters
        pcd_fs_handle.Instance = USB_OTG_FS;
        #if MICROPY_HW_USB_CDC_NUM == 2
        pcd_fs_handle.Init.dev_endpoints = 6;
        #else
        pcd_fs_handle.Init.dev_endpoints = 4;
        #endif
        pcd_fs_handle.Init.ep0_mps = 0x40;
        pcd_fs_handle.Init.low_power_enable = 0;
        pcd_fs_handle.Init.phy_itface = PCD_PHY_EMBEDDED;
        pcd_fs_handle.Init.Sof_enable = 0;
        pcd_fs_handle.Init.speed = PCD_SPEED_FULL;
        #if defined(STM32L4)
        pcd_fs_handle.Init.lpm_enable = DISABLE;
        pcd_fs_handle.Init.battery_charging_enable = DISABLE;
        #endif
        #if MICROPY_HW_USB_IS_MULTI_OTG
        pcd_fs_handle.Init.use_dedicated_ep1 = 0;
        pcd_fs_handle.Init.dma_enable = 0;
        #if !defined(MICROPY_HW_USB_VBUS_DETECT_PIN)
        pcd_fs_handle.Init.vbus_sensing_enable = 0; // No VBUS Sensing on USB0
        #else
        pcd_fs_handle.Init.vbus_sensing_enable = 1;
        #endif
        #endif

        // Link The driver to the stack
        pcd_fs_handle.pData = pdev;
        pdev->pData = &pcd_fs_handle;

        // Initialize LL Driver
        HAL_PCD_Init(&pcd_fs_handle);

        // Set FIFO buffer sizes
        #if !MICROPY_HW_USB_IS_MULTI_OTG
        uint32_t fifo_offset = USBD_PMA_RESERVE; // need to reserve some data at start of FIFO
        for (size_t i = 0; i < USBD_PMA_NUM_FIFO; ++i) {
            uint16_t ep_addr = ((i & 1) * 0x80) | (i >> 1);
            HAL_PCDEx_PMAConfig(&pcd_fs_handle, ep_addr, PCD_SNG_BUF, fifo_offset);
            fifo_offset += fifo_size[i] * 4;
        }
        #else
        HAL_PCD_SetRxFiFo(&pcd_fs_handle, fifo_size[0] * 4);
        for (size_t i = 0; i < USBD_FS_NUM_TX_FIFO; ++i) {
            HAL_PCD_SetTxFiFo(&pcd_fs_handle, i, fifo_size[1 + i] * 4);
        }
        #endif
    }
    #endif
    #if MICROPY_HW_USB_HS
    if (pdev->id == USB_PHY_HS_ID) {
        // Set LL Driver parameters
        pcd_hs_handle.Instance = USB_OTG_HS;
        #if MICROPY_HW_USB_CDC_NUM == 3
        pcd_hs_handle.Init.dev_endpoints = 8;
        #else
        pcd_hs_handle.Init.dev_endpoints = 6;
        #endif
        pcd_hs_handle.Init.use_dedicated_ep1 = 0;
        pcd_hs_handle.Init.ep0_mps = 0x40;
        pcd_hs_handle.Init.dma_enable = 0;
        pcd_hs_handle.Init.low_power_enable = 0;
        pcd_hs_handle.Init.lpm_enable = DISABLE;
        pcd_hs_handle.Init.battery_charging_enable = DISABLE;
        pcd_hs_handle.Init.Sof_enable = 0;
        pcd_hs_handle.Init.use_external_vbus = 0;

        #if !defined(MICROPY_HW_USB_VBUS_DETECT_PIN)
        pcd_hs_handle.Init.vbus_sensing_enable = 0; // No VBUS Sensing on USB0
        #else
        pcd_hs_handle.Init.vbus_sensing_enable = 1;
        #endif

        #if MICROPY_HW_USB_HS_IN_FS

        #if defined(STM32F723xx) || defined(STM32F733xx)
        pcd_hs_handle.Init.phy_itface = USB_OTG_HS_EMBEDDED_PHY;
        #else
        pcd_hs_handle.Init.phy_itface = PCD_PHY_EMBEDDED;
        #endif

        if (high_speed) {
            pcd_hs_handle.Init.speed = PCD_SPEED_HIGH;
        } else {
            pcd_hs_handle.Init.speed = PCD_SPEED_HIGH_IN_FULL;
        }

        #else

        // USB HS with external PHY
        pcd_hs_handle.Init.phy_itface = PCD_PHY_ULPI;
        pcd_hs_handle.Init.speed = PCD_SPEED_HIGH;

        #endif

        // Link The driver to the stack
        pcd_hs_handle.pData = pdev;
        pdev->pData = &pcd_hs_handle;

        // Initialize LL Driver
        HAL_PCD_Init(&pcd_hs_handle);

        // Set FIFO buffer sizes
        fifo_size += USBD_FS_NUM_FIFO; // skip over FS FIFO size values
        HAL_PCD_SetRxFiFo(&pcd_hs_handle, fifo_size[0] * 4);
        for (size_t i = 0; i < USBD_HS_NUM_TX_FIFO; ++i) {
            HAL_PCD_SetTxFiFo(&pcd_hs_handle, i, fifo_size[1 + i] * 4);
        }
    }
    #endif  // MICROPY_HW_USB_HS

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
uint32_t USBD_LL_GetRxDataSize(USBD_HandleTypeDef *pdev, uint8_t ep_addr) {
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
