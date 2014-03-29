#ifndef _USB_CDC_HID_CORE_H_
#define _USB_CDC_HID_CORE_H_

#include  "usbd_ioreq.h"

// CDC and HID packet sizes
#define CDC_DATA_FS_MAX_PACKET_SIZE (64) // endpoint IN & OUT packet size

#if 0
// CDC
#define USB_CDC_HID_CONFIG_DESC_SIZ (75)
#define USB_CDC_HID_NUM_INTERFACES (2)
#define USB_CDC_HID_USE_CDC (1)
#define USB_CDC_HID_USE_HID (0)
#define CDC_IN_EP                       0x81  /* EP1 for data IN */
#define CDC_OUT_EP                      0x01  /* EP1 for data OUT */
#define CDC_CMD_EP                      0x82  /* EP2 for CDC commands */
#define HID_IN_EP (0x83)
#define HID_OUT_EP (0x03)
#elif 0
// HID
#define USB_CDC_HID_CONFIG_DESC_SIZ (32)
#define USB_CDC_HID_NUM_INTERFACES (1)
#define USB_CDC_HID_USE_CDC (0)
#define USB_CDC_HID_USE_HID (1)
#define CDC_IN_EP                       0x81  /* EP1 for data IN */
#define CDC_OUT_EP                      0x01  /* EP1 for data OUT */
#define CDC_CMD_EP                      0x82  /* EP2 for CDC commands */
#define HID_IFACE_NUM (0)
#define HID_IN_EP (0x81)
#define HID_OUT_EP (0x01)
#else
// HID + CDC
#define USB_CDC_HID_CONFIG_DESC_SIZ (100)
#define USB_CDC_HID_NUM_INTERFACES (3)
#define USB_CDC_HID_USE_CDC (1)
#define USB_CDC_HID_USE_HID (1)
#define CDC_IFACE_NUM (1)
#define CDC_IN_EP  (0x83)
#define CDC_OUT_EP (0x03)
#define CDC_CMD_EP (0x82)
#define HID_IFACE_NUM (0)
#define HID_IN_EP (0x81)
#endif

typedef struct {
  uint32_t bitrate;
  uint8_t  format;
  uint8_t  paritytype;
  uint8_t  datatype;
} USBD_CDC_LineCodingTypeDef;

typedef struct _USBD_CDC_Itf {
  int8_t (* Init)          (void);
  int8_t (* DeInit)        (void);
  int8_t (* Control)       (uint8_t, uint8_t * , uint16_t);   
  int8_t (* Receive)       (uint8_t *, uint32_t *);  
} USBD_CDC_ItfTypeDef;

typedef struct {
  uint32_t data[CDC_DATA_FS_MAX_PACKET_SIZE/4];      /* Force 32bits alignment */
  uint8_t  CmdOpCode;
  uint8_t  CmdLength;    
  uint8_t  *RxBuffer;  
  uint8_t  *TxBuffer;   
  uint32_t RxLength;
  uint32_t TxLength;    
  
  __IO uint32_t TxState;     
  __IO uint32_t RxState;    
} USBD_CDC_HandleTypeDef;

extern USBD_ClassTypeDef USBD_CDC_HID;

uint8_t USBD_CDC_RegisterInterface(USBD_HandleTypeDef *pdev, USBD_CDC_ItfTypeDef *fops);
uint8_t USBD_CDC_SetTxBuffer(USBD_HandleTypeDef *pdev, uint8_t *pbuff, uint16_t length);
uint8_t USBD_CDC_SetRxBuffer(USBD_HandleTypeDef *pdev, uint8_t *pbuff);
uint8_t USBD_CDC_ReceivePacket(USBD_HandleTypeDef *pdev);
uint8_t USBD_CDC_TransmitPacket(USBD_HandleTypeDef *pdev);

uint8_t USBD_HID_SendReport(USBD_HandleTypeDef *pdev, uint8_t *report, uint16_t len);

#endif // _USB_CDC_HID_CORE_H_
