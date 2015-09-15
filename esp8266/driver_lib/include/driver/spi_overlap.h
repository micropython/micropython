#ifndef SPI_OVERLAP_APP_H
#define SPI_OVERLAP_APP_H

#include "ets_sys.h"
#include "spi_flash.h"
#define HSPI_OVERLAP
//#define NO_HSPI_DEVICE
#define HOST_INF_SEL 0x3ff00028 
#define FUNC_SPI_CS2 1
#define FUNC_SPI_CS1 1
#define reg_cspi_overlap  (BIT7)

#define SPI_FLASH_BYTES_LEN                24
#define IODATA_START_ADDR                 BIT0
#define SPI_BUFF_BYTE_NUM                    32

#define PERIPHS_IO_MUX_BACKUP		0
#define SPI_USER_BACKUP  	1
#define SPI_CTRL_BACKUP  	2 
#define SPI_CLOCK_BACKUP 	3
#define SPI_USER1_BACKUP	4
#define SPI_USER2_BACKUP	5
#define SPI_CMD_BACKUP		6
#define SPI_PIN_BACKUP		7
#define SPI_SLAVE_BACKUP	8

#define HSPI_CS_DEV			0
#define SPI_CS1_DEV			1
#define SPI_CS2_DEV			2
#define SPI_CS0_FLASH		3
#define HSPI_IDLE			4

struct hspi_device_config{
	uint8 active:1;
	uint8 clk_polar:1;
	uint8 res:1;
	uint8 clk_div:5;
};

struct hspi_device_register{
	uint32 hspi_flash_reg_backup[9];
	uint32 hspi_dev_reg_backup[9];
	struct hspi_device_config hspi_dev_conf[4];
	uint8 selected_dev_num:3;
	uint8 spi_io_80m:1;
	uint8 hspi_reg_backup_flag:1;
	uint8 res:3;
};

void hspi_overlap_init(void);
void hspi_overlap_deinit(void);
void spi_reg_recover(uint8 spi_no,uint32* backup_mem);
void spi_reg_backup(uint8 spi_no,uint32* backup_mem);

void hspi_master_dev_init(uint8 dev_no,uint8 clk_polar,uint8 clk_div);
void hspi_dev_sel(uint8 dev_no);

void hspi_overlap_flash_init(void);
SpiFlashOpResult hspi_overlap_read_flash_data(SpiFlashChip * spi, uint32 flash_addr, uint32 * addr_dest, uint32 byte_length);

#endif
