// The code is this file allows the user to enter DFU mode when the board
// starts up, by connecting POS10 on the external connector to GND.
// The code itself is taken from the LimiFrog software repository found at
// https://github.com/LimiFrog/LimiFrog-SW, and the original license header
// is copied below.

#include STM32_HAL_H

static void LBF_DFU_If_Needed(void);

void LIMIFROG_board_early_init(void) {
    LBF_DFU_If_Needed();
}

/*******************************************************************************
 * LBF_DFU_If_Needed.c
 *
 * (c)2015 LimiFrog / CYMEYA
 * This program is licensed under the terms of the MIT License.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
 * Please refer to the License File LICENSE.txt located at the root of this
 * project for full licensing conditions,
 * or visit https://opensource.org/licenses/MIT.
 ******************************************************************************/

#define __LIMIFROG_02

/*  ==== BTLE (excl UART)       ======================================== */
// PC9 = BT_RST (active high)

#define BT_RST_PIN  GPIO_PIN_9
#define BT_RST_PORT GPIOC

// Position 10
#ifdef __LIMIFROG_01
  #define   CONN_POS10_PIN      GPIO_PIN_9
  #define   CONN_POS10_PORT     GPIOB
#else
  #define   CONN_POS10_PIN      GPIO_PIN_8
  #define   CONN_POS10_PORT     GPIOB
#endif

static inline void  GPIO_HIGH(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
       GPIOx->BSRR = (uint32_t)GPIO_Pin;
}

static inline int  IS_GPIO_RESET(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    GPIO_PinState bitstatus;
      if((GPIOx->IDR & GPIO_Pin) != (uint32_t)GPIO_PIN_RESET)
            {
                    bitstatus = GPIO_PIN_SET;
                      }
        else
              {
                      bitstatus = GPIO_PIN_RESET;
                        }
          return (bitstatus==GPIO_PIN_RESET);
}

/**************************************************************
 RATIONALE FOR THIS FUNCTION :

 - The STM32 embeds in ROM a bootloader that allows to
   obtain code and boot from a number of different interfaces,
   including USB in a mode called "DFU" (Device Frimware Update)
   [see AN3606 from ST for full details]
   This bootloader code is executed instead of the regular
   application code when pin BOOT0 is pulled-up (which on
   LimiFrog0.2 is achieved by pressing the general-purpose
   pushbutton switch on the side.
 - The bootloader monitors a number of IOs of the STM32 to decide
   from which interface it should boot.
 - Problem in LimiFrog (up to versions 0.2a at least): upon
   power-up the BLE modules generates some activity on UART3,
   which is part of the pins monitored by the STM32.
   This misleads the bootloader in trying to boot from UART3
   and, as a result, not continuing with booting from USB.

  - This code implements an alternative solution to launch the
  bootloader while making sure UART3 remains stable.
  - The idea it to start application code with a check, prior to any
  other applicative code, of whether USB bootload is required (as
  flagged by a GPIO pulled low at reset, in the same way as BOOT0).
  The hadware reset pin of BLE is asserted (so that now it won't
  generate any acitivity on UART3), and if USB bootload is required :
  bootload ROM is remapped at address 0x0, stack pointer is
  updated and the code is branched to the start of the bootloader.
  - This code is run prior to any applicative configuration of clocks,
  IRQs etc. -- the STM32 is therefore still running from MSI

  THIS FUNCTION MAY BE SUPPRESSED IF YOU NEVER NEED TO BOOT DFU MODE

  ********************************************************************/

static void LBF_DFU_If_Needed(void)
{


 GPIO_InitTypeDef GPIO_InitStruct;


   // Initialize and assert pin BTLE_RST
   // (hw reset to BLE module, so it won't drive UART3)

    __GPIOC_CLK_ENABLE();
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    GPIO_InitStruct.Pin = BT_RST_PIN;
    HAL_GPIO_Init(BT_RST_PORT, &GPIO_InitStruct);

    GPIO_HIGH(BT_RST_PORT, BT_RST_PIN); // assert BTLE reset


 /* -- Bootloader will be called if position 10 on the extension port
       is actively pulled low -- */
       // Note - this is an arbitrary choice, code could be modified to
       // monitor another GPIO of the STM32 and/or decide that active level
       // is high rather than low


    // Initialize Extension Port Position 10 = PB8 (bears I2C1_SCL)
    // Use weak pull-up to detect if pin is externally pulled low

    __GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Pin = CONN_POS10_PIN;
    HAL_GPIO_Init(CONN_POS10_PORT, &GPIO_InitStruct);

    // If selection pin pulled low...
    if ( IS_GPIO_RESET(CONN_POS10_PORT, CONN_POS10_PIN ))

    {
        // Remap bootloader ROM (ie System Flash) to address 0x0
        SYSCFG->MEMRMP = 0x00000001;

        // Init stack pointer with value residing at ROM base
    asm (
        "LDR     R0, =0x00000000\n\t"  // load ROM base address"
        "LDR     SP,[R0, #0]\n\t"      // assign main stack pointer"
        );

        // Jump to address pointed by 0x00000004 -- */

    asm (
        "LDR     R0,[R0, #4]\n\t"      // load bootloader address
        "BX      R0\n\t"
        );

    }
}
