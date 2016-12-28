/**
 ******************************************************************************
 * @file    platform_config.h
 * @authors Satish Nair, Brett Walach
 * @version V1.0.0
 * @date    22-Oct-2014
 * @brief   Board specific configuration file.
 ******************************************************************************
  Copyright (c) 2013-2015 Particle Industries, Inc.  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
  ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PLATFORM_CONFIG_H
#define __PLATFORM_CONFIG_H

#include "platforms.h"

#define         ID1          (0x1FFF7A10)
#define         ID2          (0x1FFF7A14)
#define         ID3          (0x1FFF7A18)

#ifndef PLATFORM_ID
#error "PLATFORM_ID not defined"
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

//LEDs
#if PLATFORM_DUO_PRODUCTION == PLATFORM_ID
#define LEDn                                4
#define LED1_GPIO_AF_TIM                    0                       //User Led
#define LED1_GPIO_PIN                       GPIO_Pin_13             //User Led
#define LED1_GPIO_PIN_SOURCE                GPIO_PinSource13        //User Led
#define LED1_GPIO_PORT                      GPIOA                   //User Led
#define LED1_GPIO_CLK                       RCC_AHB1Periph_GPIOA    //User Led
#define LED2_GPIO_AF_TIM                    GPIO_AF_TIM3            //BLUE Led
#define LED2_GPIO_PIN                       GPIO_Pin_0              //BLUE Led
#define LED2_GPIO_PIN_SOURCE                GPIO_PinSource0         //BLUE Led
#define LED2_GPIO_PORT                      GPIOB                   //BLUE Led
#define LED2_GPIO_CLK                       RCC_AHB1Periph_GPIOB    //BLUE Led
#define LED3_GPIO_AF_TIM                    GPIO_AF_TIM2            //RED Led
#define LED3_GPIO_PIN                       GPIO_Pin_11             //RED Led
#define LED3_GPIO_PIN_SOURCE                GPIO_PinSource11        //RED Led
#define LED3_GPIO_PORT                      GPIOB                   //RED Led
#define LED3_GPIO_CLK                       RCC_AHB1Periph_GPIOB    //RED Led
#define LED4_GPIO_AF_TIM                    GPIO_AF_TIM3            //GREEN Led
#define LED4_GPIO_PIN                       GPIO_Pin_1              //GREEN Led
#define LED4_GPIO_PIN_SOURCE                GPIO_PinSource1         //GREEN Led
#define LED4_GPIO_PORT                      GPIOB                   //GREEN Led
#define LED4_GPIO_CLK                       RCC_AHB1Periph_GPIOB    //GREEN Led
#else
#define LEDn                                4
#define LED1_GPIO_AF_TIM                    0                       //User Led
#define LED1_GPIO_PIN                       GPIO_Pin_13             //User Led
#define LED1_GPIO_PIN_SOURCE                GPIO_PinSource13        //User Led
#define LED1_GPIO_PORT                      GPIOA                   //User Led
#define LED1_GPIO_CLK                       RCC_AHB1Periph_GPIOA    //User Led
#define LED2_GPIO_AF_TIM                    GPIO_AF_TIM2            //BLUE Led
#define LED2_GPIO_PIN                       GPIO_Pin_3              //BLUE Led
#define LED2_GPIO_PIN_SOURCE                GPIO_PinSource3         //BLUE Led
#define LED2_GPIO_PORT                      GPIOA                   //BLUE Led
#define LED2_GPIO_CLK                       RCC_AHB1Periph_GPIOA    //BLUE Led
#define LED3_GPIO_AF_TIM                    GPIO_AF_TIM2            //RED Led
#define LED3_GPIO_PIN                       GPIO_Pin_1              //RED Led
#define LED3_GPIO_PIN_SOURCE                GPIO_PinSource1         //RED Led
#define LED3_GPIO_PORT                      GPIOA                   //RED Led
#define LED3_GPIO_CLK                       RCC_AHB1Periph_GPIOA    //RED Led
#define LED4_GPIO_AF_TIM                    GPIO_AF_TIM2            //GREEN Led
#define LED4_GPIO_PIN                       GPIO_Pin_2              //GREEN Led
#define LED4_GPIO_PIN_SOURCE                GPIO_PinSource2         //GREEN Led
#define LED4_GPIO_PORT                      GPIOA                   //GREEN Led
#define LED4_GPIO_CLK                       RCC_AHB1Periph_GPIOA    //GREEN Led
#if   PLATFORM_TEACUP_PIGTAIL_DEV == PLATFORM_ID
//On Pigtail board with BM-14, RGB lines are reversed
#define RGB_LINES_REVERSED
#endif
#endif

//Push Buttons
#define BUTTONn                             1
#if   PLATFORM_ID == PLATFORM_PHOTON_DEV
#define BUTTON1_GPIO_PIN                    GPIO_Pin_2
#define BUTTON1_GPIO_PORT                   GPIOC
#define BUTTON1_GPIO_CLK                    RCC_AHB1Periph_GPIOC
#define BUTTON1_GPIO_MODE		            GPIO_Mode_IN
#define BUTTON1_GPIO_PUPD                   GPIO_PuPd_UP
#define BUTTON1_PRESSED			            0x00
#define BUTTON1_EXTI_LINE                   EXTI_Line2
#define BUTTON1_EXTI_PORT_SOURCE            EXTI_PortSourceGPIOC
#define BUTTON1_EXTI_PIN_SOURCE             EXTI_PinSource2
#define BUTTON1_EXTI_IRQn                   EXTI2_IRQn
#define BUTTON1_EXTI_IRQ_HANDLER            EXTI2_IRQHandler
#define BUTTON1_EXTI_IRQ_PRIORITY           7
#define BUTTON1_EXTI_IRQ_INDEX              24
#define	BUTTON1_EXTI_TRIGGER		        EXTI_Trigger_Falling
#elif PLATFORM_TEACUP_PIGTAIL_DEV == PLATFORM_ID || \
      PLATFORM_PHOTON_PRODUCTION == PLATFORM_ID || \
      PLATFORM_TEACUP_PIGTAIL_PRODUCTION == PLATFORM_ID || \
      PLATFORM_P1 == PLATFORM_ID || \
      PLATFORM_ELECTRON_PRODUCTION == PLATFORM_ID
#define BUTTON1_GPIO_PIN                    GPIO_Pin_7
#define BUTTON1_GPIO_PORT                   GPIOC
#define BUTTON1_GPIO_CLK                    RCC_AHB1Periph_GPIOC
#define BUTTON1_GPIO_MODE                   GPIO_Mode_IN
#define BUTTON1_GPIO_PUPD                   GPIO_PuPd_UP
#define BUTTON1_PRESSED                     0x00
#define BUTTON1_EXTI_LINE                   EXTI_Line7
#define BUTTON1_EXTI_PORT_SOURCE            EXTI_PortSourceGPIOC
#define BUTTON1_EXTI_PIN_SOURCE             EXTI_PinSource7
#define BUTTON1_EXTI_IRQn                   EXTI9_5_IRQn
#define BUTTON1_EXTI_IRQ_HANDLER            EXTI9_5_IRQHandler
#define BUTTON1_EXTI_IRQ_PRIORITY           7
#define BUTTON1_EXTI_IRQ_INDEX              39
#define BUTTON1_EXTI_TRIGGER                EXTI_Trigger_Falling
#elif PLATFORM_DUO_PRODUCTION == PLATFORM_ID
#define BUTTON1_GPIO_PIN                    GPIO_Pin_2
#define BUTTON1_GPIO_PORT                   GPIOB
#define BUTTON1_GPIO_CLK                    RCC_AHB1Periph_GPIOB
#define BUTTON1_GPIO_MODE                   GPIO_Mode_IN
#define BUTTON1_GPIO_PUPD                   GPIO_PuPd_UP
#define BUTTON1_PRESSED                     0x00
#define BUTTON1_EXTI_LINE                   EXTI_Line2
#define BUTTON1_EXTI_PORT_SOURCE            EXTI_PortSourceGPIOB
#define BUTTON1_EXTI_PIN_SOURCE             EXTI_PinSource2
#define BUTTON1_EXTI_IRQn                   EXTI2_IRQn
#define BUTTON1_EXTI_IRQ_HANDLER            EXTI2_IRQHandler
#define BUTTON1_EXTI_IRQ_PRIORITY           7
#define BUTTON1_EXTI_IRQ_INDEX              24
#define BUTTON1_EXTI_TRIGGER                EXTI_Trigger_Falling
#endif

#define UI_TIMER_FREQUENCY                  100	/* 100Hz -> 10ms */
#define BUTTON_DEBOUNCE_INTERVAL            1000 / UI_TIMER_FREQUENCY

//USB OTG Peripheral
#if   PLATFORM_TEACUP_PIGTAIL_DEV == PLATFORM_ID || \
      PLATFORM_TEACUP_PIGTAIL_PRODUCTION == PLATFORM_ID || \
      PLATFORM_P1 == PLATFORM_ID || \
      PLATFORM_ELECTRON_PRODUCTION == PLATFORM_ID
//BM-14 and ELECTRON uses USB_OTG_FS peripheral
#define USE_USB_OTG_FS
//BM-14 has serial flash
#elif   PLATFORM_PHOTON_DEV == PLATFORM_ID || \
        PLATFORM_PHOTON_PRODUCTION == PLATFORM_ID || \
		PLATFORM_DUO_PRODUCTION == PLATFORM_ID
//BM-09 uses USB_OTG_HS peripheral
#define USE_USB_OTG_HS
#endif

#if   PLATFORM_TEACUP_PIGTAIL_DEV == PLATFORM_ID || \
      PLATFORM_TEACUP_PIGTAIL_PRODUCTION == PLATFORM_ID || \
      PLATFORM_P1 == PLATFORM_ID
	#define HAS_SERIAL_FLASH
    #define sFLASH_PAGESIZE     0x1000 /* 4096 bytes sector size that needs to be erased */
    #define sFLASH_PAGECOUNT    256    /* 1MByte storage */
#elif PLATFORM_DUO_PRODUCTION == PLATFORM_ID
	#define HAS_SERIAL_FLASH
	#define USE_SERIAL_FLASH
    #define sFLASH_PAGESIZE     0x1000 /* 4096 bytes sector size that needs to be erased */
    #define sFLASH_PAGECOUNT    512    /* 1MByte storage */
#endif

#define FLASH_UPDATE_MODULES

#ifdef HAS_SERIAL_FLASH
//SPI FLASH Interface pins
#if PLATFORM_DUO_PRODUCTION == PLATFORM_ID
#define sFLASH_SPI                          SPI2
#define sFLASH_SPI_CLK                      RCC_APB1Periph_SPI2
#define sFLASH_SPI_CLK_CMD                  RCC_APB1PeriphClockCmd
#define sFLASH_SPI_CS_GPIO_PIN              GPIO_Pin_12                 /* PB.12 */
#define sFLASH_SPI_CS_GPIO_PORT             GPIOB                       /* GPIOB */
#define sFLASH_SPI_CS_GPIO_CLK              RCC_AHB1Periph_GPIOB
#define sFLASH_SPI_SCK_GPIO_PIN             GPIO_Pin_13                 /* PB.13 */
#define sFLASH_SPI_SCK_GPIO_PORT            GPIOB                       /* GPIOB */
#define sFLASH_SPI_SCK_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define sFLASH_SPI_SCK_SOURCE               GPIO_PinSource13
#define sFLASH_SPI_SCK_AF                   GPIO_AF_SPI2
#define sFLASH_SPI_MISO_GPIO_PIN            GPIO_Pin_2                 /* PC.2 */
#define sFLASH_SPI_MISO_GPIO_PORT           GPIOC                       /* GPIOC */
#define sFLASH_SPI_MISO_GPIO_CLK            RCC_AHB1Periph_GPIOC
#define sFLASH_SPI_MISO_SOURCE              GPIO_PinSource2
#define sFLASH_SPI_MISO_AF                  GPIO_AF_SPI2
#define sFLASH_SPI_MOSI_GPIO_PIN            GPIO_Pin_3                 /* PC.3 */
#define sFLASH_SPI_MOSI_GPIO_PORT           GPIOC                       /* GPIOC */
#define sFLASH_SPI_MOSI_GPIO_CLK            RCC_AHB1Periph_GPIOC
#define sFLASH_SPI_MOSI_SOURCE              GPIO_PinSource3
#define sFLASH_SPI_MOSI_AF                  GPIO_AF_SPI2
#else
#define sFLASH_SPI                          SPI2
#define sFLASH_SPI_CLK                      RCC_APB1Periph_SPI2
#define sFLASH_SPI_CLK_CMD                  RCC_APB1PeriphClockCmd
#define sFLASH_SPI_CS_GPIO_PIN              GPIO_Pin_12                 /* PB.12 */
#define sFLASH_SPI_CS_GPIO_PORT             GPIOB                       /* GPIOB */
#define sFLASH_SPI_CS_GPIO_CLK              RCC_AHB1Periph_GPIOB
#define sFLASH_SPI_SCK_GPIO_PIN             GPIO_Pin_13                 /* PB.13 */
#define sFLASH_SPI_SCK_GPIO_PORT            GPIOB                       /* GPIOB */
#define sFLASH_SPI_SCK_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define sFLASH_SPI_SCK_SOURCE               GPIO_PinSource13
#define sFLASH_SPI_SCK_AF                   GPIO_AF_SPI2
#define sFLASH_SPI_MISO_GPIO_PIN            GPIO_Pin_14                 /* PB.14 */
#define sFLASH_SPI_MISO_GPIO_PORT           GPIOB                       /* GPIOB */
#define sFLASH_SPI_MISO_GPIO_CLK            RCC_AHB1Periph_GPIOB
#define sFLASH_SPI_MISO_SOURCE              GPIO_PinSource14
#define sFLASH_SPI_MISO_AF                  GPIO_AF_SPI2
#define sFLASH_SPI_MOSI_GPIO_PIN            GPIO_Pin_15                 /* PB.15 */
#define sFLASH_SPI_MOSI_GPIO_PORT           GPIOB                       /* GPIOB */
#define sFLASH_SPI_MOSI_GPIO_CLK            RCC_AHB1Periph_GPIOB
#define sFLASH_SPI_MOSI_SOURCE              GPIO_PinSource15
#define sFLASH_SPI_MOSI_AF                  GPIO_AF_SPI2
#endif

#define sFLASH_SPI_BAUDRATE_PRESCALER       SPI_BaudRatePrescaler_4
#endif

//NVIC Priorities based on NVIC_PriorityGroup_4
#define SDIO_IRQ_PRIORITY                   0       //??? BCM43362 SDIO Interrupt
#ifdef USE_USB_OTG_FS
#define OTG_FS_IRQ_PRIORITY                 2       //USB OTG FS Interrupt
#define OTG_FS_WKUP_IRQ_PRIORITY            2       //USB OTG FS Wakeup Interrupt
#elif defined USE_USB_OTG_HS
#define OTG_HS_EP1_IN_IRQ_PRIORITY          2       //USB OTG HS EP1 IN Interrupt
#define OTG_HS_EP1_OUT_IRQ_PRIORITY         2       //USB OTG HS EP1 OUT Interrupt
#define OTG_HS_IRQ_PRIORITY                 2       //USB OTG HS Interrupt
#define OTG_HS_WKUP_IRQ_PRIORITY            2       //USB OTG HS Wakeup Interrupt
#endif
#define RTC_Alarm_IRQ_PRIORITY              3       //RTC Alarm Interrupt
#define RTC_WKUP_IRQ_PRIORITY               4       //RTC Seconds Interrupt
#define USART1_IRQ_PRIORITY                 5       //USART1 Interrupt
#define USART2_IRQ_PRIORITY                 5       //USART2 Interrupt
#define TIM2_IRQ_PRIORITY                   6       //TIM2 CC Interrupt(Button Use)
//BUTTON1_EXTI_IRQ_PRIORITY                 7       //Mode Button Interrupt
#define EXTI15_10_IRQ_PRIORITY              8       //User Interrupt
#define EXTI9_5_IRQ_PRIORITY                9       //User Interrupt
#define EXTI0_IRQ_PRIORITY                  10      //User Interrupt
#define EXTI1_IRQ_PRIORITY                  10      //User Interrupt
#define EXTI2_IRQ_PRIORITY                  10      //User Interrupt
#define EXTI3_IRQ_PRIORITY                  10      //User Interrupt
#define EXTI4_IRQ_PRIORITY                  10      //User Interrupt
#define SYSTICK_IRQ_PRIORITY                13      //CORTEX_M3 Systick Interrupt
#define SVCALL_IRQ_PRIORITY                 14      //CORTEX_M3 SVCall Interrupt
#define PENDSV_IRQ_PRIORITY                 15      //CORTEX_M3 PendSV Interrupt

#define PREPSTRING2(x) #x
#define PREPSTRING(x) PREPSTRING2(x)

#if PLATFORM_ID == PLATFORM_SPARK_CORE
#define INTERNAL_FLASH_SIZE                 (0x20000)
#elif PLATFORM_ID == PLATFORM_SPARK_CORE_HD
    #define INTERNAL_FLASH_SIZE             (0x40000)
#elif PLATFORM_ID == PLATFORM_PHOTON_DEV
    #define INTERNAL_FLASH_SIZE             (0x100000)
#elif PLATFORM_ID == PLATFORM_TEACUP_PIGTAIL_DEV
    #define INTERNAL_FLASH_SIZE             (0x100000)
#elif   PLATFORM_ID == PLATFORM_PHOTON_PRODUCTION || \
        PLATFORM_ID == PLATFORM_P1 || \
        PLATFORM_ID == PLATFORM_ELECTRON_PRODUCTION || \
		PLATFORM_ID == PLATFORM_DUO_PRODUCTION
    #define INTERNAL_FLASH_SIZE             (0x100000)
#elif PLATFORM_ID == PLATFORM_TEACUP_PIGTAIL_PRODUCTION
    #define INTERNAL_FLASH_SIZE             (0x100000)
#else
    #pragma message "PLATFORM_ID is " PREPSTRING(PLATFORM_ID)
    #error "Unknown PLATFORM_ID"
#endif

/* Exported functions ------------------------------------------------------- */

#endif /* __PLATFORM_CONFIG_H */
