#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

// Pin & SPI setup
#define SPI_DRIVER  (&SPID2)
#define SPI_PORT    GPIOB
#define SCK_PAD     10
#define MOSI_PAD    15

#define CS_PORT     GPIOB
#define RESET_PORT  GPIOB
#define DC_PORT     GPIOB
#define CS_PAD      3        // 0 = chip selected
#define RESET_PAD   4        // 0 = reset
#define DC_PAD      5        // control = 0, data = 1

static SPIConfig spi_cfg = {
	NULL,
	CS_PORT,
	CS_PAD,
	SPI_CR1_BR_1
};

static GFXINLINE void init_board(GDisplay* g)
{
	(void)g;

	g->board = 0;

	// Set pin modes
	palSetPadMode(SPI_PORT, SCK_PAD, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);
	palSetPadMode(SPI_PORT, MOSI_PAD, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);
	palSetPadMode(RESET_PORT, RESET_PAD, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
	palSetPadMode(CS_PORT, CS_PAD, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
	palSetPadMode(DC_PORT, DC_PAD, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);

	// Apply default states
	palSetPad(CS_PORT, CS_PAD);
	palSetPad(RESET_PORT, RESET_PAD);
	palClearPad(DC_PORT, DC_PAD);

	// Start the SPI driver
	spiStart(SPI_DRIVER, &spi_cfg);
}

static GFXINLINE void post_init_board(GDisplay* g)
{
	(void)g;
}


static GFXINLINE void setpin_reset(GDisplay* g, bool_t state)
{
	(void)g;

	palWritePad(RESET_PORT, RESET_PAD, !state);
}

static GFXINLINE void acquire_bus(GDisplay* g)
{
	(void)g;

	spiAcquireBus(SPI_DRIVER);
	spiSelect(SPI_DRIVER);
}

static GFXINLINE void release_bus(GDisplay* g)
{
	(void)g;

	spiUnselect(SPI_DRIVER);
	spiReleaseBus(SPI_DRIVER);
}

static GFXINLINE void write_cmd(GDisplay* g, uint8_t cmd)
{
	(void)g;

	static uint8_t buf;
	palClearPad(DC_PORT, DC_PAD);
	buf = cmd;
	spiSend(SPI_DRIVER, 1, &buf);
}

static GFXINLINE void write_data(GDisplay* g, uint8_t* data, uint16_t length)
{
	(void)g;

	palSetPad(DC_PORT, DC_PAD);
	spiSend(SPI_DRIVER, length, data);
}

#endif /* _GDISP_LLD_BOARD_H */
