#ifndef _BOARD_H_
#define _BOARD_H_

// Board identifier.
#define BOARD_FIREBULL_STM32_F103
#define BOARD_NAME              "FireBull STM32F103-FB"

// Board frequencies.
#define STM32_LSECLK            32768
#define STM32_HSECLK            8000000

// MCU type, supported types are defined in ./os/hal/platforms/hal_lld.h.
#define STM32F10X_HD

// IO pins assignments.
#define GPIOA_BUTTON         8

#define GPIOD_LED1          8
#define GPIOD_LED2          9
#define GPIOD_LED3          10
#define GPIOD_LED4			11

#define GPIOA_SPI1NSS		4
#define GPIOB_SPI2NSS       12
#define GPIOA_SPI3NSS		15
#define GPIOA_MMCCP         4
#define GPIOC_SDPWR			8
#define GPIOC_USB_EN		9

#define TP_CS_PORT			GPIOC
#define TP_IRQ_PORT			GPIOC
#define TP_IRQ				4
#define TP_CS				6

#define GDISP_DATA_PORT			GPIOE
#define GDISP_CMD_PORT			GPIOD
#define GDISP_CS					12
#define GDISP_RS					13
#define GDISP_WR					14
#define GDISP_RD					15

    /*
    * I/O ports initial setup, this configuration is established soon after reset
    * in the initialization code.
    *
    * The digits have the following meaning:
    *   0 - Analog input.
    *   1 - Push Pull output 10MHz.
    *   2 - Push Pull output 2MHz.
    *   3 - Push Pull output 50MHz.
    *   4 - Digital input.
    *   5 - Open Drain output 10MHz.
    *   6 - Open Drain output 2MHz.
    *   7 - Open Drain output 50MHz.
    *   8 - Digital input with PullUp or PullDown resistor depending on ODR.
    *   9 - Alternate Push Pull output 10MHz.
    *   A - Alternate Push Pull output 2MHz.
    *   B - Alternate Push Pull output 50MHz.
    *   C - Reserved.
    *   D - Alternate Open Drain output 10MHz.
    *   E - Alternate Open Drain output 2MHz.
    *   F - Alternate Open Drain output 50MHz.
    * Please refer to the STM32 Reference Manual for details.
    */

#define VAL_GPIOACRL            0xB4B44B34      /*  PA7...PA0 */
#define VAL_GPIOACRH            0x444334B4      /* PA15...PA8 */
#define VAL_GPIOAODR            0xFFFFFFFF

#define VAL_GPIOBCRL            0x88888888      /*  PB7...PB0 */
#define VAL_GPIOBCRH            0xB4B88888      /* PB15...PB8 */
#define VAL_GPIOBODR            0xFFFFFFFF

#define VAL_GPIOCCRL            0x33048888      /*  PC7...PC0 */
#define VAL_GPIOCCRH            0x88833383      /* PC15...PC8 */
#define VAL_GPIOCODR            0xFFFFFFFF

#define VAL_GPIODCRL            0x33388888      /*  PD7...PD0 */
#define VAL_GPIODCRH            0x33333333      /* PD15...PD8 */
#define VAL_GPIODODR            0xFFFFFFFF

#define VAL_GPIOECRL            0x33333333      /*  PE7...PE0 */
#define VAL_GPIOECRH            0x33333333      /* PE15...PE8 */
#define VAL_GPIOEODR            0xFFFFFFFF

#define usb_lld_connect_bus(usbp) palClearPad(GPIOC, GPIOC_USB_EN)
#define usb_lld_disconnect_bus(usbp) palSetPad(GPIOC, GPIOC_USB_EN)

#if !defined(_FROM_ASM_)
#ifdef __cplusplus
extern "C" {
	#endif
	void boardInit(void);
	#ifdef __cplusplus
}
#endif
#endif /* _FROM_ASM_ */

#endif /* _BOARD_H_ */

