/***********************************************************************
* $Id:: mw_usbd_core.h 331 2012-08-09 18:54:34Z usb10131                      $
*
* Project: USB device ROM Stack
*
* Description:
*     USB core controller structure definitions and function prototypes.
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
#ifndef __MW_USBD_CORE_H__
#define __MW_USBD_CORE_H__

#include "error.h"
#include "usbd.h"
#include "app_usbd_cfg.h"

/** \file
 *  \brief ROM API for USB device stack.
 *
 *  Definition of functions exported by core layer of ROM based USB device stack.
 *
 */

/** \ingroup Group_USBD
 *  @defgroup USBD_Core USB Core Layer
 *  \section Sec_CoreModDescription Module Description
 *  The USB Core Layer implements the device abstraction defined in the <em> Universal Serial Bus Specification, </em>
 *  for applications to interact with the USB device interface on the device. The software in this layer responds to 
 *  standard requests and returns standard descriptors. In current stack the Init() routine part of 
 *  \ref USBD_HW_API_T structure initializes both hardware layer and core layer.
 */


/* function pointer types */

/** \ingroup USBD_Core 
 *  \typedef USB_CB_T
 *  \brief USB device stack's event callback function type.
 *
 *  The USB device stack exposes several event triggers through callback to application layer. The
 *  application layer can register methods to be called when such USB event happens.
 *  
 *  \param[in] hUsb Handle to the USB device stack. 
 *  \return The call back should returns \ref ErrorCode_t type to indicate success or error condition.
 *          \retval LPC_OK On success
 *          \retval ERR_USBD_UNHANDLED  Event is not handled hence pass the event to next in line. 
 *          \retval ERR_USBD_xxx  Other error conditions. 
 *                                             
 */
typedef ErrorCode_t (*USB_CB_T) (USBD_HANDLE_T hUsb);

/** \ingroup USBD_Core 
 *  \typedef USB_PARAM_CB_T
 *  \brief USB device stack's event callback function type.
 *
 *  The USB device stack exposes several event triggers through callback to application layer. The
 *  application layer can register methods to be called when such USB event happens.
 *  
 *  \param[in] hUsb Handle to the USB device stack. 
 *  \param[in] param1 Extra information related to the event. 
 *  \return The call back should returns \ref ErrorCode_t type to indicate success or error condition.
 *          \retval LPC_OK On success
 *          \retval ERR_USBD_UNHANDLED  Event is not handled hence pass the event to next in line. 
 *          \retval ERR_USBD_xxx  For other error conditions. 
 *                                             
 */
typedef ErrorCode_t (*USB_PARAM_CB_T) (USBD_HANDLE_T hUsb, uint32_t param1);

/** \ingroup USBD_Core
 *  \typedef USB_EP_HANDLER_T
 *  \brief USBD setup request and endpoint event handler type.
 *
 *  The application layer should define the custom class's EP0 handler with function signature. 
 *  The stack calls all the registered class handlers on any EP0 event before going through default 
 *  handling of the event. This gives the class handlers to implement class specific request handlers
 *  and also to override the default stack handling for a particular event targeted to the interface.
 *  If an event is not handled by the callback the function should return ERR_USBD_UNHANDLED. For all
 *  other return codes the stack assumes that callback has taken care of the event and hence will not
 *  process the event any further and issues a STALL condition on EP0 indicating error to the host.
 *  \n
 *  For endpoint interrupt handler the return value is ignored by the stack.
 *  \n
 *  \param[in] hUsb Handle to the USB device stack. 
 *  \param[in] data Pointer to the data which will be passed when callback function is called by the stack. 
 *  \param[in] event  Type of endpoint event. See \ref USBD_EVENT_T for more details.
 *  \return The call back should returns \ref ErrorCode_t type to indicate success or error condition.
 *          \retval LPC_OK On success.
 *          \retval ERR_USBD_UNHANDLED  Event is not handled hence pass the event to next in line. 
 *          \retval ERR_USBD_xxx  For other error conditions. 
 *                                             
 */
typedef ErrorCode_t (*USB_EP_HANDLER_T)(USBD_HANDLE_T hUsb, void* data, uint32_t event);


/** \ingroup USBD_Core 
 *  \brief USB descriptors data structure.
 *  \ingroup USBD_Core
 *
 *  \details  This structure is used as part of USB device stack initialization
 *  parameter structure \ref USBD_API_INIT_PARAM_T. This structure contains
 *  pointers to various descriptor arrays needed by the stack. These descriptors
 *  are reported to USB host as part of enumerations process.
 *
 *  \note All descriptor pointers assigned in this structure should be on 4 byte
 *  aligned address boundary.
 */
typedef struct _USB_CORE_DESCS_T
{
  uint8_t *device_desc; /**< Pointer to USB device descriptor */
  uint8_t *string_desc; /**< Pointer to array of USB string descriptors */
  uint8_t *full_speed_desc; /**< Pointer to USB device configuration descriptor
                            * when device is operating in full speed mode.
                            */
  uint8_t *high_speed_desc; /**< Pointer to USB device configuration descriptor
                            * when device is operating in high speed mode. For
                            * full-speed only implementation this pointer should
                            * be same as full_speed_desc.
                            */
  uint8_t *device_qualifier; /**< Pointer to USB device qualifier descriptor. For
                             * full-speed only implementation this pointer should
                             * be set to null (0).
                             */
} USB_CORE_DESCS_T;

/** \brief USB device stack initialization parameter data structure.
 *  \ingroup USBD_Core
 *
 *  \details  This data structure is used to pass initialization parameters to the 
 *  USB device stack's init function.
 *
 */
typedef struct USBD_API_INIT_PARAM
{
  uint32_t usb_reg_base; /**< USB device controller's base register address. */ 
  uint32_t mem_base;  /**< Base memory location from where the stack can allocate
                      data and buffers. \note The memory address set in this field
                      should be accessible by USB DMA controller. Also this value
                      should be aligned on 2048 byte boundary.
                      */
  uint32_t mem_size;  /**< The size of memory buffer which stack can use. 
                      \note The \em mem_size should be greater than the size 
                      returned by USBD_HW_API::GetMemSize() routine.*/
  uint8_t max_num_ep; /**< max number of endpoints supported by the USB device 
                      controller instance (specified by \em usb_reg_base field)
                      to which this instance of stack is attached. 
                      */
  uint8_t pad0[3];
  /* USB Device Events Callback Functions */
	/** Event for USB interface reset. This event fires when the USB host requests that the device 
	 *  reset its interface. This event fires after the control endpoint has been automatically
	 *  configured by the library.
	 *  \n
	 *  \note This event is called from USB_ISR context and hence is time-critical. Having delays in this
	 *  callback will prevent the device from enumerating correctly or operate properly.
	 *
	 */
  USB_CB_T USB_Reset_Event;

	/** Event for USB suspend. This event fires when the USB host suspends the device by halting its
	 *  transmission of Start Of Frame pulses to the device. This is generally hooked in order to move
	 *  the device over to a low power state until the host wakes up the device. 
	 *  \n
	 *  \note This event is called from USB_ISR context and hence is time-critical. Having delays in this
	 *  callback will cause other system issues.
	 */
  USB_CB_T USB_Suspend_Event;

	/** Event for USB wake up or resume. This event fires when a the USB device interface is suspended 
	 *  and the host wakes up the device by supplying Start Of Frame pulses. This is generally
	 *  hooked to pull the user application out of a low power state and back into normal operating
	 *  mode. 
	 *  \n
	 *  \note This event is called from USB_ISR context and hence is time-critical. Having delays in this
	 *  callback will cause other system issues.
	 *
	 */
  USB_CB_T USB_Resume_Event;

  /** Reserved parameter should be set to zero. */
  USB_CB_T reserved_sbz;

  /** Event for USB Start Of Frame detection, when enabled. This event fires at the start of each USB
	 *  frame, once per millisecond in full-speed mode or once per 125 microseconds in high-speed mode,
   *  and is synchronized to the USB bus. 
	 *
	 *  This event is time-critical; it is run once per millisecond (full-speed mode) and thus long handlers 
	 *  will significantly degrade device performance. This event should only be enabled when needed to 
   *  reduce device wake-ups.
	 *
	 *  \note This event is not normally active - it must be manually enabled and disabled via the USB interrupt
	 *        register.
	 *        \n\n
	 */  
  USB_CB_T USB_SOF_Event;

  /** Event for remote wake-up configuration, when enabled. This event fires when the USB host
	 *  request the device to configure itself for remote wake-up capability. The USB host sends
   *  this request to device which report remote wake-up capable in their device descriptors,
   *  before going to low-power state. The application layer should implement this callback if
   *  they have any special on board circuit to trigger remote wake up event. Also application
   *  can use this callback to differentiate the following SUSPEND event is caused by cable plug-out
   *  or host SUSPEND request. The device can wake-up host only after receiving this callback and
   *  remote wake-up feature is enabled by host. To signal remote wake-up the device has to generate
   *  resume signaling on bus by calling usapi.hw->WakeUp() routine.
	 *
	 *  \n\n
   *  \param[in] hUsb Handle to the USB device stack. 
   *  \param[in] param1 When 0 - Clear the wake-up configuration, 1 - Enable the wake-up configuration.
   *  \return The call back should return \ref ErrorCode_t type to indicate success or error condition.
	 */  
  USB_PARAM_CB_T USB_WakeUpCfg;

  /** Reserved parameter should be set to zero. */
  USB_PARAM_CB_T USB_Power_Event;

  /** Event for error condition. This event fires when USB device controller detect 
	 *  an error condition in the system.  
	 *
	 *  \n\n
   *  \param[in] hUsb Handle to the USB device stack. 
   *  \param[in] param1 USB device interrupt status register. 
   *  \return The call back should return \ref ErrorCode_t type to indicate success or error condition.
   */  
  USB_PARAM_CB_T USB_Error_Event;

  /* USB Core Events Callback Functions */
  /** Event for USB configuration number changed. This event fires when a the USB host changes the
   *  selected configuration number. On receiving configuration change request from host, the stack
   *  enables/configures the endpoints needed by the new configuration before calling this callback
   *  function.
	 *  \n
	 *  \note This event is called from USB_ISR context and hence is time-critical. Having delays in this
	 *  callback will prevent the device from enumerating correctly or operate properly.
   *
   */
  USB_CB_T USB_Configure_Event;

  /** Event for USB interface setting changed. This event fires when a the USB host changes the
   *  interface setting to one of alternate interface settings. On receiving interface change 
   *  request from host, the stack enables/configures the endpoints needed by the new alternate 
   *  interface setting before calling this callback function.
	 *  \n
	 *  \note This event is called from USB_ISR context and hence is time-critical. Having delays in this
	 *  callback will prevent the device from enumerating correctly or operate properly.
   *
   */
  USB_CB_T USB_Interface_Event;

  /** Event for USB feature changed. This event fires when a the USB host send set/clear feature
   *  request. The stack handles this request for USB_FEATURE_REMOTE_WAKEUP, USB_FEATURE_TEST_MODE
   *  and USB_FEATURE_ENDPOINT_STALL features only. On receiving feature request from host, the  
   *  stack handle the request appropriately and then calls this callback function.
	 *  \n
	 *  \note This event is called from USB_ISR context and hence is time-critical. Having delays in this
	 *  callback will prevent the device from enumerating correctly or operate properly.
   *
   */
 USB_CB_T USB_Feature_Event;

  /* cache and MMU translation functions */
  /** Reserved parameter for future use. should be set to zero. */
  uint32_t (* virt_to_phys)(void* vaddr);
  /** Reserved parameter for future use. should be set to zero. */
  void (* cache_flush)(uint32_t* start_adr, uint32_t* end_adr);

} USBD_API_INIT_PARAM_T;


/** \brief USBD stack Core API functions structure.
 *  \ingroup USBD_Core
 *
 *  \details  This module exposes functions which interact directly with USB device stack's core layer.
 *  The application layer uses this component when it has to implement custom class function driver or 
 *  standard class function driver which is not part of the current USB device stack.
 *  The functions exposed by this interface are to register class specific EP0 handlers and corresponding
 *  utility functions to manipulate EP0 state machine of the stack. This interface also exposes
 *  function to register custom endpoint interrupt handler.
 *
 */
typedef struct USBD_CORE_API 
{
 /** \fn ErrorCode_t RegisterClassHandler(USBD_HANDLE_T hUsb, USB_EP_HANDLER_T pfn, void* data)
  *  Function to register class specific EP0 event handler with USB device stack.
  *
  *  The application layer uses this function when it has to register the custom class's EP0 handler. 
  *  The stack calls all the registered class handlers on any EP0 event before going through default 
  *  handling of the event. This gives the class handlers to implement class specific request handlers
  *  and also to override the default stack handling for a particular event targeted to the interface.
  *  Check \ref USB_EP_HANDLER_T for more details on how the callback function should be implemented. Also
  *  application layer could use this function to register EP0 handler which responds to vendor specific 
  *  requests.
  *  
  *  \param[in] hUsb Handle to the USB device stack. 
  *  \param[in] pfn  Class specific EP0 handler function.
  *  \param[in] data Pointer to the data which will be passed when callback function is called by the stack. 
  *  \return Returns \ref ErrorCode_t type to indicate success or error condition.
  *          \retval LPC_OK On success
  *          \retval ERR_USBD_TOO_MANY_CLASS_HDLR(0x0004000c)  The number of class handlers registered is 
                        greater than the number of handlers allowed by the stack.
  *                                             
  */
  ErrorCode_t (*RegisterClassHandler)(USBD_HANDLE_T hUsb, USB_EP_HANDLER_T pfn, void* data);

 /** \fn ErrorCode_t RegisterEpHandler(USBD_HANDLE_T hUsb, uint32_t ep_index, USB_EP_HANDLER_T pfn, void* data)
  *  Function to register interrupt/event handler for the requested endpoint with USB device stack.
  *
  *  The application layer uses this function to register the endpoint event handler. 
  *  The stack calls all the registered endpoint handlers when 
  *  - USB_EVT_OUT or USB_EVT_OUT_NAK events happen for OUT endpoint.  
  *  - USB_EVT_IN or USB_EVT_IN_NAK events happen for IN endpoint.
  *  Check USB_EP_HANDLER_T for more details on how the callback function should be implemented.
  *  \note By default endpoint _NAK events are not enabled. Application should call \ref USBD_HW_API_T::EnableEvent
  *  for the corresponding endpoint.
  *  
  *  \param[in] hUsb Handle to the USB device stack. 
  *  \param[in] ep_index  Endpoint index. Computed as 
  *                       - For OUT endpoints = 2 * endpoint number eg. for EP2_OUT it is 4.
  *                       - For IN endopoints = (2 * endpoint number) + 1 eg. for EP2_IN it is 5.
  *  \param[in] pfn  Endpoint event handler function.
  *  \param[in] data Pointer to the data which will be passed when callback function is called by the stack. 
  *  \return Returns \ref ErrorCode_t type to indicate success or error condition.
  *          \retval LPC_OK On success
  *          \retval ERR_API_INVALID_PARAM2  ep_index is outside the boundary ( < 2 * USBD_API_INIT_PARAM_T::max_num_ep). 
  *                                             
  */
  ErrorCode_t (*RegisterEpHandler)(USBD_HANDLE_T hUsb, uint32_t ep_index, USB_EP_HANDLER_T pfn, void* data);
  
  /** \fn void SetupStage(USBD_HANDLE_T hUsb)
   *  Function to set EP0 state machine in setup state.
   *
   *  This function is called by USB stack and the application layer to 
   *  set the EP0 state machine in setup state. This function will read
   *  the setup packet received from USB host into stack's buffer. 
   *  \n
   *  \note This interface is provided to users to invoke this function in other 
   *  scenarios which are not handle by current stack. In most user applications 
   *  this function is not called directly.Also this function can be used by  
   *  users who are selectively modifying the USB device stack's standard handlers 
   *  through callback interface exposed by the stack.
   *  
   *  \param[in] hUsb Handle to the USB device stack. 
   *  \return Nothing.
   */
  void (*SetupStage )(USBD_HANDLE_T hUsb); 
  
  /** \fn void DataInStage(USBD_HANDLE_T hUsb)
   *  Function to set EP0 state machine in data_in state.
   *
   *  This function is called by USB stack and the application layer to 
   *  set the EP0 state machine in data_in state. This function will write
   *  the data present in EP0Data buffer to EP0 FIFO for transmission to host.
   *  \n
   *  \note This interface is provided to users to invoke this function in other 
   *  scenarios which are not handle by current stack. In most user applications 
   *  this function is not called directly.Also this function can be used by  
   *  users who are selectively modifying the USB device stack's standard handlers 
   *  through callback interface exposed by the stack.
   *  
   *  \param[in] hUsb Handle to the USB device stack. 
   *  \return Nothing.
   */
  void (*DataInStage)(USBD_HANDLE_T hUsb);

  /** \fn void DataOutStage(USBD_HANDLE_T hUsb)
   *  Function to set EP0 state machine in data_out state.
   *
   *  This function is called by USB stack and the application layer to 
   *  set the EP0 state machine in data_out state. This function will read
   *  the control data (EP0 out packets) received from USB host into EP0Data buffer. 
   *  \n
   *  \note This interface is provided to users to invoke this function in other 
   *  scenarios which are not handle by current stack. In most user applications 
   *  this function is not called directly.Also this function can be used by  
   *  users who are selectively modifying the USB device stack's standard handlers 
   *  through callback interface exposed by the stack.
   *  
   *  \param[in] hUsb Handle to the USB device stack. 
   *  \return Nothing.
   */
  void (*DataOutStage)(USBD_HANDLE_T hUsb); 

  /** \fn void StatusInStage(USBD_HANDLE_T hUsb)
   *  Function to set EP0 state machine in status_in state.
   *
   *  This function is called by USB stack and the application layer to 
   *  set the EP0 state machine in status_in state. This function will send
   *  zero length IN packet on EP0 to host, indicating positive status. 
   *  \n
   *  \note This interface is provided to users to invoke this function in other 
   *  scenarios which are not handle by current stack. In most user applications 
   *  this function is not called directly.Also this function can be used by  
   *  users who are selectively modifying the USB device stack's standard handlers 
   *  through callback interface exposed by the stack.
   *  
   *  \param[in] hUsb Handle to the USB device stack. 
   *  \return Nothing.
   */
  void (*StatusInStage)(USBD_HANDLE_T hUsb); 
  /** \fn void StatusOutStage(USBD_HANDLE_T hUsb)
   *  Function to set EP0 state machine in status_out state.
   *
   *  This function is called by USB stack and the application layer to 
   *  set the EP0 state machine in status_out state. This function will read
   *  the zero length OUT packet received from USB host on EP0. 
   *  \n
   *  \note This interface is provided to users to invoke this function in other 
   *  scenarios which are not handle by current stack. In most user applications 
   *  this function is not called directly.Also this function can be used by  
   *  users who are selectively modifying the USB device stack's standard handlers 
   *  through callback interface exposed by the stack.
   *  
   *  \param[in] hUsb Handle to the USB device stack. 
   *  \return Nothing.
   */
  void (*StatusOutStage)(USBD_HANDLE_T hUsb);

  /** \fn void StallEp0(USBD_HANDLE_T hUsb)
   *  Function to set EP0 state machine in stall state.
   *
   *  This function is called by USB stack and the application layer to 
   *  generate STALL signaling on EP0 endpoint. This function will also
   *  reset the EP0Data buffer. 
   *  \n
   *  \note This interface is provided to users to invoke this function in other 
   *  scenarios which are not handle by current stack. In most user applications 
   *  this function is not called directly.Also this function can be used by  
   *  users who are selectively modifying the USB device stack's standard handlers 
   *  through callback interface exposed by the stack.
   *  
   *  \param[in] hUsb Handle to the USB device stack. 
   *  \return Nothing.
   */
  void (*StallEp0)(USBD_HANDLE_T hUsb);

} USBD_CORE_API_T;

/*-----------------------------------------------------------------------------
 *  Private functions & structures prototypes
 *-----------------------------------------------------------------------------*/

 /** @cond  ADVANCED_API */

/* forward declaration */
struct _USB_CORE_CTRL_T;
typedef struct _USB_CORE_CTRL_T  USB_CORE_CTRL_T;

/* USB device Speed status defines */
#define USB_FULL_SPEED    0
#define USB_HIGH_SPEED    1

/* USB Endpoint Data Structure */
typedef struct _USB_EP_DATA
{
  uint8_t  *pData;
  uint16_t   Count;
  uint16_t pad0;
} USB_EP_DATA;


/* USB core controller data structure */
struct _USB_CORE_CTRL_T
{
  /* override-able function pointers ~ c++ style virtual functions*/
  USB_CB_T USB_EvtSetupHandler;
  USB_CB_T USB_EvtOutHandler;
  USB_PARAM_CB_T USB_ReqVendor;
  USB_CB_T USB_ReqGetStatus;
  USB_CB_T USB_ReqGetDescriptor;
  USB_CB_T USB_ReqGetConfiguration;
  USB_CB_T USB_ReqSetConfiguration;
  USB_CB_T USB_ReqGetInterface;
  USB_CB_T USB_ReqSetInterface;
  USB_PARAM_CB_T USB_ReqSetClrFeature;

  /* USB Device Events Callback Functions */
  USB_CB_T USB_Reset_Event;
  USB_CB_T USB_Suspend_Event;
  USB_CB_T USB_Resume_Event;
  USB_CB_T USB_SOF_Event;
  USB_PARAM_CB_T USB_Power_Event;
  USB_PARAM_CB_T USB_Error_Event;
  USB_PARAM_CB_T USB_WakeUpCfg;

  /* USB Core Events Callback Functions */
  USB_CB_T USB_Configure_Event;
  USB_CB_T USB_Interface_Event;
  USB_CB_T USB_Feature_Event;

  /* cache and MMU translation functions */
  uint32_t (* virt_to_phys)(void* vaddr);
  void (* cache_flush)(uint32_t* start_adr, uint32_t* end_adr);

  /* event handlers for endpoints. */
  USB_EP_HANDLER_T  ep_event_hdlr[2 * USB_MAX_EP_NUM];
  void*  ep_hdlr_data[2 * USB_MAX_EP_NUM];

  /* USB class handlers */
  USB_EP_HANDLER_T  ep0_hdlr_cb[USB_MAX_IF_NUM];
  void*  ep0_cb_data[USB_MAX_IF_NUM];
  uint8_t num_ep0_hdlrs;
  /* USB Core data Variables */
  uint8_t max_num_ep; /* max number of endpoints supported by the HW */
  uint8_t device_speed;
  uint8_t  num_interfaces;
  uint8_t  device_addr;
  uint8_t  config_value;
  uint16_t device_status;
  uint8_t *device_desc;
  uint8_t *string_desc;
  uint8_t *full_speed_desc;
  uint8_t *high_speed_desc;
  uint8_t *device_qualifier;
  uint32_t ep_mask;
  uint32_t ep_halt;
  uint32_t ep_stall;
  uint8_t  alt_setting[USB_MAX_IF_NUM];
  /* HW driver data pointer */
  void* hw_data;

  /* USB Endpoint 0 Data Info */
  USB_EP_DATA EP0Data;

  /* USB Endpoint 0 Buffer */
  //ALIGNED(4)
  uint8_t  EP0Buf[64];

  /* USB Setup Packet */
  //ALIGNED(4)
  USB_SETUP_PACKET SetupPacket;

};

/* USB Core Functions */
extern void mwUSB_InitCore(USB_CORE_CTRL_T* pCtrl, USB_CORE_DESCS_T* pdescr, USBD_API_INIT_PARAM_T* param);
extern void mwUSB_ResetCore(USBD_HANDLE_T hUsb);

/* inline functions */
static INLINE void USB_SetSpeedMode(USB_CORE_CTRL_T* pCtrl, uint8_t mode)
{
    pCtrl->device_speed = mode;   
}

static INLINE bool USB_IsConfigured(USBD_HANDLE_T hUsb)
{
    USB_CORE_CTRL_T* pCtrl = (USB_CORE_CTRL_T*) hUsb;
    return (bool) (pCtrl->config_value != 0);   
}

/** @cond  DIRECT_API */
/* midleware API */
extern ErrorCode_t mwUSB_RegisterClassHandler(USBD_HANDLE_T hUsb, USB_EP_HANDLER_T pfn, void* data);
extern ErrorCode_t mwUSB_RegisterEpHandler(USBD_HANDLE_T hUsb, uint32_t ep_index, USB_EP_HANDLER_T pfn, void* data);
extern void mwUSB_SetupStage (USBD_HANDLE_T hUsb); 
extern void mwUSB_DataInStage(USBD_HANDLE_T hUsb);
extern void mwUSB_DataOutStage(USBD_HANDLE_T hUsb); 
extern void mwUSB_StatusInStage(USBD_HANDLE_T hUsb); 
extern void mwUSB_StatusOutStage(USBD_HANDLE_T hUsb);
extern void mwUSB_StallEp0(USBD_HANDLE_T hUsb);
extern ErrorCode_t mwUSB_RegisterClassHandler(USBD_HANDLE_T hUsb, USB_EP_HANDLER_T pfn, void* data);
extern ErrorCode_t mwUSB_RegisterEpHandler(USBD_HANDLE_T hUsb, uint32_t ep_index, USB_EP_HANDLER_T pfn, void* data);
extern void mwUSB_SetupStage (USBD_HANDLE_T hUsb); 
extern void mwUSB_DataInStage(USBD_HANDLE_T hUsb);
extern void mwUSB_DataOutStage(USBD_HANDLE_T hUsb); 
extern void mwUSB_StatusInStage(USBD_HANDLE_T hUsb); 
extern void mwUSB_StatusOutStage(USBD_HANDLE_T hUsb);
extern void mwUSB_StallEp0(USBD_HANDLE_T hUsb);
/** @endcond */

/** @endcond */

#endif  /* __MW_USBD_CORE_H__ */
