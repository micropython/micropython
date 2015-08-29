/***********************************************************************
* $Id:: mw_usbd_dfuuser.h 331 2012-08-09 18:54:34Z usb10131                   $
*
* Project: USB device ROM Stack
*
* Description:
*     Device Firmware Upgrade Class Custom User Module Definitions.
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

#ifndef __DFUUSER_H__
#define __DFUUSER_H__

#include "usbd.h"
#include "usbd_dfu.h"
#include "usbd_core.h"

/** \file
 *  \brief Device Firmware Upgrade (DFU) API structures and function prototypes.
 *
 *  Definition of functions exported by ROM based DFU function driver.
 *
 */


/** \ingroup Group_USBD
 *  @defgroup USBD_DFU Device Firmware Upgrade (DFU) Class Function Driver
 *  \section Sec_MSCModDescription Module Description
 *  DFU Class Function Driver module. This module contains an internal implementation of the USB DFU Class.
 *  User applications can use this class driver instead of implementing the DFU class manually
 *  via the low-level USBD_HW and USBD_Core APIs.
 *
 *  This module is designed to simplify the user code by exposing only the required interface needed to interface with
 *  Devices using the USB DFU Class.
 */

/** \brief USB descriptors data structure.
 *  \ingroup USBD_DFU
 *
 *  \details  This module exposes functions which interact directly with USB device stack's core layer.
 *  The application layer uses this component when it has to implement custom class function driver or 
 *  standard class function driver which is not part of the current USB device stack.
 *  The functions exposed by this interface are to register class specific EP0 handlers and corresponding
 *  utility functions to manipulate EP0 state machine of the stack. This interface also exposes
 *  function to register custom endpoint interrupt handler.
 *
 */
typedef struct USBD_DFU_INIT_PARAM
{
  /* memory allocation params */
  uint32_t mem_base;  /**< Base memory location from where the stack can allocate
                      data and buffers. \note The memory address set in this field
                      should be accessible by USB DMA controller. Also this value
                      should be aligned on 4 byte boundary.
                      */
  uint32_t mem_size;  /**< The size of memory buffer which stack can use. 
                      \note The \em mem_size should be greater than the size 
                      returned by USBD_DFU_API::GetMemSize() routine.*/
  /* DFU paramas */
  uint16_t wTransferSize; /**< DFU transfer block size in number of bytes.
                          This value should match the value set in DFU descriptor
                          provided as part of the descriptor array 
                          (\em high_speed_desc) passed to Init() through 
                          \ref USB_CORE_DESCS_T structure.  */

  uint16_t pad;
  /** Pointer to the DFU interface descriptor within the descriptor
  * array (\em high_speed_desc) passed to Init() through \ref USB_CORE_DESCS_T 
  * structure.  
  */
  uint8_t* intf_desc;
  /* user defined functions */
  /** 
  *  DFU Write callback function.
  *
  *  This function is provided by the application software. This function gets called 
  *  when host sends a write command. For application using zero-copy buffer scheme
  *  this function is called for the first time with \em length parameter set to 0.
  *  The application code should update the buffer pointer.
  *  
  *  \param[in] block_num Destination start address. 
  *  \param[in, out] src  Pointer to a pointer to the source of data. Pointer-to-pointer
  *                     is used to implement zero-copy buffers. See \ref USBD_ZeroCopy
  *                     for more details on zero-copy concept.
  *  \param[out] bwPollTimeout  Pointer to a 3 byte buffer which the callback implementer
  *                     should fill with the amount of minimum time, in milliseconds, 
  *                     that the host should wait before sending a subsequent
  *                     DFU_GETSTATUS request. 
  *  \param[in] length  Number of bytes to be written.
  *  \return Returns DFU_STATUS_ values defined in mw_usbd_dfu.h. 
  *                                             
  */
  uint8_t (*DFU_Write)( uint32_t block_num, uint8_t** src, uint32_t length, uint8_t* bwPollTimeout);

  /** 
  *  DFU Read callback function.
  *
  *  This function is provided by the application software. This function gets called 
  *  when host sends a read command.
  *  
  *  \param[in] block_num Destination start address. 
  *  \param[in, out] dst  Pointer to a pointer to the source of data. Pointer-to-pointer
  *                       is used to implement zero-copy buffers. See \ref USBD_ZeroCopy
  *                       for more details on zero-copy concept.
  *  \param[in] length  Amount of data copied to destination buffer.
  *  \return Returns 
  *                 - DFU_STATUS_ values defined in mw_usbd_dfu.h to return error conditions. 
  *                 - 0 if there is no more data to be read. Stack will send EOF frame and set 
  *                     DFU state-machine to dfuIdle state.
  *                 - length of the data copied, should be greater than or equal to 16. If the data copied
  *                   is less than DFU \em wTransferSize the stack will send EOF frame and 
  *                   goes to dfuIdle state.
  *                                             
  */
  uint32_t (*DFU_Read)( uint32_t block_num, uint8_t** dst, uint32_t length);

  /** 
  *  DFU done callback function.
  *
  *  This function is provided by the application software. This function gets called 
  *  after firmware download completes.
  *  
  *  \return Nothing. 
  *                                             
  */
  void (*DFU_Done)(void);

  /** 
  *  DFU detach callback function.
  *
  *  This function is provided by the application software. This function gets called 
  *  after USB_REQ_DFU_DETACH is received. Applications which set USB_DFU_WILL_DETACH
  *  bit in DFU descriptor should define this function. As part of this function
  *  application can call Connect() routine to disconnect and then connect back with 
  *  host. For application which rely on WinUSB based host application should use this
  *  feature since USB reset can be invoked only by kernel drivers on Windows host. 
  *  By implementing this feature host doen't have to issue reset instead the device
  *  has to do it automatically by disconnect and connect procedure.
  *  
  *  \param[in] hUsb Handle DFU control structure. 
  *  \return Nothing. 
  *                                             
  */
  void (*DFU_Detach)(USBD_HANDLE_T hUsb);

  /** 
  *  Optional user override-able function to replace the default DFU class handler.
  *
  *  The application software could override the default EP0 class handler with their
  *  own by providing the handler function address as this data member of the parameter
  *  structure. Application which like the default handler should set this data member
  *  to zero before calling the USBD_DFU_API::Init().
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
  ErrorCode_t (*DFU_Ep0_Hdlr) (USBD_HANDLE_T hUsb, void* data, uint32_t event);

} USBD_DFU_INIT_PARAM_T;


/** \brief DFU class API functions structure.
 *  \ingroup USBD_DFU
 *
 *  This module exposes functions which interact directly with USB device controller hardware.
 *
 */
typedef struct USBD_DFU_API
{
  /** \fn uint32_t GetMemSize(USBD_DFU_INIT_PARAM_T* param)
   *  Function to determine the memory required by the DFU function driver module.
   * 
   *  This function is called by application layer before calling pUsbApi->dfu->Init(), to allocate memory used 
   *  by DFU function driver module. The application should allocate the memory which is accessible by USB
   *  controller/DMA controller. 
   *  \note Some memory areas are not accessible by all bus masters.
   *
   *  \param[in] param Structure containing DFU function driver module initialization parameters.
   *  \return Returns the required memory size in bytes.
   */
  uint32_t (*GetMemSize)(USBD_DFU_INIT_PARAM_T* param);

  /** \fn ErrorCode_t init(USBD_HANDLE_T hUsb, USBD_DFU_INIT_PARAM_T* param)
   *  Function to initialize DFU function driver module.
   * 
   *  This function is called by application layer to initialize DFU function driver module. 
   *
   *  \param[in] hUsb Handle to the USB device stack. 
   *  \param[in, out] param Structure containing DFU function driver module initialization parameters.
   *  \return Returns \ref ErrorCode_t type to indicate success or error condition.
   *          \retval LPC_OK On success
   *          \retval ERR_USBD_BAD_MEM_BUF  Memory buffer passed is not 4-byte aligned or smaller than required. 
   *          \retval ERR_API_INVALID_PARAM2 Either DFU_Write() or DFU_Done() or DFU_Read() call-backs are not defined.
   *          \retval ERR_USBD_BAD_DESC  
   *            - USB_DFU_DESCRIPTOR_TYPE is not defined immediately after 
   *              interface descriptor.
   *            - wTransferSize in descriptor doesn't match the value passed 
   *              in param->wTransferSize.
   *            - DFU_Detach() is not defined while USB_DFU_WILL_DETACH is set 
   *              in DFU descriptor.
   *          \retval ERR_USBD_BAD_INTF_DESC  Wrong interface descriptor is passed. 
   */
  ErrorCode_t (*init)(USBD_HANDLE_T hUsb, USBD_DFU_INIT_PARAM_T* param, uint32_t init_state);

} USBD_DFU_API_T;

/*-----------------------------------------------------------------------------
 *  Private functions & structures prototypes
 *-----------------------------------------------------------------------------*/
/** @cond  ADVANCED_API */

typedef struct _USBD_DFU_CTRL_T
{
  /*ALIGNED(4)*/ DFU_STATUS_T dfu_req_get_status;
  uint16_t pad;
  uint8_t dfu_state;
  uint8_t dfu_status;
  uint8_t download_done;
  uint8_t if_num;                  /* interface number */

  uint8_t* xfr_buf;
  USB_DFU_FUNC_DESCRIPTOR* dfu_desc;

  USB_CORE_CTRL_T*  pUsbCtrl;
  /* user defined functions */
  /* return DFU_STATUS_ values defined in mw_usbd_dfu.h */
  uint8_t (*DFU_Write)( uint32_t block_num, uint8_t** src, uint32_t length, uint8_t* bwPollTimeout);
  /* return 
  * DFU_STATUS_ : values defined in mw_usbd_dfu.h in case of errors
  * 0 : If end of memory reached
  * length : Amount of data copied to destination buffer
  */
  uint32_t (*DFU_Read)( uint32_t block_num, uint8_t** dst, uint32_t length);
  /* callback called after download is finished */
  void (*DFU_Done)(void);
  /* callback called after USB_REQ_DFU_DETACH is recived */
  void (*DFU_Detach)(USBD_HANDLE_T hUsb);

} USBD_DFU_CTRL_T;

/** @cond  DIRECT_API */
uint32_t mwDFU_GetMemSize(USBD_DFU_INIT_PARAM_T* param);
extern ErrorCode_t mwDFU_init(USBD_HANDLE_T hUsb, USBD_DFU_INIT_PARAM_T* param, uint32_t init_state);
/** @endcond */

/** @endcond */

#endif  /* __DFUUSER_H__ */
