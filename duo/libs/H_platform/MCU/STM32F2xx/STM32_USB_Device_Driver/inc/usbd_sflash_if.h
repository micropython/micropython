/**
 ******************************************************************************
 * @file    usbd_sflash_if.h
 * @author  Satish Nair
 * @version V1.0.0
 * @date    26-Nov-2014
 * @brief   Header for usbd_sflash_if.c file.
 ******************************************************************************
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SFLASH_IF_MAL_H
#define __SFLASH_IF_MAL_H

/* Includes ------------------------------------------------------------------*/
#include "usbd_dfu_mal.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define sFLASH_START_ADD                  0x00000000
#if PLATFORM_ID == PLATFORM_DUO_PRODUCTION
#define sFLASH_END_ADD                    0x00200000
#else
#define sFLASH_END_ADD                    0x00100000
#endif

#if PLATFORM_ID == PLATFORM_DUO_PRODUCTION
#define sFLASH_IF_STRING                  "@Serial Flash   /0x00000000/192*004Kg,128*004Kg,64*004Kg,128*004Kg"
#else
#define sFLASH_IF_STRING                  "@Serial Flash   /0x00000000/256*004Kg"
#endif

extern DFU_MAL_Prop_TypeDef DFU_sFlash_cb;

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __SFLASH_IF_MAL_H */

