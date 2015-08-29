/***********************************************************************
* $Id:: mw_usbd_hw.h 331 2012-08-09 18:54:34Z usb10131                        $
*
* Project: USB device ROM Stack
*
* Description:
*     USB Hardware Function prototypes.
*
***********************************************************************
*   Copyright(C) 2011, NXP Semiconductor
*   All rights reserved.
*
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
**********************************************************************/
#ifndef __USBHW_H__
#define __USBHW_H__

#include "error.h"
#include "usbd.h"
#include "usbd_core.h"

/** \file
 *  \brief USB Hardware Function prototypes.
 *
 *  Definition of functions exported by ROM based Device Controller Driver (DCD).
 *
 */

/** \ingroup Group_USBD
 *  @defgroup USBD_HW USB Device Controller Driver 
 *  \section Sec_HWModDescription Module Description
 *  The Device Controller Driver Layer implements the routines to deal directly with the hardware. 
 */

/** \ingroup USBD_HW
*  USB Endpoint/class handler Callback Events. 
* 
*/
enum USBD_EVENT_T {
  USB_EVT_SETUP =1,    /**< 1   Setup Packet received */
  USB_EVT_OUT,         /**< 2   OUT Packet received */
  USB_EVT_IN,          /**< 3    IN Packet sent */
  USB_EVT_OUT_NAK,     /**< 4   OUT Packet - Not Acknowledged */
  USB_EVT_IN_NAK,      /**< 5    IN Packet - Not Acknowledged */
  USB_EVT_OUT_STALL,   /**< 6   OUT Packet - Stalled */
  USB_EVT_IN_STALL,    /**< 7    IN Packet - Stalled */
  USB_EVT_OUT_DMA_EOT, /**< 8   DMA OUT EP - End of Transfer */
  USB_EVT_IN_DMA_EOT,  /**< 9   DMA  IN EP - End of Transfer */
  USB_EVT_OUT_DMA_NDR, /**< 10  DMA OUT EP - New Descriptor Request */
  USB_EVT_IN_DMA_NDR,  /**< 11  DMA  IN EP - New Descriptor Request */
  USB_EVT_OUT_DMA_ERR, /**< 12  DMA OUT EP - Error */
  USB_EVT_IN_DMA_ERR,  /**< 13  DMA  IN EP - Error */
  USB_EVT_RESET,       /**< 14  Reset event recieved */
  USB_EVT_SOF,         /**< 15  Start of Frame event */
  USB_EVT_DEV_STATE,   /**< 16  Device status events */
  USB_EVT_DEV_ERROR   /**< 17  Device error events */
};

/** 
 *  \brief Hardware API functions structure.
 *  \ingroup USBD_HW
 *
 *  This module exposes functions which interact directly with USB device controller hardware.
 *
 */
typedef struct USBD_HW_API
{
  /** \fn uint32_t GetMemSize(USBD_API_INIT_PARAM_T* param)
   *  Function to determine the memory required by the USB device stack's DCD and core layers.
   * 
   *  This function is called by application layer before calling pUsbApi->hw->Init(), to allocate memory used
   *  by DCD and core layers. The application should allocate the memory which is accessible by USB
   *  controller/DMA controller. 
   *  \note Some memory areas are not accessible by all bus masters.
   *
   *  \param[in] param Structure containing USB device stack initialization parameters.
   *  \return Returns the required memory size in bytes.
   */
  uint32_t (*GetMemSize)(USBD_API_INIT_PARAM_T* param);
  
  /** \fn ErrorCode_t Init(USBD_HANDLE_T* phUsb, USB_CORE_DESCS_T* pDesc, USBD_API_INIT_PARAM_T* param)
   *  Function to initialize USB device stack's DCD and core layers.
   * 
   *  This function is called by application layer to initialize USB hardware and core layers. 
   *  On successful initialization the function returns a handle to USB device stack which should
   *  be passed to the rest of the functions.  
   *
   *  \param[in,out] phUsb Pointer to the USB device stack handle of type USBD_HANDLE_T. 
   *  \param[in]  pDesc Structure containing pointers to various descriptor arrays needed by the stack.
   *                    These descriptors are reported to USB host as part of enumerations process.
   *  \param[in]  param Structure containing USB device stack initialization parameters.
   *  \return Returns \ref ErrorCode_t type to indicate success or error condition.
   *          \retval LPC_OK(0) On success
   *          \retval ERR_USBD_BAD_MEM_BUF(0x0004000b) When insufficient memory buffer is passed or memory
   *                                             is not aligned on 2048 boundary.
   */
  ErrorCode_t (*Init)(USBD_HANDLE_T* phUsb, USB_CORE_DESCS_T* pDesc, USBD_API_INIT_PARAM_T* param);
  
  /** \fn void Connect(USBD_HANDLE_T hUsb, uint32_t con)
   *  Function to make USB device visible/invisible on the USB bus.
   *
   *  This function is called after the USB initialization. This function uses the soft connect
   *  feature to make the device visible on the USB bus. This function is called only after the
   *  application is ready to handle the USB data. The enumeration process is started by the
   *  host after the device detection. The driver handles the enumeration process according to
   *  the USB descriptors passed in the USB initialization function.
   *  
   *  \param[in] hUsb Handle to the USB device stack. 
   *  \param[in] con  States whether to connect (1) or to disconnect (0).
   *  \return Nothing.
   */
  void (*Connect)(USBD_HANDLE_T hUsb, uint32_t con);
  
  /** \fn void ISR(USBD_HANDLE_T hUsb)
   *  Function to USB device controller interrupt events.
   *  
   *  When the user application is active the interrupt handlers are mapped in the user flash
   *  space. The user application must provide an interrupt handler for the USB interrupt and
   *  call this function in the interrupt handler routine. The driver interrupt handler takes
   *  appropriate action according to the data received on the USB bus. 
   *  
   *  \param[in]  hUsb Handle to the USB device stack. 
   *  \return Nothing.
   */
  void (*ISR)(USBD_HANDLE_T hUsb);

  /** \fn void Reset(USBD_HANDLE_T hUsb)
   *  Function to Reset USB device stack and hardware controller.
   *  
   *  Reset USB device stack and hardware controller. Disables all endpoints except EP0.
   *  Clears all pending interrupts and resets endpoint transfer queues.
   *  This function is called internally by pUsbApi->hw->init() and from reset event.
   *  
   *  \param[in]  hUsb Handle to the USB device stack. 
   *  \return Nothing.
   */
  void  (*Reset)(USBD_HANDLE_T hUsb);
  
  /** \fn void ForceFullSpeed(USBD_HANDLE_T hUsb, uint32_t cfg)
   *  Function to force high speed USB device to operate in full speed mode.
   *
   *  This function is useful for testing the behavior of current device when connected
   *  to a full speed only hosts.
   *  
   *  \param[in] hUsb Handle to the USB device stack. 
   *  \param[in] cfg  When 1 - set force full-speed or 
   *                       0 - clear force full-speed.
   *  \return Nothing.
   */
  void  (*ForceFullSpeed )(USBD_HANDLE_T hUsb, uint32_t cfg);
  
  /** \fn void WakeUpCfg(USBD_HANDLE_T hUsb, uint32_t cfg)
   *  Function to configure USB device controller to wake-up host on remote events.
   *
   *  This function is called by application layer to configure the USB device controller 
   *  to wakeup on remote events. It is recommended to call this function from users's 
   *  USB_WakeUpCfg() callback routine registered with stack. 
   *  \note User's USB_WakeUpCfg() is registered with stack by setting the USB_WakeUpCfg member 
   *  of USBD_API_INIT_PARAM_T structure before calling pUsbApi->hw->Init() routine.
   *  Certain USB device controllers needed to keep some clocks always on to generate 
   *  resume signaling through pUsbApi->hw->WakeUp(). This hook is provided to support 
   *  such controllers. In most controllers cases this is an empty routine.
   *  
   *  \param[in] hUsb Handle to the USB device stack. 
   *  \param[in] cfg  When 1 - Configure controller to wake on remote events or 
   *                       0 - Configure controller not to wake on remote events.
   *  \return Nothing.
   */
  void  (*WakeUpCfg)(USBD_HANDLE_T hUsb, uint32_t  cfg);
  
  /** \fn void SetAddress(USBD_HANDLE_T hUsb, uint32_t adr)
   *  Function to set USB address assigned by host in device controller hardware.
   *
   *  This function is called automatically when USB_REQUEST_SET_ADDRESS request is received  
   *  by the stack from USB host.
   *  This interface is provided to users to invoke this function in other scenarios which are not 
   *  handle by current stack. In most user applications this function is not called directly.
   *  Also this function can be used by users who are selectively modifying the USB device stack's 
   *  standard handlers through callback interface exposed by the stack. 
   *  
   *  \param[in] hUsb Handle to the USB device stack. 
   *  \param[in] adr  USB bus Address to which the device controller should respond. Usually 
   *                  assigned by the USB host.
   *  \return Nothing.
   */
  void  (*SetAddress)(USBD_HANDLE_T hUsb, uint32_t adr);

  /** \fn void Configure(USBD_HANDLE_T hUsb, uint32_t cfg)
   *  Function to configure device controller hardware with selected configuration.
   *
   *  This function is called automatically when USB_REQUEST_SET_CONFIGURATION request is received  
   *  by the stack from USB host.
   *  This interface is provided to users to invoke this function in other scenarios which are not 
   *  handle by current stack. In most user applications this function is not called directly.
   *  Also this function can be used by users who are selectively modifying the USB device stack's 
   *  standard handlers through callback interface exposed by the stack. 
   *  
   *  \param[in] hUsb Handle to the USB device stack. 
   *  \param[in] cfg  Configuration index. 
   *  \return Nothing.
   */
  void  (*Configure)(USBD_HANDLE_T hUsb, uint32_t  cfg);

  /** \fn void ConfigEP(USBD_HANDLE_T hUsb, USB_ENDPOINT_DESCRIPTOR *pEPD)
   *  Function to configure USB Endpoint according to descriptor.
   *
   *  This function is called automatically when USB_REQUEST_SET_CONFIGURATION request is received  
   *  by the stack from USB host. All the endpoints associated with the selected configuration
   *  are configured.
   *  This interface is provided to users to invoke this function in other scenarios which are not 
   *  handle by current stack. In most user applications this function is not called directly.
   *  Also this function can be used by users who are selectively modifying the USB device stack's 
   *  standard handlers through callback interface exposed by the stack. 
   *  
   *  \param[in] hUsb Handle to the USB device stack. 
   *  \param[in] pEPD Endpoint descriptor structure defined in USB 2.0 specification.
   *  \return Nothing.
   */
  void  (*ConfigEP)(USBD_HANDLE_T hUsb, USB_ENDPOINT_DESCRIPTOR *pEPD);

  /** \fn void DirCtrlEP(USBD_HANDLE_T hUsb, uint32_t dir)
   *  Function to set direction for USB control endpoint EP0.
   *
   *  This function is called automatically by the stack on need basis.
   *  This interface is provided to users to invoke this function in other scenarios which are not 
   *  handle by current stack. In most user applications this function is not called directly.
   *  Also this function can be used by users who are selectively modifying the USB device stack's 
   *  standard handlers through callback interface exposed by the stack. 
   *  
   *  \param[in] hUsb Handle to the USB device stack. 
   *  \param[in] cfg  When 1 - Set EP0 in IN transfer mode 
   *                       0 - Set EP0 in OUT transfer mode
   *  \return Nothing.
   */
  void  (*DirCtrlEP)(USBD_HANDLE_T hUsb, uint32_t dir);

  /** \fn void EnableEP(USBD_HANDLE_T hUsb, uint32_t EPNum)
   *  Function to enable selected USB endpoint.
   *
   *  This function enables interrupts on selected endpoint.
   *  
   *  \param[in] hUsb Handle to the USB device stack. 
   *  \param[in] EPNum  Endpoint number as per USB specification. 
   *                    ie. An EP1_IN is represented by 0x81 number.
   *  \return Nothing.
   */
  void  (*EnableEP)(USBD_HANDLE_T hUsb, uint32_t EPNum);

  /** \fn void DisableEP(USBD_HANDLE_T hUsb, uint32_t EPNum)
   *  Function to disable selected USB endpoint.
   *
   *  This function disables interrupts on selected endpoint.
   *  
   *  \param[in] hUsb Handle to the USB device stack. 
   *  \param[in] EPNum  Endpoint number as per USB specification. 
   *                    ie. An EP1_IN is represented by 0x81 number.
   *  \return Nothing.
   */
  void  (*DisableEP)(USBD_HANDLE_T hUsb, uint32_t EPNum);

  /** \fn void ResetEP(USBD_HANDLE_T hUsb, uint32_t EPNum)
   *  Function to reset selected USB endpoint.
   *
   *  This function flushes the endpoint buffers and resets data toggle logic.
   *  
   *  \param[in] hUsb Handle to the USB device stack. 
   *  \param[in] EPNum  Endpoint number as per USB specification. 
   *                    ie. An EP1_IN is represented by 0x81 number.
   *  \return Nothing.
  */
  void  (*ResetEP)(USBD_HANDLE_T hUsb, uint32_t EPNum);

  /** \fn void SetStallEP(USBD_HANDLE_T hUsb, uint32_t EPNum)
   *  Function to STALL selected USB endpoint.
   *
   *  Generates STALL signaling for requested endpoint.
   *  
   *  \param[in] hUsb Handle to the USB device stack. 
   *  \param[in] EPNum  Endpoint number as per USB specification. 
   *                    ie. An EP1_IN is represented by 0x81 number.
   *  \return Nothing.
   */
  void  (*SetStallEP)(USBD_HANDLE_T hUsb, uint32_t EPNum);

  /** \fn void ClrStallEP(USBD_HANDLE_T hUsb, uint32_t EPNum)
   *  Function to clear STALL state for the requested endpoint.
   *
   *  This function clears STALL state for the requested endpoint.
   *  
   *  \param[in] hUsb Handle to the USB device stack. 
   *  \param[in] EPNum  Endpoint number as per USB specification. 
   *                    ie. An EP1_IN is represented by 0x81 number.
   *  \return Nothing.
   */
  void  (*ClrStallEP)(USBD_HANDLE_T hUsb, uint32_t EPNum);

  /** \fn ErrorCode_t SetTestMode(USBD_HANDLE_T hUsb, uint8_t mode)
   *  Function to set high speed USB device controller in requested test mode.
   *
   *  USB-IF requires the high speed device to be put in various test modes
   *  for electrical testing. This USB device stack calls this function whenever
   *  it receives USB_REQUEST_CLEAR_FEATURE request for USB_FEATURE_TEST_MODE. 
   *  Users can put the device in test mode by directly calling this function.
   *  Returns ERR_USBD_INVALID_REQ when device controller is full-speed only.
   *  
   *  \param[in] hUsb Handle to the USB device stack. 
   *  \param[in] mode  Test mode defined in USB 2.0 electrical testing specification.
   *  \return Returns \ref ErrorCode_t type to indicate success or error condition.
   *          \retval LPC_OK(0) - On success
   *          \retval ERR_USBD_INVALID_REQ(0x00040001) - Invalid test mode or 
   *                                             Device controller is full-speed only.
   */
  ErrorCode_t (*SetTestMode)(USBD_HANDLE_T hUsb, uint8_t mode); 

  /** \fn uint32_t ReadEP(USBD_HANDLE_T hUsb, uint32_t EPNum, uint8_t *pData)
   *  Function to read data received on the requested endpoint.
   *
   *  This function is called by USB stack and the application layer to read the data
   *  received on the requested endpoint.
   *  
   *  \param[in] hUsb Handle to the USB device stack. 
   *  \param[in] EPNum  Endpoint number as per USB specification. 
   *                    ie. An EP1_IN is represented by 0x81 number.
   *  \param[in,out] pData Pointer to the data buffer where data is to be copied. 
   *  \return Returns the number of bytes copied to the buffer.
   */
  uint32_t (*ReadEP)(USBD_HANDLE_T hUsb, uint32_t EPNum, uint8_t *pData);

  /** \fn uint32_t ReadReqEP(USBD_HANDLE_T hUsb, uint32_t EPNum, uint8_t *pData, uint32_t len)
   *  Function to queue read request on the specified endpoint.
   *
   *  This function is called by USB stack and the application layer to queue a read request
   *  on the specified endpoint.
   *  
   *  \param[in] hUsb Handle to the USB device stack. 
   *  \param[in] EPNum  Endpoint number as per USB specification. 
   *                    ie. An EP1_IN is represented by 0x81 number.
   *  \param[in,out] pData Pointer to the data buffer where data is to be copied. This buffer
   *                       address should be accessible by USB DMA master.
   *  \param[in] len  Length of the buffer passed. 
   *  \return Returns the length of the requested buffer.
   */
  uint32_t (*ReadReqEP)(USBD_HANDLE_T hUsb, uint32_t EPNum, uint8_t *pData, uint32_t len);

  /** \fn uint32_t ReadSetupPkt(USBD_HANDLE_T hUsb, uint32_t EPNum, uint32_t *pData)
   *  Function to read setup packet data received on the requested endpoint.
   *
   *  This function is called by USB stack and the application layer to read setup packet data
   *  received on the requested endpoint.
   *  
   *  \param[in] hUsb Handle to the USB device stack. 
   *  \param[in] EPNum  Endpoint number as per USB specification. 
   *                    ie. An EP0_IN is represented by 0x80 number.
   *  \param[in,out] pData Pointer to the data buffer where data is to be copied. 
   *  \return Returns the number of bytes copied to the buffer.
   */
  uint32_t (*ReadSetupPkt)(USBD_HANDLE_T hUsb, uint32_t EPNum, uint32_t *pData);

  /** \fn uint32_t WriteEP(USBD_HANDLE_T hUsb, uint32_t EPNum, uint8_t *pData, uint32_t cnt)
   *  Function to write data to be sent on the requested endpoint.
   *
   *  This function is called by USB stack and the application layer to send data
   *  on the requested endpoint.
   *  
   *  \param[in] hUsb Handle to the USB device stack. 
   *  \param[in] EPNum  Endpoint number as per USB specification. 
   *                    ie. An EP1_IN is represented by 0x81 number.
   *  \param[in] pData Pointer to the data buffer from where data is to be copied. 
   *  \param[in] cnt  Number of bytes to write. 
   *  \return Returns the number of bytes written.
   */
  uint32_t (*WriteEP)(USBD_HANDLE_T hUsb, uint32_t EPNum, uint8_t *pData, uint32_t cnt);

  /** \fn void WakeUp(USBD_HANDLE_T hUsb)
   *  Function to generate resume signaling on bus for remote host wakeup.
   *
   *  This function is called by application layer to remotely wakeup host controller 
   *  when system is in suspend state. Application should indicate this remote wakeup
   *  capability by setting USB_CONFIG_REMOTE_WAKEUP in bmAttributes of Configuration 
   *  Descriptor. Also this routine will generate resume signalling only if host
   *  enables USB_FEATURE_REMOTE_WAKEUP by sending SET_FEATURE request before suspending
   *  the bus.
   *  
   *  \param[in] hUsb Handle to the USB device stack. 
   *  \return Nothing.
   */
  void  (*WakeUp)(USBD_HANDLE_T hUsb);

  /** \fn void EnableEvent(USBD_HANDLE_T hUsb, uint32_t EPNum, uint32_t event_type, uint32_t enable)
   *  Function to enable/disable selected USB event.
   *
   *  This function enables interrupts on selected endpoint.
   *  
   *  \param[in] hUsb Handle to the USB device stack. 
   *  \param[in] EPNum  Endpoint number corresponding to the event.
   *                    ie. An EP1_IN is represented by 0x81 number. For device events 
   *                    set this param to 0x0. 
   *  \param[in] event_type  Type of endpoint event. See \ref USBD_EVENT_T for more details.
   *  \param[in] enable  1 - enable event, 0 - disable event.
   *  \return Returns \ref ErrorCode_t type to indicate success or error condition.
   *          \retval LPC_OK(0) - On success
   *          \retval ERR_USBD_INVALID_REQ(0x00040001) - Invalid event type.
   */
  ErrorCode_t  (*EnableEvent)(USBD_HANDLE_T hUsb, uint32_t EPNum, uint32_t event_type, uint32_t enable);

} USBD_HW_API_T;

/*-----------------------------------------------------------------------------
 *  Private functions & structures prototypes used by stack internally
 *-----------------------------------------------------------------------------*/
/** @cond  DIRECT_API */

/* Driver functions */
uint32_t hwUSB_GetMemSize(USBD_API_INIT_PARAM_T* param);
ErrorCode_t hwUSB_Init(USBD_HANDLE_T* phUsb, USB_CORE_DESCS_T* pDesc, USBD_API_INIT_PARAM_T* param);
void hwUSB_Connect(USBD_HANDLE_T hUsb, uint32_t con);
void hwUSB_ISR(USBD_HANDLE_T hUsb);

/* USB Hardware Functions */
extern void  hwUSB_Reset(USBD_HANDLE_T hUsb);
extern void  hwUSB_ForceFullSpeed (USBD_HANDLE_T hUsb, uint32_t con);
extern void  hwUSB_WakeUpCfg(USBD_HANDLE_T hUsb, uint32_t  cfg);
extern void  hwUSB_SetAddress(USBD_HANDLE_T hUsb, uint32_t adr);
extern void  hwUSB_Configure(USBD_HANDLE_T hUsb, uint32_t  cfg);
extern void  hwUSB_ConfigEP(USBD_HANDLE_T hUsb, USB_ENDPOINT_DESCRIPTOR *pEPD);
extern void  hwUSB_DirCtrlEP(USBD_HANDLE_T hUsb, uint32_t dir);
extern void  hwUSB_EnableEP(USBD_HANDLE_T hUsb, uint32_t EPNum);
extern void  hwUSB_DisableEP(USBD_HANDLE_T hUsb, uint32_t EPNum);
extern void  hwUSB_ResetEP(USBD_HANDLE_T hUsb, uint32_t EPNum);
extern void  hwUSB_SetStallEP(USBD_HANDLE_T hUsb, uint32_t EPNum);
extern void  hwUSB_ClrStallEP(USBD_HANDLE_T hUsb, uint32_t EPNum);
extern ErrorCode_t hwUSB_SetTestMode(USBD_HANDLE_T hUsb, uint8_t mode); /* for FS only devices return ERR_USBD_INVALID_REQ */
extern uint32_t hwUSB_ReadEP(USBD_HANDLE_T hUsb, uint32_t EPNum, uint8_t *pData);
extern uint32_t hwUSB_ReadReqEP(USBD_HANDLE_T hUsb, uint32_t EPNum, uint8_t *pData, uint32_t len);
extern uint32_t hwUSB_ReadSetupPkt(USBD_HANDLE_T hUsb, uint32_t, uint32_t *);
extern uint32_t hwUSB_WriteEP(USBD_HANDLE_T hUsb, uint32_t EPNum, uint8_t *pData, uint32_t cnt);

/* generate resume signaling on the bus */
extern void  hwUSB_WakeUp(USBD_HANDLE_T hUsb);
extern ErrorCode_t  hwUSB_EnableEvent(USBD_HANDLE_T hUsb, uint32_t EPNum, uint32_t event_type, uint32_t enable);
/* TODO implement following routines
- function to program TD and queue them to ep Qh
*/

/** @endcond */


#endif  /* __USBHW_H__ */
