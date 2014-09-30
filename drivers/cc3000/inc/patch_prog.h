#ifndef __CC3000_PATCH_PROG_H__
#define __CC3000_PATCH_PROG_H__
extern unsigned short fw_length;
extern const unsigned char fw_patch[];

extern unsigned short drv_length;
extern const unsigned char wlan_drv_patch[];
extern const unsigned char cRMdefaultParams[128];

void patch_prog_start();
#endif //__CC3000_PATCH_PROG_H__
