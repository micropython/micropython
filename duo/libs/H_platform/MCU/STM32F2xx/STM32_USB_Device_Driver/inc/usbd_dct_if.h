/**
  ******************************************************************************
  * @file    usbd_dct_if.h
  * @author  Spark Firmware Team
  * @version V1.1.0
  * @date    18-Nov-2014
  * @brief   Header for usbd_dct_if.c file.
  ******************************************************************************
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DCT_IF_MAL_H
#define __DCT_IF_MAL_H

/* Includes ------------------------------------------------------------------*/
#include "usbd_dfu_mal.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define DCT_START_ADD                  0x00000000

#define DCT_END_ADD                   0x4000
#define DCT_IF_STRING                 "@DCT Flash   /0x00000000/01*016Kg"


extern DFU_MAL_Prop_TypeDef DFU_DCT_cb;

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __DCT_IF_MAL_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
