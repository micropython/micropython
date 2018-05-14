#ifndef _USB_CDC_MSC_CORE_H_
#define _USB_CDC_MSC_CORE_H_

#include  "usbd_cdc_msc_hid0.h"
#include  "usbd_msc_bot.h"
#include  "usbd_msc_scsi.h"
#include  "usbd_ioreq.h"

// These are included to get direct access the MICROPY_HW_USB_xxx config
#include "mpconfigboard.h"
#include "mpconfigboard_common.h"

// Work out if we should support USB high-speed device mode
#if MICROPY_HW_USB_HS \
    && (!MICROPY_HW_USB_HS_IN_FS || defined(STM32F723xx) || defined(STM32F733xx))
#define USBD_SUPPORT_HS_MODE (1)
#else
#define USBD_SUPPORT_HS_MODE (0)
#endif

// Needed for the CDC+MSC+HID state and should be maximum of all template
// config descriptors defined in usbd_cdc_msc_hid.c
#define MAX_TEMPLATE_CONFIG_DESC_SIZE (107)

// CDC, MSC and HID packet sizes
#define MSC_FS_MAX_PACKET           (64)
#define MSC_HS_MAX_PACKET           (512)
#define CDC_DATA_FS_MAX_PACKET_SIZE (64) // endpoint IN & OUT packet size
#define CDC_DATA_HS_MAX_PACKET_SIZE (512) // endpoint IN & OUT packet size
#if USBD_SUPPORT_HS_MODE
#define CDC_DATA_MAX_PACKET_SIZE    CDC_DATA_HS_MAX_PACKET_SIZE
#else
#define CDC_DATA_MAX_PACKET_SIZE    CDC_DATA_FS_MAX_PACKET_SIZE
#endif
#define MSC_MEDIA_PACKET            (2048) // was 8192; how low can it go whilst still working?
#define HID_DATA_FS_MAX_PACKET_SIZE (64) // endpoint IN & OUT packet size

// Need to define here for BOT and SCSI layers
#define MSC_IN_EP     (0x81)
#define MSC_OUT_EP    (0x01)

struct _usbd_cdc_msc_hid_state_t;

typedef struct {
    struct _usbd_cdc_msc_hid_state_t *usbd; // The parent USB device
    uint32_t ctl_packet_buf[CDC_DATA_MAX_PACKET_SIZE / 4]; // Force 32-bit alignment
    uint8_t iface_num;
    uint8_t in_ep;
    uint8_t cur_request;
    uint8_t cur_length;
    volatile uint8_t tx_in_progress;
} usbd_cdc_state_t;

typedef struct _USBD_STORAGE {
  int8_t (* Init) (uint8_t lun);
  int8_t (* GetCapacity) (uint8_t lun, uint32_t *block_num, uint16_t *block_size);
  int8_t (* IsReady) (uint8_t lun);
  int8_t (* IsWriteProtected) (uint8_t lun);
  int8_t (* StartStopUnit)(uint8_t lun, uint8_t started);
  int8_t (* PreventAllowMediumRemoval)(uint8_t lun, uint8_t param0);
  int8_t (* Read) (uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
  int8_t (* Write)(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
  int8_t (* GetMaxLun)(void);
  int8_t *pInquiry;
} USBD_StorageTypeDef;

typedef struct {
  uint32_t                 max_lun;   
  uint32_t                 interface; 
  uint8_t                  bot_state;
  uint8_t                  bot_status;  
  uint16_t                 bot_data_length;
  uint8_t                  bot_data[MSC_MEDIA_PACKET];  
  USBD_MSC_BOT_CBWTypeDef  cbw;
  USBD_MSC_BOT_CSWTypeDef  csw;
  
  USBD_SCSI_SenseTypeDef   scsi_sense [SENSE_LIST_DEEPTH];
  uint8_t                  scsi_sense_head;
  uint8_t                  scsi_sense_tail;
  
  uint16_t                 scsi_blk_size;
  uint32_t                 scsi_blk_nbr;
  
  uint32_t                 scsi_blk_addr_in_blks;
  uint32_t                 scsi_blk_len;

  // operations of the underlying block device
  USBD_StorageTypeDef *bdev_ops;
} USBD_MSC_BOT_HandleTypeDef;

typedef enum {
    HID_IDLE = 0,
    HID_BUSY,
} HID_StateTypeDef;

typedef struct {
    struct _usbd_cdc_msc_hid_state_t *usbd; // The parent USB device
    uint8_t iface_num;
    uint8_t in_ep;
    uint8_t out_ep;
    uint8_t state;
    uint8_t ctl_protocol;
    uint8_t ctl_idle_state;
    uint8_t ctl_alt_setting;
    uint8_t *desc;
    const uint8_t *report_desc;
} usbd_hid_state_t;

typedef struct _usbd_cdc_msc_hid_state_t {
    USBD_HandleTypeDef *pdev;

    uint8_t usbd_mode;
    uint8_t usbd_config_desc_size;

    USBD_MSC_BOT_HandleTypeDef MSC_BOT_ClassData;

    // RAM to hold the current descriptors, which we configure on the fly
    __ALIGN_BEGIN uint8_t usbd_device_desc[USB_LEN_DEV_DESC] __ALIGN_END;
    __ALIGN_BEGIN uint8_t usbd_str_desc[USBD_MAX_STR_DESC_SIZ] __ALIGN_END;
    __ALIGN_BEGIN uint8_t usbd_config_desc[MAX_TEMPLATE_CONFIG_DESC_SIZE] __ALIGN_END;

    usbd_cdc_state_t *cdc;
    usbd_hid_state_t *hid;
} usbd_cdc_msc_hid_state_t;

#define USBD_HID_MOUSE_MAX_PACKET          (4)
#define USBD_HID_MOUSE_REPORT_DESC_SIZE    (74)

extern const uint8_t USBD_HID_MOUSE_ReportDesc[USBD_HID_MOUSE_REPORT_DESC_SIZE];

#define USBD_HID_KEYBOARD_MAX_PACKET       (8)
#define USBD_HID_KEYBOARD_REPORT_DESC_SIZE (63)

extern const uint8_t USBD_HID_KEYBOARD_ReportDesc[USBD_HID_KEYBOARD_REPORT_DESC_SIZE];

extern const USBD_ClassTypeDef USBD_CDC_MSC_HID;

static inline uint32_t usbd_msc_max_packet(USBD_HandleTypeDef *pdev) {
    #if USBD_SUPPORT_HS_MODE
    if (pdev->dev_speed == USBD_SPEED_HIGH) {
        return MSC_HS_MAX_PACKET;
    } else
    #endif
    {
        return MSC_FS_MAX_PACKET;
    }
}

static inline uint32_t usbd_cdc_max_packet(USBD_HandleTypeDef *pdev) {
    #if USBD_SUPPORT_HS_MODE
    if (pdev->dev_speed == USBD_SPEED_HIGH) {
        return CDC_DATA_HS_MAX_PACKET_SIZE;
    } else
    #endif
    {
        return CDC_DATA_FS_MAX_PACKET_SIZE;
    }
}

// returns 0 on success, -1 on failure
int USBD_SelectMode(usbd_cdc_msc_hid_state_t *usbd, uint32_t mode, USBD_HID_ModeInfoTypeDef *hid_info);
// returns the current usb mode
uint8_t USBD_GetMode(usbd_cdc_msc_hid_state_t *usbd);

uint8_t USBD_CDC_ReceivePacket(usbd_cdc_state_t *cdc, uint8_t *buf);
uint8_t USBD_CDC_TransmitPacket(usbd_cdc_state_t *cdc, size_t len, const uint8_t *buf);

static inline void USBD_MSC_RegisterStorage(usbd_cdc_msc_hid_state_t *usbd, USBD_StorageTypeDef *fops) {
    usbd->MSC_BOT_ClassData.bdev_ops = fops;
}

uint8_t USBD_HID_ReceivePacket(usbd_hid_state_t *usbd, uint8_t *buf);
int USBD_HID_CanSendReport(usbd_hid_state_t *usbd);
uint8_t USBD_HID_SendReport(usbd_hid_state_t *usbd, uint8_t *report, uint16_t len);
uint8_t USBD_HID_SetNAK(usbd_hid_state_t *usbd);
uint8_t USBD_HID_ClearNAK(usbd_hid_state_t *usbd);

// These are provided externally to implement the CDC interface
uint8_t *usbd_cdc_init(usbd_cdc_state_t *cdc);
int8_t usbd_cdc_control(usbd_cdc_state_t *cdc, uint8_t cmd, uint8_t* pbuf, uint16_t length);
int8_t usbd_cdc_receive(usbd_cdc_state_t *cdc, size_t len);

// These are provided externally to implement the HID interface
uint8_t *usbd_hid_init(usbd_hid_state_t *hid);
int8_t usbd_hid_receive(usbd_hid_state_t *hid, size_t len);

#endif // _USB_CDC_MSC_CORE_H_
