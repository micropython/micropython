/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

#include STM32_HAL_H
#include "py/mphal.h"
#include "pin.h"
#include "genhdr/pins.h"
#include "spi.h"

SPI_HandleTypeDef ili_spi;

#if MICROPY_HW_UGFX_INTERFACE == UGFX_DRIVER_PARALLEL

#define FMC_WRITE_FIFO_ENABLE_BODGE FMC_BCR1_WFDIS
//this line appears to be missing from stm32l476xx.h
#define  FMC_BCR1_WFDIS                     ((uint32_t)0x00200000)        /*!<Write FIFO Disable         */

#define LCD_RAM (*((volatile uint8_t *)(FMC_BASE_ADDR + (1<<FMC_ADDR_PIN))))
#define LCD_REG (*((volatile uint8_t *)(FMC_BASE_ADDR)))

static void HAL_FMC_MspInit(void){
  /* USER CODE BEGIN FMC_MspInit 0 */

  /* USER CODE END FMC_MspInit 0 */
  GPIO_InitTypeDef GPIO_InitStruct;
  //if (FMC_Initialized) {
  //  return;
  //}
  //FMC_Initialized = 1;
  /* Peripheral clock enable */
  __FMC_CLK_ENABLE();
  
  /** FMC GPIO Configuration  
  PE7   ------> FMC_D4
  PE8   ------> FMC_D5
  PE9   ------> FMC_D6
  PE10   ------> FMC_D7
  PD14   ------> FMC_D0
  PD15   ------> FMC_D1
  PD0   ------> FMC_D2
  PD1   ------> FMC_D3
  PD4   ------> FMC_NOE
  PD5   ------> FMC_NWE
  PD7   ------> FMC_NE1
  */
  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9 
                          |GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = MICROPY_HW_UGFX_PIN_A0;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(MICROPY_HW_UGFX_PORT_A0, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_0|GPIO_PIN_1 
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* USER CODE BEGIN FMC_MspInit 1 */

  /* USER CODE END FMC_MspInit 1 */
}
#endif

static GFXINLINE void init_board(GDisplay *g) {
	// As we are not using multiple displays we set g->board to NULL as we don't use it.

	GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
	

	
	
	#if MICROPY_HW_UGFX_INTERFACE == UGFX_DRIVER_SPI
	ili_spi = MICROPY_HW_UGFX_SPI;
	
	// init the SPI bus
    ili_spi.Init.Mode = SPI_MODE_MASTER;

    // compute the baudrate prescaler from the desired baudrate
    // select a prescaler that yields at most the desired baudrate
    uint spi_clock;
    if ((ili_spi.Instance == SPI2) || (ili_spi.Instance == SPI3)) {
        // SPI2/3 is on APB1
        spi_clock = HAL_RCC_GetPCLK1Freq();
    } else {
        // SPI1 and SPI4 are on APB2
        spi_clock = HAL_RCC_GetPCLK2Freq();
    }
    uint br_prescale = spi_clock / 10000000;
    if (br_prescale <= 2) { ili_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2; }
    else if (br_prescale <= 4) { ili_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4; }
    else if (br_prescale <= 8) { ili_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8; }
    else if (br_prescale <= 16) { ili_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16; }
    else if (br_prescale <= 32) { ili_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32; }
    else if (br_prescale <= 64) { ili_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64; }
    else if (br_prescale <= 128) { ili_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128; }
    else { ili_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256; }

    // data is sent bigendian, latches on rising clock
    ili_spi.Init.CLKPolarity = SPI_POLARITY_LOW;
    ili_spi.Init.CLKPhase = SPI_PHASE_1EDGE;
    ili_spi.Init.Direction = SPI_DIRECTION_2LINES;
    ili_spi.Init.DataSize = SPI_DATASIZE_8BIT;
    ili_spi.Init.NSS = SPI_NSS_SOFT;
    ili_spi.Init.FirstBit = SPI_FIRSTBIT_MSB;
    ili_spi.Init.TIMode = SPI_TIMODE_DISABLED;
    ili_spi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
    ili_spi.Init.CRCPolynomial = 0;


    // init the SPI bus
    spi_init(&ili_spi, false);

    // set the pins to default values
    GPIO_set_pin(MICROPY_HW_UGFX_PORT_CS, MICROPY_HW_UGFX_PIN_CS);
	GPIO_set_pin(MICROPY_HW_UGFX_PORT_A0, MICROPY_HW_UGFX_PIN_A0);

    // init the pins to be push/pull outputs   
    GPIO_InitStructure.Pin = MICROPY_HW_UGFX_PIN_CS;
    HAL_GPIO_Init(MICROPY_HW_UGFX_PORT_CS, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin = MICROPY_HW_UGFX_PIN_A0;
    HAL_GPIO_Init(MICROPY_HW_UGFX_PORT_A0, &GPIO_InitStructure);

	
	#else
			
	FMC_NORSRAM_TimingTypeDef Timing;

	/** Perform the SRAM1 memory initialization sequence
	*/
	SRAM_HandleTypeDef hsram1;
	hsram1.Instance = FMC_NORSRAM_DEVICE;
	hsram1.Extended = FMC_NORSRAM_EXTENDED_DEVICE;
	/* hsram1.Init */
	hsram1.Init.NSBank = FMC_NORSRAM_BANK1;
	hsram1.Init.DataAddressMux = FMC_DATA_ADDRESS_MUX_DISABLE;
	hsram1.Init.MemoryType = FMC_MEMORY_TYPE_SRAM;
	hsram1.Init.MemoryDataWidth = FMC_NORSRAM_MEM_BUS_WIDTH_8;
	hsram1.Init.BurstAccessMode = FMC_BURST_ACCESS_MODE_DISABLE;
	hsram1.Init.WaitSignalPolarity = FMC_WAIT_SIGNAL_POLARITY_LOW;
	hsram1.Init.WaitSignalActive = FMC_WAIT_TIMING_BEFORE_WS;
	hsram1.Init.WriteOperation = FMC_WRITE_OPERATION_ENABLE;
	hsram1.Init.WaitSignal = FMC_WAIT_SIGNAL_DISABLE;
	hsram1.Init.ExtendedMode = FMC_EXTENDED_MODE_DISABLE;
	hsram1.Init.AsynchronousWait = FMC_ASYNCHRONOUS_WAIT_DISABLE;
	hsram1.Init.WriteBurst = FMC_WRITE_BURST_DISABLE;
	hsram1.Init.ContinuousClock = FMC_CONTINUOUS_CLOCK_SYNC_ONLY;
	hsram1.Init.WriteFifo = FMC_WRITE_FIFO_ENABLE_BODGE;
	hsram1.Init.PageSize = FMC_PAGE_SIZE_NONE;
	/* Timing */
	//TODO: check these against the datasheet
	Timing.AddressSetupTime = 1;
	Timing.AddressHoldTime = 0;
	Timing.DataSetupTime = 5;
	Timing.BusTurnAroundDuration = 0;
	Timing.CLKDivision = 0;
	Timing.DataLatency = 0;
	Timing.AccessMode = FMC_ACCESS_MODE_A;

	HAL_FMC_MspInit();

	HAL_SRAM_Init(&hsram1, &Timing, NULL);		
	#endif
	
	
	//configure the RST and backlight pins
    GPIO_set_pin(MICROPY_HW_UGFX_PORT_RST, MICROPY_HW_UGFX_PIN_RST);
    GPIO_set_pin(MICROPY_HW_UGFX_PORT_BL, MICROPY_HW_UGFX_PIN_BL);	
	
	GPIO_InitStructure.Pin = MICROPY_HW_UGFX_PIN_RST;
    HAL_GPIO_Init(MICROPY_HW_UGFX_PORT_RST, &GPIO_InitStructure);
    GPIO_InitStructure.Pin = MICROPY_HW_UGFX_PIN_BL;
    HAL_GPIO_Init(MICROPY_HW_UGFX_PORT_BL, &GPIO_InitStructure);
	

	
	#ifdef MICROPY_HW_UGFX_PIN_MODE
	GPIO_InitStructure.Pin = MICROPY_HW_UGFX_PIN_MODE;
	HAL_GPIO_Init(MICROPY_HW_UGFX_PORT_MODE, &GPIO_InitStructure);
	MICROPY_HW_UGFX_SET_MODE;
	#endif

	
}

static GFXINLINE void post_init_board(GDisplay *g) {
	(void) g;
}

static GFXINLINE void setpin_reset(GDisplay *g, bool_t state) {
	(void) g;
	if(state) {
		// reset lcd
		GPIO_clear_pin(MICROPY_HW_UGFX_PORT_RST, MICROPY_HW_UGFX_PIN_RST);
	} else {
		GPIO_set_pin(MICROPY_HW_UGFX_PORT_RST, MICROPY_HW_UGFX_PIN_RST);
	}
}


static GFXINLINE void set_backlight(GDisplay *g, uint8_t percent) {
	(void) g;
	// TODO: can probably pwm this
	if(percent) {
		// turn back light on
		GPIO_set_pin(MICROPY_HW_UGFX_PORT_BL, MICROPY_HW_UGFX_PIN_BL);
	} else {
		// turn off
		GPIO_clear_pin(MICROPY_HW_UGFX_PORT_BL, MICROPY_HW_UGFX_PIN_BL);
	}
}

static GFXINLINE void acquire_bus(GDisplay *g) {
	(void) g;
}



#if MICROPY_HW_UGFX_INTERFACE == UGFX_DRIVER_SPI
static GFXINLINE void release_bus(GDisplay *g) {
	(void) g;
	GPIO_set_pin(MICROPY_HW_UGFX_PORT_CS, MICROPY_HW_UGFX_PIN_CS);  //CS high
}
static GFXINLINE void write_index(GDisplay *g, uint16_t index) {
	(void) g;
	
	GPIO_clear_pin(MICROPY_HW_UGFX_PORT_CS, MICROPY_HW_UGFX_PIN_CS);  //CS low
	GPIO_clear_pin(MICROPY_HW_UGFX_PORT_A0, MICROPY_HW_UGFX_PIN_A0);  //CMD low
	HAL_SPI_Transmit(&ili_spi, &index, 1, 1000);
	GPIO_set_pin(MICROPY_HW_UGFX_PORT_A0, MICROPY_HW_UGFX_PIN_A0);  //CMD high
}
static GFXINLINE void write_data(GDisplay *g, uint16_t data) {
	(void) g;
	HAL_SPI_Transmit(&ili_spi, &data, 1, 1000);
}
static GFXINLINE uint16_t read_data(GDisplay *g) {
	(void) g;
	uint8_t d;
	HAL_SPI_Receive(&ili_spi, &d, 1, 1000);
	return d;
}
#elif MICROPY_HW_UGFX_INTERFACE == UGFX_DRIVER_PARALLEL
static GFXINLINE void release_bus(GDisplay *g) {
	(void) g;
}
static GFXINLINE void write_index(GDisplay *g, uint16_t index) {
	(void) g;	
	LCD_REG = index;
}
static GFXINLINE void write_data(GDisplay *g, uint8_t data) {
	(void) g;
	LCD_RAM = data;
}
static GFXINLINE uint8_t read_data(GDisplay *g) {
	(void) g;
	uint8_t d = LCD_RAM;
	return d;
}
#else
#error "Select SPI or PARALLEL for UGFX driver type"
#endif


static GFXINLINE void setreadmode(GDisplay *g) {
	(void) g;
}

static GFXINLINE void setwritemode(GDisplay *g) {
	(void) g;
}



#endif /* _GDISP_LLD_BOARD_H */
