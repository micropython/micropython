/* generated configuration header file - do not edit */
#ifndef R_USB_PCDC_CFG_H_
#define R_USB_PCDC_CFG_H_
#ifdef __cplusplus
extern "C" {
#endif

#if (RA_NOT_DEFINED != RA_NOT_DEFINED)
#include "r_usb_pcdc_2ch_cfg.h"
#else
#define USB_CFG_PCDC_BULK_IN2 (USB_NULL)
#define USB_CFG_PCDC_BULK_OUT2 (USB_NULL)
#define USB_CFG_PCDC_INT_IN2 (USB_NULL)
#endif

#define USB_CFG_PCDC_BULK_IN (USB_PIPE1)
#define USB_CFG_PCDC_BULK_OUT (USB_PIPE2)
#define USB_CFG_PCDC_INT_IN (USB_PIPE6)

#ifdef __cplusplus
}
#endif
#endif /* R_USB_PCDC_CFG_H_ */
