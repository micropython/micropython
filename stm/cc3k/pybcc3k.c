#include <stdint.h>

#include "stm32f4xx_rcc.h"
#include <stm32f4xx_syscfg.h>
#include "stm32f4xx_gpio.h"
#include <stm32f4xx_exti.h>
#include <stm_misc.h>
#include "stm32f4xx_spi.h"

#include "misc.h"
#include "systick.h"

#include "ccdebug.h"
#include "pybcc3k.h"

// IRQ on PA14, input, pulled up, active low
// EN on PC7, output, active high
// CS on PC6, output, active low
// SPI2 on PB15=MOSI, PB14=MISO, PB13=SCK
// SCK for CC3000: max 16MHz, low when idle, data sampled on falling edge

// TODO this could be really wrong wrt calibration
void pyb_delay_us(uint32_t usec) {
    volatile uint32_t count = 0;
    const uint32_t utime = (160 * usec / 5);
    do {
        if (++count > utime) {
            return;
        }
    } while (1);
}

void pyb_cc3000_set_en(int val) {
    DEBUG_printf("pyb_cc3000_set_en val=%d\n", val);
    if (val) {
        GPIOC->BSRRL = GPIO_Pin_7; // set pin high
    } else {
        GPIOC->BSRRH = GPIO_Pin_7; // set pin low
    }
}

void pyb_cc3000_set_cs(int val) {
    DEBUG_printf("pyb_cc3000_set_cs val=%d\n", val);
    if (val) {
        GPIOC->BSRRL = GPIO_Pin_6; // set pin high
    } else {
        GPIOC->BSRRH = GPIO_Pin_6; // set pin low
    }
}

int pyb_cc3000_get_irq(void) {
    if ((GPIOA->IDR & GPIO_Pin_14) == 0) {
        return 0;
    } else {
        return 1;
    }
}

uint32_t exti14_enabled = 0; // TODO hack; do it properly!
uint32_t exti14_missed = 0; // TODO hack; do it properly!

void pyb_cc3000_enable_irq(void) {
    DEBUG_printf("pyb_cc3000_enable_irq: en=%lu miss=%lu\n", exti14_enabled, exti14_missed);
    if (exti14_missed) {
        /* doesn't look like this is needed
        DEBUG_printf("pyb_cc3000_enable_irq: handling missed IRQ\n");
        // TODO hack if we have a pending IRQ
        extern void SpiIntGPIOHandler(void);
        SpiIntGPIOHandler();
        */
        exti14_missed = 0;
    }
    exti14_enabled = 1;
}

void pyb_cc3000_disable_irq(void) {
    DEBUG_printf("pyb_cc3000_disable_irq: en=%lu miss=%lu\n", exti14_enabled, exti14_missed);
    exti14_enabled = 0;
}

void pyb_cc3000_pause_spi(void) {
    DEBUG_printf("pyb_cc3000_pause_spi\n");
    exti14_enabled = 0;
}

void pyb_cc3000_resume_spi(void) {
    DEBUG_printf("pyb_cc3000_resume_spi\n");
    exti14_enabled = 1;
}

static void EXTILine14_Config(void) {
    /* Enable SYSCFG clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    /* Configure PA14 pin as pulled-up input */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Connect EXTI Line14 to PA14 pin */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource14);

    /* Configure EXTI Line14, falling edge */
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line14;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable and set EXTI15_10 Interrupt to the lowest priority */
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void pyb_cc3000_spi_init(void) {
    DEBUG_printf("pyb_cc3000_spi_init\n");

    // enable SPI clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    // GPIO clocks should already be enabled

    /*!< SPI pins configuration *************************************************/

    /*!< Connect SPI pins to AF5 */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /*
  inf.baudRate = 100000; // FIXME - just slow for debug
  inf.spiMode = SPIF_SPI_MODE_1;  // Mode 1   CPOL= 0  CPHA= 1
  */

    /*!< SPI configuration */
    SPI_InitTypeDef SPI_InitStructure;
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; // should be correct
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low; // clock is low when idle
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; // data latched on second edge, which is falling edge for low-idle
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; // software control
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; // clock freq = f_PCLK / this_prescale_value
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; // should be correct
    SPI_InitStructure.SPI_CRCPolynomial = 7; // ?
    SPI_Init(SPI2, &SPI_InitStructure);

    /*!< Enable the SPI  */
    SPI_Cmd(SPI2, ENABLE);

  /*
  // WLAN CS, EN and WALN IRQ Configuration
  jshSetPinStateIsManual(WLAN_CS_PIN, MP_FALSE);
  jshPinOutput(WLAN_CS_PIN, 1); // de-assert CS
  jshSetPinStateIsManual(WLAN_EN_PIN, MP_FALSE);
  jshPinOutput(WLAN_EN_PIN, 0); // disable WLAN
  jshSetPinStateIsManual(WLAN_IRQ_PIN, MP_TRUE);
  jshPinSetState(WLAN_IRQ_PIN, JSHPINSTATE_GPIO_IN_PULLUP); // flip into read mode with pullup
  */
    // configure wlan CS and EN pins
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    pyb_cc3000_set_cs(1); // de-assert CS
    pyb_cc3000_set_en(0); // disable wlan

    // configure EXTI on A14
    EXTILine14_Config();

    // wait a little (ensure that WLAN takes effect)
    sys_tick_delay_ms(500); // force a 500ms delay! FIXME
}

uint8_t pyb_cc3000_spi_send(uint8_t val) {
    /*!< Loop while DR register in not emplty */
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);

    /*!< Send byte through the SPI1 peripheral */
    SPI_I2S_SendData(SPI2, val);

    /*!< Wait to receive a byte */
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

    /*!< Return the byte read from the SPI bus */
    return SPI_I2S_ReceiveData(SPI2);
}
