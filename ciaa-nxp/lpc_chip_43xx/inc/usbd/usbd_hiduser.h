/***********************************************************************
* $Id:: mw_usbd_hiduser.h 331 2012-08-09 18:54:34Z usb10131                   $
*
* Project: USB device ROM Stack
*
* Description:
*     HID Custom User Module Definitions.
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

#ifndef __HIDUSER_H__
#define __HIDUSER_H__

#include "usbd.h"
#include "usbd_hid.h"
#include "usbd_core.h"

/** \file
 *  \brief Human Interface Device (HID) API structures and function prototypes.
 *
 *  Definition of functions exported by ROM based HID function driver.
 *
 */

/** \ingroup Group_USBD
 *  @defgroup USBD_HID HID Class Function Driver
 *  \section Sec_HIDModDescription Module Description
 *  HID Class Function Driver module. This module contains an internal implementation of the USB HID Class.
 *  User applications can use this class driver instead of implementing the HID class manually
 *  via the low-level HW and core APIs.
 *
 *  This module is designed to simplify the user code by exposing only the required interface needed to interface with
 *  Devices using the USB HID Class.
 */

/** \brief HID report descriptor data structure. 
 *  \ingroup USBD_HID
 *
 *  \details  This structure is used as part of HID function driver initialization 
 *  parameter structure \ref USBD_HID_INIT_PARAM. This structure contains
 *  details of a report type supported by the application. An application
 *  can support multiple report types as a single HID device. The application
 *  should define this report type data structure per report it supports and
 *  the array of report types to USBD_HID_API::init() through \ref USBD_HID_INIT_PARAM
 *  structure. 
 *
 *  \note All descriptor pointers assigned in this structure should be on 4 byte
 *  aligned address boundary. 
 *
 */
typedef struct _HID_REPORT_T {
  uint16_t len; /**< Size of the report descriptor in bytes. */ 
  uint8_t idle_time; /**< This value is used by stack to respond to Set_Idle & 
                     GET_Idle requests for the specified report ID. The value
                     of this field specified the rate at which duplicate reports 
                     are generated for the specified Report ID. For example, a 
                     device with two input reports could specify an idle rate of 
                     20 milliseconds for report ID 1 and 500 milliseconds for 
                     report ID 2.
                     */
  uint8_t __pad; /**< Padding space. */
  uint8_t* desc; /**< Report descriptor. */
} USB_HID_REPORT_T;

/** \brief USB descriptors data structure. 
 *  \ingroup USBD_HID
 *
 *  \details  This module exposes functions which interact directly with USB device stack's core layer.
 *  The application layer uses this component when it has to implement custom class function driver or 
 *  standard class function driver which is not part of the current USB device stack.
 *  The functions exposed by this interface are to register class specific EP0 handlers and corresponding
 *  utility functions to manipulate EP0 state machine of the stack. This interface also exposes
 *  function to register custom endpoint interrupt handler.
 *
 */
typedef struct USBD_HID_INIT_PARAM
{
  /* memory allocation params */
  uint32_t mem_base;  /**< Base memory location from where the stack can allocate
                      data and buffers. \note The memory address set in this field
                      should be accessible by USB DMA controller. Also this value
                      should be aligned on 4 byte boundary.
                      */
  uint32_t mem_size;  /**< The size of memory buffer which stack can use. 
                      \note The \em mem_size should be greater than the size 
                      returned by USBD_HID_API::GetMemSize() routine.*/
  /* HID paramas */
  uint8_t max_reports; /**< Number of HID reports supported by this instance
                       of HID class driver. 
                       */
  uint8_t pad[3];
  uint8_t* intf_desc; /**< Pointer to the HID interface descriptor within the 
                      descriptor array (\em high_speed_desc) passed to Init()
                      through \ref USB_CORE_DESCS_T structure.  
                      */
  USB_HID_REPORT_T* report_data; /**< Pointer to an array of HID report descriptor
                                 data structure (\ref USB_HID_REPORT_T). The number
                                 of elements in the array should be same a \em max_reports
                                 value. The stack uses this array to respond to 
                                 requests received for various HID report descriptor
                                 information. \note This array should be of global scope.
                                 */

  /* user defined functions */
  /* required functions */
  /** 
  *  HID get report callback function.
  *
  *  This function is provided by the application software. This function gets called 
  *  when host sends a HID_REQUEST_GET_REPORT request. The setup packet data (\em pSetup)
  *  is passed to the callback so that application can extract the report ID, report
  *  type and other information need to generate the report. \note HID reports are sent
  *  via interrupt IN endpoint also. This function is called only when report request
  *  is received on control endpoint. Application should implement \em HID_EpIn_Hdlr to
  *  send reports to host via interrupt IN endpoint.
  *   
  *  
  *  \param[in] hHid Handle to HID function driver. 
  *  \param[in] pSetup Pointer to setup packet received from host.
  *  \param[in, out] pBuffer  Pointer to a pointer of data buffer containing report data. 
  *                       Pointer-to-pointer is used to implement zero-copy buffers. 
  *                       See \ref USBD_ZeroCopy for more details on zero-copy concept.
  *  \param[in] length  Amount of data copied to destination buffer.
  *  \return The call back should returns \ref ErrorCode_t type to indicate success or error condition.
  *          \retval LPC_OK On success.
  *          \retval ERR_USBD_UNHANDLED  Event is not handled hence pass the event to next in line. 
  *          \retval ERR_USBD_xxx  For other error conditions. 
  *                                             
  */
  ErrorCode_t (*HID_GetReport)( USBD_HANDLE_T hHid, USB_SETUP_PACKET* pSetup, uint8_t** pBuffer, uint16_t* length); 
  
  /** 
  *  HID set report callback function.
  *
  *  This function is provided by the application software. This function gets called 
  *  when host sends a HID_REQUEST_SET_REPORT request. The setup packet data (\em pSetup)
  *  is passed to the callback so that application can extract the report ID, report
  *  type and other information need to modify the report. An application might choose 
  *  to ignore input Set_Report requests as meaningless. Alternatively these reports 
  *  could be used to reset the origin of a control (that is, current position should 
  *  report zero).
  *  
  *  \param[in] hHid Handle to HID function driver. 
  *  \param[in] pSetup Pointer to setup packet received from host.
  *  \param[in, out] pBuffer  Pointer to a pointer of data buffer containing report data. 
  *                       Pointer-to-pointer is used to implement zero-copy buffers. 
  *                       See \ref USBD_ZeroCopy for more details on zero-copy concept.
  *  \param[in] length  Amount of data copied to destination buffer.
  *  \return The call back should returns \ref ErrorCode_t type to indicate success or error condition.
  *          \retval LPC_OK On success.
  *          \retval ERR_USBD_UNHANDLED  Event is not handled hence pass the event to next in line. 
  *          \retval ERR_USBD_xxx  For other error conditions. 
  *                                             
  */
  ErrorCode_t (*HID_SetReport)( USBD_HANDLE_T hHid, USB_SETUP_PACKET* pSetup, uint8_t** pBuffer, uint16_t length);
  
  /* optional functions */
  
  /** 
  *  Optional callback function to handle HID_GetPhysDesc request.
  *
  *  The application software could provide this callback HID_GetPhysDesc handler to
  *  handle get physical descriptor requests sent by the host. When host requests 
  *  Physical Descriptor set 0, application should return a special descriptor
  *  identifying the number of descriptor sets and their sizes. A Get_Descriptor 
  *  request with the Physical Index equal to 1 should return the first Physical 
  *  Descriptor set. A device could possibly have alternate uses for its items. 
  *  These can be enumerated by issuing subsequent Get_Descriptor requests while 
  *  incrementing the Descriptor Index. A device should return the last descriptor
  *  set to requests with an index greater than the last number defined in the HID 
  *  descriptor.
  *  \note Applications which don't have physical descriptor should set this data member
  *  to zero before calling the USBD_HID_API::Init().
  *  \n
  *  
  *  \param[in] hHid Handle to HID function driver. 
  *  \param[in] pSetup Pointer to setup packet received from host.
  *  \param[in] pBuf Pointer to a pointer of data buffer containing physical descriptor 
  *                   data. If the physical descriptor is in USB accessible memory area
  *                   application could just update the pointer or else it should copy 
  *                   the descriptor to the address pointed by this pointer.
  *  \param[in] length  Amount of data copied to destination buffer or descriptor length.
  *  \return The call back should returns \ref ErrorCode_t type to indicate success or error condition.
  *          \retval LPC_OK On success.
  *          \retval ERR_USBD_UNHANDLED  Event is not handled hence pass the event to next in line. 
  *          \retval ERR_USBD_xxx  For other error conditions. 
  *                                             
  */
  ErrorCode_t (*HID_GetPhysDesc)( USBD_HANDLE_T hHid, USB_SETUP_PACKET* pSetup, uint8_t** pBuf, uint16_t* length);

  /** 
  *  Optional callback function to handle HID_REQUEST_SET_IDLE request.
  *
  *  The application software could provide this callback to handle HID_REQUEST_SET_IDLE
  *  requests sent by the host. This callback is provided to applications to adjust
  *  timers associated with various reports, which are sent to host over interrupt 
  *  endpoint. The setup packet data (\em pSetup) is passed to the callback so that
  *  application can extract the report ID, report type and other information need 
  *  to modify the report's idle time.
  *  \note Applications which don't send reports on Interrupt endpoint or don't
  *  have idle time between reports should set this data member to zero before 
  *  calling the USBD_HID_API::Init().
  *  \n
  *  
  *  \param[in] hHid Handle to HID function driver. 
  *  \param[in] pSetup Pointer to setup packet received from host.
  *  \param[in] idleTime  Idle time to be set for the specified report.
  *  \return The call back should returns \ref ErrorCode_t type to indicate success or error condition.
  *          \retval LPC_OK On success.
  *          \retval ERR_USBD_UNHANDLED  Event is not handled hence pass the event to next in line. 
  *          \retval ERR_USBD_xxx  For other error conditions. 
  *                                             
  */
  ErrorCode_t (*HID_SetIdle)( USBD_HANDLE_T hHid, USB_SETUP_PACKET* pSetup, uint8_t idleTime); 
 
  /** 
  *  Optional callback function to handle HID_REQUEST_SET_PROTOCOL request.
  *
  *  The application software could provide this callback to handle HID_REQUEST_SET_PROTOCOL
  *  requests sent by the host. This callback is provided to applications to adjust
  *  modes of their code between boot mode and report mode. 
  *  \note Applications which don't support protocol modes should set this data member
  *  to zero before calling the USBD_HID_API::Init().
  *  \n
  *  
  *  \param[in] hHid Handle to HID function driver. 
  *  \param[in] pSetup Pointer to setup packet received from host.
  *  \param[in] protocol  Protocol mode. 
  *                       0 = Boot Protocol
  *                       1 = Report Protocol
  *  \return The call back should returns \ref ErrorCode_t type to indicate success or error condition.
  *          \retval LPC_OK On success.
  *          \retval ERR_USBD_UNHANDLED  Event is not handled hence pass the event to next in line. 
  *          \retval ERR_USBD_xxx  For other error conditions. 
  *                                             
  */
  ErrorCode_t (*HID_SetProtocol)( USBD_HANDLE_T hHid, USB_SETUP_PACKET* pSetup, uint8_t protocol); 
  
  /** 
  *  Optional Interrupt IN endpoint event handler.
  *
  *  The application software could provide Interrupt IN endpoint event handler. 
  *  Application which send reports to host on interrupt endpoint should provide
  *  an endpoint event handler through this data member. This data member is
  *  ignored if the interface descriptor \em intf_desc doesn't have any IN interrupt 
  *  endpoint descriptor associated. 
  *  \n
  *  
  *  \param[in] hUsb Handle to the USB device stack. 
  *  \param[in] data Handle to HID function driver. 
  *  \param[in] event  Type of endpoint event. See \ref USBD_EVENT_T for more details.
  *  \return The call back should return \ref ErrorCode_t type to indicate success or error condition.
  *          \retval LPC_OK On success.
  *          \retval ERR_USBD_UNHANDLED  Event is not handled hence pass the event to next in line. 
  *          \retval ERR_USBD_xxx  For other error conditions. 
  *                                             
  */
  ErrorCode_t (*HID_EpIn_Hdlr) (USBD_HANDLE_T hUsb, void* data, uint32_t event);
  /** 
  *  Optional Interrupt OUT endpoint event handler.
  *
  *  The application software could provide Interrupt OUT endpoint event handler. 
  *  Application which receives reports from host on interrupt endpoint should provide
  *  an endpoint event handler through this data member. This data member is
  *  ignored if the interface descriptor \em intf_desc doesn't have any OUT interrupt 
  *  endpoint descriptor associated. 
  *  \n
  *  
  *  \param[in] hUsb Handle to the USB device stack. 
  *  \param[in] data Handle to HID function driver. 
  *  \param[in] event  Type of endpoint event. See \ref USBD_EVENT_T for more details.
  *  \return The call back should return \ref ErrorCode_t type to indicate success or error condition.
  *          \retval LPC_OK On success.
  *          \retval ERR_USBD_UNHANDLED  Event is not handled hence pass the event to next in line. 
  *          \retval ERR_USBD_xxx  For other error conditions. 
  *                                             
  */
  ErrorCode_t (*HID_EpOut_Hdlr) (USBD_HANDLE_T hUsb, void* data, uint32_t event);

  /* user override-able function */
  /** 
  *  Optional user override-able function to replace the default HID_GetReportDesc handler.
  *
  *  The application software could override the default HID_GetReportDesc handler with their
  *  own by providing the handler function address as this data member of the parameter
  *  structure. Application which like the default handler should set this data member
  *  to zero before calling the USBD_HID_API::Init() and also provide report data array
  *  \em report_data field.
  *  \n
  *  \note 
  *  
  *  \param[in] hUsb Handle to the USB device stack. 
  *  \param[in] data Pointer to the data which will be passed when callback function is called by the stack. 
  *  \param[in] event  Type of endpoint event. See \ref USBD_EVENT_T for more details.
  *  \return The call back should returns \ref ErrorCode_t type to indicate success or error condition.
  *          \retval LPC_OK On success.
  *          \retval ERR_USBD_UNHANDLED  Event is not handled hence pass the event to next in line. 
  *          \retval ERR_USBD_xxx  For other error conditions. 
  *                                             
  */
  ErrorCode_t (*HID_GetReportDesc)(USBD_HANDLE_T hHid, USB_SETUP_PACKET* pSetup, uint8_t** pBuf, uint16_t* length);
  /** 
  *  Optional user override-able function to replace the default HID class handler.
  *
  *  The application software could override the default EP0 class handler with their
  *  own by providing the handler function address as this data member of the parameter
  *  structure. Application which like the default handler should set this data member
  *  to zero before calling the USBD_HID_API::Init().
  *  \n
  *  \note 
  *  
  *  \param[in] hUsb Handle to the USB device stack. 
  *  \param[in] data Pointer to the data which will be passed when callback function is called by the stack. 
  *  \param[in] event  Type of endpoint event. See \ref USBD_EVENT_T for more details.
  *  \return The call back should returns \ref ErrorCode_t type to indicate success or error condition.
  *          \retval LPC_OK On success.
  *          \retval ERR_USBD_UNHANDLED  Event is not handled hence pass the event to next in line. 
  *          \retval ERR_USBD_xxx  For other error conditions. 
  *                                             
  */
  ErrorCode_t (*HID_Ep0_Hdlr) (USBD_HANDLE_T hUsb, void* data, uint32_t event);

} USBD_HID_INIT_PARAM_T;

/** \brief HID class API functions structure.
 *  \ingroup USBD_HID
 *
 *  This structure contains pointers to all the function exposed by HID function driver module.
 *
 */
typedef struct USBD_HID_API 
{
  /** \fn uint32_t GetMemSize(USBD_HID_INIT_PARAM_T* param)
   *  Function to determine the memory required by the HID function driver module.
   * 
   *  This function is called by application layer before calling pUsbApi->hid->Init(), to allocate memory used 
   *  by HID function driver module. The application should allocate the memory which is accessible by USB
   *  controller/DMA controller. 
   *  \note Some memory areas are not accessible by all bus masters.
   *
   *  \param[in] param Structure containing HID function driver module initialization parameters.
   *  \return Returns the required memory size in bytes.
   */
  uint32_t (*GetMemSize)(USBD_HID_INIT_PARAM_T* param);

  /** \fn ErrorCode_t init(USBD_HANDLE_T hUsb, USBD_HID_INIT_PARAM_T* param)
   *  Function to initialize HID function driver module.
   * 
   *  This function is called by application layer to initialize HID function driver  
   *  module. On successful initialization the function returns a handle to HID 
   *  function driver module in passed param structure.  
   *
   *  \param[in] hUsb Handle to the USB device stack. 
   *  \param[in, out] param Structure containing HID function driver module 
   *      initialization parameters.
   *  \return Returns \ref ErrorCode_t type to indicate success or error condition.
   *          \retval LPC_OK On success
   *          \retval ERR_USBD_BAD_MEM_BUF  Memory buffer passed is not 4-byte 
   *              aligned or smaller than required. 
   *          \retval ERR_API_INVALID_PARAM2 Either HID_GetReport() or HID_SetReport()
   *              callback are not defined. 
   *          \retval ERR_USBD_BAD_DESC  HID_HID_DESCRIPTOR_TYPE is not defined 
   *              immediately after interface descriptor. 
   *          \retval ERR_USBD_BAD_INTF_DESC  Wrong interface descriptor is passed. 
   *          \retval ERR_USBD_BAD_EP_DESC  Wrong endpoint descriptor is passed. 
   */
  ErrorCode_t (*init)(USBD_HANDLE_T hUsb, USBD_HID_INIT_PARAM_T* param);

} USBD_HID_API_T;

/*-----------------------------------------------------------------------------
 *  Private functions & structures prototypes
 *-----------------------------------------------------------------------------*/
/** @cond  ADVANCED_API */

typedef struct _HID_CTRL_T {
  /* pointer to controller */
  USB_CORE_CTRL_T*  pUsbCtrl;
  /* descriptor pointers */
  uint8_t* hid_desc;
  USB_HID_REPORT_T* report_data;

  uint8_t protocol;
  uint8_t if_num;                  /* interface number */
  uint8_t epin_adr;                /* IN interrupt endpoint */
  uint8_t epout_adr;               /* OUT interrupt endpoint */

  /* user defined functions */
  ErrorCode_t (*HID_GetReport)( USBD_HANDLE_T hHid, USB_SETUP_PACKET* pSetup, uint8_t** pBuffer, uint16_t* length); 
  ErrorCode_t (*HID_SetReport)( USBD_HANDLE_T hHid, USB_SETUP_PACKET* pSetup, uint8_t** pBuffer, uint16_t length);
  ErrorCode_t (*HID_GetPhysDesc)( USBD_HANDLE_T hHid, USB_SETUP_PACKET* pSetup, uint8_t** pBuf, uint16_t* length);
  ErrorCode_t (*HID_SetIdle)( USBD_HANDLE_T hHid, USB_SETUP_PACKET* pSetup, uint8_t idleTime); 
  ErrorCode_t (*HID_SetProtocol)( USBD_HANDLE_T hHid, USB_SETUP_PACKET* pSetup, uint8_t protocol); 

  /* virtual overridable functions */ 
  ErrorCode_t (*HID_GetReportDesc)(USBD_HANDLE_T hHid, USB_SETUP_PACKET* pSetup, uint8_t** pBuf, uint16_t* length);

}USB_HID_CTRL_T;

/** @cond  DIRECT_API */
extern uint32_t mwHID_GetMemSize(USBD_HID_INIT_PARAM_T* param);
extern ErrorCode_t mwHID_init(USBD_HANDLE_T hUsb, USBD_HID_INIT_PARAM_T* param);
/** @endcond */

/** @endcond */

#endif  /* __HIDUSER_H__ */
