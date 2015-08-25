#include "driver/spi_overlap.h"
#include "driver/spi.h"
#include "gpio.h"

#define SPI_FLASH_READ_MODE_MASK 0x196000
#define WAIT_HSPI_IDLE() 	while(READ_PERI_REG(SPI_EXT2(HSPI))||(READ_PERI_REG(SPI_CMD(HSPI))&0xfffc0000));
#define CONF_HSPI_CLK_DIV(div)	WRITE_PERI_REG(SPI_CLOCK(HSPI), (((div<<1)+1)<<12)+(div<<6)+(div<<1)+1)
#define HSPI_FALLING_EDGE_SAMPLE()		SET_PERI_REG_MASK(SPI_USER(HSPI),  SPI_CK_OUT_EDGE)
#define HSPI_RISING_EDGE_SAMPLE()			CLEAR_PERI_REG_MASK(SPI_USER(HSPI),  SPI_CK_OUT_EDGE)
#define ACTIVE_HSPI_CS0	 	CLEAR_PERI_REG_MASK(SPI_PIN(HSPI), SPI_CS0_DIS);\
						SET_PERI_REG_MASK(SPI_PIN(HSPI), SPI_CS1_DIS |SPI_CS2_DIS)
#define ACTIVE_HSPI_CS1		CLEAR_PERI_REG_MASK(SPI_PIN(HSPI), SPI_CS1_DIS);\
						SET_PERI_REG_MASK(SPI_PIN(HSPI), SPI_CS0_DIS |SPI_CS2_DIS)
#define ACTIVE_HSPI_CS2		CLEAR_PERI_REG_MASK(SPI_PIN(HSPI), SPI_CS2_DIS);\
						SET_PERI_REG_MASK(SPI_PIN(HSPI), SPI_CS0_DIS |SPI_CS1_DIS)
#define ENABLE_HSPI_DEV_CS()		PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, 2)
#define DISABLE_HSPI_DEV_CS()		GPIO_OUTPUT_SET(15, 1);\
									PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, FUNC_GPIO15)
struct hspi_device_register hspi_dev_reg;
/******************************************************************************
 * FunctionName : hspi_overlap_init
 * Description  : enable hspi and spi module overlap mode
*******************************************************************************/
void ICACHE_FLASH_ATTR
hspi_overlap_init(void)
{
	//hspi overlap to spi, two spi masters on cspi
	SET_PERI_REG_MASK(HOST_INF_SEL, reg_cspi_overlap);

	//set higher priority for spi than hspi
	SET_PERI_REG_MASK(SPI_EXT3(SPI),0x1);
	SET_PERI_REG_MASK(SPI_EXT3(HSPI),0x3);
	SET_PERI_REG_MASK(SPI_USER(HSPI), BIT(5));
}
/******************************************************************************
 * FunctionName : hspi_overlap_deinit
 * Description  : recover hspi and spi module from overlap mode
*******************************************************************************/
void ICACHE_FLASH_ATTR
hspi_overlap_deinit(void)
{
	//hspi overlap to spi, two spi masters on cspi
	CLEAR_PERI_REG_MASK(HOST_INF_SEL, reg_cspi_overlap);

	//set higher priority for spi than hspi
	CLEAR_PERI_REG_MASK(SPI_EXT3(SPI),0x1);
	CLEAR_PERI_REG_MASK(SPI_EXT3(HSPI),0x3);
	CLEAR_PERI_REG_MASK(SPI_USER(HSPI), BIT(5));
}

/******************************************************************************
 * FunctionName : spi_reg_backup
 * Description  : backup SPI normal operation register value and disable CPU cache to modify some flash registers.
 * Parameters   : 	uint8 spi_no - SPI module number, Only "SPI" and "HSPI" are valid
*******************************************************************************/
void ICACHE_FLASH_ATTR
    spi_reg_backup(uint8 spi_no,uint32* backup_mem)
{
	if(spi_no>1) 		return; //handle invalid input number

	backup_mem[PERIPHS_IO_MUX_BACKUP]	=READ_PERI_REG(PERIPHS_IO_MUX);
	backup_mem[SPI_USER_BACKUP]	=READ_PERI_REG(SPI_USER(spi_no)); 	
	backup_mem[SPI_CTRL_BACKUP]	=READ_PERI_REG(SPI_CTRL(spi_no)); 	
	backup_mem[SPI_CLOCK_BACKUP]	=READ_PERI_REG(SPI_CLOCK(spi_no));
	backup_mem[SPI_USER1_BACKUP]	=READ_PERI_REG(SPI_USER1(spi_no));
	backup_mem[SPI_USER2_BACKUP]	=READ_PERI_REG(SPI_USER2(spi_no));	
	backup_mem[SPI_CMD_BACKUP]	=READ_PERI_REG(SPI_CMD(spi_no));
	backup_mem[SPI_PIN_BACKUP]	=READ_PERI_REG(SPI_PIN(spi_no));
	backup_mem[SPI_SLAVE_BACKUP]	=READ_PERI_REG(SPI_SLAVE(spi_no));
}
/******************************************************************************
 * FunctionName : spi_reg_recover
 * Description  : recover SPI normal operation register value and enable CPU cache.
 * Parameters   : 	uint8 spi_no - SPI module number, Only "SPI" and "HSPI" are valid
*******************************************************************************/
void ICACHE_FLASH_ATTR
   spi_reg_recover(uint8 spi_no,uint32* backup_mem)
{
	if(spi_no>1) 		return; //handle invalid input number

//	WRITE_PERI_REG(PERIPHS_IO_MUX, backup_mem[PERIPHS_IO_MUX_BACKUP]); 	
	WRITE_PERI_REG(SPI_USER(spi_no), backup_mem[SPI_USER_BACKUP]); 	
	WRITE_PERI_REG(SPI_CTRL(spi_no), backup_mem[SPI_CTRL_BACKUP]); 	
	WRITE_PERI_REG(SPI_CLOCK(spi_no), backup_mem[SPI_CLOCK_BACKUP]); 	
	WRITE_PERI_REG(SPI_USER1(spi_no), backup_mem[SPI_USER1_BACKUP]); 	
	WRITE_PERI_REG(SPI_USER2(spi_no), backup_mem[SPI_USER2_BACKUP]); 	
	WRITE_PERI_REG(SPI_CMD(spi_no), backup_mem[SPI_CMD_BACKUP]); 	
	WRITE_PERI_REG(SPI_PIN(spi_no), backup_mem[SPI_PIN_BACKUP]); 
//	WRITE_PERI_REG(SPI_SLAVE(spi_no), backup_mem[SPI_SLAVE_BACKUP]); 
}

void ICACHE_FLASH_ATTR  
    hspi_master_dev_init(uint8 dev_no,uint8 clk_polar,uint8 clk_div)
{
	uint32 regtemp;
	if((dev_no>3)||(clk_polar>1)||(clk_div>0x1f))
	{
		os_printf("hspi_master_dev_init parameter is out of range!\n\r");
		return;
	}
	
	WAIT_HSPI_IDLE();
	if(!hspi_dev_reg.hspi_reg_backup_flag){
		if(READ_PERI_REG(PERIPHS_IO_MUX)&BIT8){
			hspi_dev_reg.spi_io_80m=1;
			SET_PERI_REG_MASK(SPI_CLOCK(HSPI),SPI_CLK_EQU_SYSCLK);
		}else{
			hspi_dev_reg.spi_io_80m=0;
			CLEAR_PERI_REG_MASK(SPI_CLOCK(HSPI),SPI_CLK_EQU_SYSCLK);
		}
		
		regtemp=READ_PERI_REG(SPI_CTRL(SPI))&SPI_FLASH_READ_MODE_MASK;
		CLEAR_PERI_REG_MASK(SPI_CTRL(HSPI), SPI_FLASH_READ_MODE_MASK);
		SET_PERI_REG_MASK(SPI_CTRL(HSPI), regtemp);
		spi_reg_backup(HSPI, hspi_dev_reg.hspi_flash_reg_backup);

		spi_master_init(HSPI);
		spi_reg_backup(HSPI, hspi_dev_reg.hspi_dev_reg_backup);

		hspi_dev_reg.hspi_reg_backup_flag=1;
		
	//	spi_reg_recover(HSPI, hspi_dev_reg.hspi_flash_reg_backup);
		hspi_dev_reg.selected_dev_num=HSPI_IDLE;
	}

	hspi_dev_reg.hspi_dev_conf[dev_no].active=1;
	hspi_dev_reg.hspi_dev_conf[dev_no].clk_div=clk_div;
	hspi_dev_reg.hspi_dev_conf[dev_no].clk_polar=clk_polar;
	
	switch(dev_no){
		case HSPI_CS_DEV :
			PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, 2);
			PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, 2);
			PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, 2);	
			PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, 2);
			CLEAR_PERI_REG_MASK(PERIPHS_IO_MUX, BIT9);
			break;

		case SPI_CS1_DEV :	
			PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0TXD_U, FUNC_SPI_CS1);
			if(hspi_dev_reg.spi_io_80m){	
				os_printf("SPI CS1 device must work at 80Mhz");
			}
			break;
			
		case SPI_CS2_DEV :	
			PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_SPI_CS2);
			if(hspi_dev_reg.spi_io_80m){	
				os_printf("SPI CS2 device must work at 80Mhz");
			}
			break;
			
		default: break;
	}
}

void ICACHE_FLASH_ATTR
    hspi_dev_sel(uint8 dev_no)
{
	uint32 regval;
	
	if(dev_no>3){
		os_printf("hspi_dev_sel parameter is out of range!\n\r");
		return;
	}

	if(!hspi_dev_reg.hspi_dev_conf[dev_no].active){
		os_printf("device%d has not been initialized!\n\r",dev_no);
		return;
	}
	
	switch(hspi_dev_reg.selected_dev_num){
		case HSPI_CS_DEV:
			if((dev_no==SPI_CS1_DEV)||(dev_no==SPI_CS2_DEV)){
				WAIT_HSPI_IDLE();
				DISABLE_HSPI_DEV_CS();
				hspi_overlap_init();
				
				if(hspi_dev_reg.spi_io_80m)	{SET_PERI_REG_MASK(SPI_CLOCK(HSPI), SPI_CLK_EQU_SYSCLK);}
				else							{CONF_HSPI_CLK_DIV(hspi_dev_reg.hspi_dev_conf[dev_no].clk_div);}
				
				if(hspi_dev_reg.hspi_dev_conf[dev_no].clk_polar)	{HSPI_FALLING_EDGE_SAMPLE();}
				else												{HSPI_RISING_EDGE_SAMPLE();}
				
				if(dev_no==SPI_CS1_DEV)		{ACTIVE_HSPI_CS1;}
				else							{ACTIVE_HSPI_CS2;}
			}
			else if(dev_no==SPI_CS0_FLASH){
				WAIT_HSPI_IDLE();
				DISABLE_HSPI_DEV_CS();
				hspi_overlap_init();
				spi_reg_recover(HSPI, hspi_dev_reg.hspi_flash_reg_backup);

				if(hspi_dev_reg.spi_io_80m)	{SET_PERI_REG_MASK(SPI_CLOCK(HSPI), SPI_CLK_EQU_SYSCLK);}

				HSPI_RISING_EDGE_SAMPLE();
				ACTIVE_HSPI_CS0	;
			}
			break;

		case SPI_CS1_DEV:
			if(dev_no==SPI_CS2_DEV){
				WAIT_HSPI_IDLE();
				if(!hspi_dev_reg.spi_io_80m)	{CONF_HSPI_CLK_DIV(hspi_dev_reg.hspi_dev_conf[dev_no].clk_div);}

				if(hspi_dev_reg.hspi_dev_conf[dev_no].clk_polar)	{HSPI_FALLING_EDGE_SAMPLE();}
				else												{HSPI_RISING_EDGE_SAMPLE();}
				ACTIVE_HSPI_CS2;			
			}
			else if(dev_no==SPI_CS0_FLASH){
				WAIT_HSPI_IDLE();
				spi_reg_recover(HSPI, hspi_dev_reg.hspi_flash_reg_backup);
				HSPI_RISING_EDGE_SAMPLE();
				ACTIVE_HSPI_CS0;
			}
			else if(dev_no==HSPI_CS_DEV){
				WAIT_HSPI_IDLE();
				ENABLE_HSPI_DEV_CS();
				hspi_overlap_deinit();
				CONF_HSPI_CLK_DIV(hspi_dev_reg.hspi_dev_conf[dev_no].clk_div);

				if(hspi_dev_reg.hspi_dev_conf[dev_no].clk_polar)	{HSPI_FALLING_EDGE_SAMPLE();}
				else												{HSPI_RISING_EDGE_SAMPLE();}

				ACTIVE_HSPI_CS0;
			}
			break;

		case SPI_CS2_DEV:
			if(dev_no==SPI_CS1_DEV){
				WAIT_HSPI_IDLE();
				if(!hspi_dev_reg.spi_io_80m)	{CONF_HSPI_CLK_DIV(hspi_dev_reg.hspi_dev_conf[dev_no].clk_div);}

				if(hspi_dev_reg.hspi_dev_conf[dev_no].clk_polar)	{HSPI_FALLING_EDGE_SAMPLE();}
				else												{HSPI_RISING_EDGE_SAMPLE();}

				ACTIVE_HSPI_CS1;
			}
			else if(dev_no==SPI_CS0_FLASH){
				WAIT_HSPI_IDLE();
				spi_reg_recover(HSPI, hspi_dev_reg.hspi_flash_reg_backup);
				HSPI_RISING_EDGE_SAMPLE();
				ACTIVE_HSPI_CS0;
			}
			else if(dev_no==HSPI_CS_DEV){
				WAIT_HSPI_IDLE();
				ENABLE_HSPI_DEV_CS();
				hspi_overlap_deinit();
				CONF_HSPI_CLK_DIV(hspi_dev_reg.hspi_dev_conf[dev_no].clk_div);

				if(hspi_dev_reg.hspi_dev_conf[dev_no].clk_polar)	{HSPI_FALLING_EDGE_SAMPLE();}
				else												{HSPI_RISING_EDGE_SAMPLE();}

				ACTIVE_HSPI_CS0;
			}
			break;
			
		case SPI_CS0_FLASH:
			if((dev_no==SPI_CS1_DEV)||(dev_no==SPI_CS2_DEV)){
				WAIT_HSPI_IDLE();
				spi_reg_recover(HSPI, hspi_dev_reg.hspi_dev_reg_backup);

				if(hspi_dev_reg.spi_io_80m)	{SET_PERI_REG_MASK(SPI_CLOCK(HSPI), SPI_CLK_EQU_SYSCLK);}
				else							{CONF_HSPI_CLK_DIV(hspi_dev_reg.hspi_dev_conf[dev_no].clk_div);}

				if(hspi_dev_reg.hspi_dev_conf[dev_no].clk_polar)	{HSPI_FALLING_EDGE_SAMPLE();}
				else												{HSPI_RISING_EDGE_SAMPLE();}

				if(dev_no==SPI_CS1_DEV)		{ACTIVE_HSPI_CS1;}
				else							{ACTIVE_HSPI_CS2;}			
			}
			else if(dev_no==HSPI_CS_DEV){
				WAIT_HSPI_IDLE();
				ENABLE_HSPI_DEV_CS();
				hspi_overlap_deinit();
				spi_reg_recover(HSPI, hspi_dev_reg.hspi_dev_reg_backup);
				CONF_HSPI_CLK_DIV(hspi_dev_reg.hspi_dev_conf[dev_no].clk_div);

				if(hspi_dev_reg.hspi_dev_conf[dev_no].clk_polar)	{HSPI_FALLING_EDGE_SAMPLE();}
				else												{HSPI_RISING_EDGE_SAMPLE();}

				ACTIVE_HSPI_CS0;
			}
			break;

		default: 
			if((dev_no==SPI_CS1_DEV)||(dev_no==SPI_CS2_DEV)){
				WAIT_HSPI_IDLE();
				DISABLE_HSPI_DEV_CS();
				hspi_overlap_init();
				spi_reg_recover(HSPI, hspi_dev_reg.hspi_dev_reg_backup);
				
				if(hspi_dev_reg.spi_io_80m)	{SET_PERI_REG_MASK(SPI_CLOCK(HSPI), SPI_CLK_EQU_SYSCLK);}
				else							{CONF_HSPI_CLK_DIV(hspi_dev_reg.hspi_dev_conf[dev_no].clk_div);}

				if(hspi_dev_reg.hspi_dev_conf[dev_no].clk_polar)	{HSPI_FALLING_EDGE_SAMPLE();}
				else												{HSPI_RISING_EDGE_SAMPLE();}

				if(dev_no==SPI_CS1_DEV)		{ACTIVE_HSPI_CS1;}
				else							{ACTIVE_HSPI_CS2;}
			}
			else if(dev_no==SPI_CS0_FLASH){
				WAIT_HSPI_IDLE();
				DISABLE_HSPI_DEV_CS();
				hspi_overlap_init();
				spi_reg_recover(HSPI, hspi_dev_reg.hspi_flash_reg_backup);

				if(hspi_dev_reg.spi_io_80m)	{SET_PERI_REG_MASK(SPI_CLOCK(HSPI), SPI_CLK_EQU_SYSCLK);}
				
				HSPI_RISING_EDGE_SAMPLE();
				ACTIVE_HSPI_CS0	;
			}			
			else if(dev_no==HSPI_CS_DEV){
				WAIT_HSPI_IDLE();
				ENABLE_HSPI_DEV_CS();
				hspi_overlap_deinit();
				spi_reg_recover(HSPI, hspi_dev_reg.hspi_dev_reg_backup);
				CONF_HSPI_CLK_DIV(hspi_dev_reg.hspi_dev_conf[dev_no].clk_div);

				if(hspi_dev_reg.hspi_dev_conf[dev_no].clk_polar)	{HSPI_FALLING_EDGE_SAMPLE();}
				else												{HSPI_RISING_EDGE_SAMPLE();}

				ACTIVE_HSPI_CS0;
			}
			break;
	}
	hspi_dev_reg.selected_dev_num=dev_no;
}

/******************************************************************************
 * FunctionName : spi_read_data
 * Description  : use hspi to read flash data for stability test
 * Parameters   : 	SpiFlashChip * spi-- flash parameter structure pointer
 * 				uint32 flash_addr--flash start address
 *				uint32 * addr_dest--start address for preped destination memory space
 *				uint32 byte_length--length of the data which needs to be read from flash
*******************************************************************************/
SpiFlashOpResult ICACHE_FLASH_ATTR
hspi_overlap_read_flash_data(SpiFlashChip * spi, uint32 flash_addr, uint32 * addr_dest, uint32 byte_length)
{
    uint32  temp_addr,reg_tmp;
    sint32  temp_length;
    uint8   i;
    uint8   remain_word_num;	
	
   hspi_dev_sel(SPI_CS0_FLASH);

      //address range check 	
   if ((flash_addr+byte_length) > (spi->chip_size))
   {
        return SPI_FLASH_RESULT_ERR;
   }
   
    temp_addr = flash_addr;
    temp_length = byte_length;
	
    while(temp_length > 0)
    {
       if(temp_length >= SPI_BUFF_BYTE_NUM)
       {
	//   reg_tmp=((temp_addr&0xff)<<16)|(temp_addr&0xff00)|((temp_addr&0xff0000)>>16)|(SPI_BUFF_BYTE_NUM << SPI_FLASH_BYTES_LEN);
	   reg_tmp= temp_addr |(SPI_BUFF_BYTE_NUM<< SPI_FLASH_BYTES_LEN) ;
	   WRITE_PERI_REG(SPI_ADDR(HSPI), reg_tmp);
          WRITE_PERI_REG(SPI_CMD(HSPI), SPI_FLASH_READ);
          while(READ_PERI_REG(SPI_CMD(HSPI)) != 0);

          for(i=0; i<(SPI_BUFF_BYTE_NUM>>2);i++)
          {    
              *addr_dest++ = READ_PERI_REG(SPI_W0(HSPI)+i*4);
          }
          temp_length = temp_length - SPI_BUFF_BYTE_NUM;
          temp_addr = temp_addr + SPI_BUFF_BYTE_NUM;
       }
       else
       {
            WRITE_PERI_REG(SPI_ADDR(HSPI), temp_addr |(temp_length << SPI_FLASH_BYTES_LEN ));
            WRITE_PERI_REG(SPI_CMD(HSPI), SPI_FLASH_READ);
            while(READ_PERI_REG(SPI_CMD(HSPI)) != 0);

            remain_word_num = (0== (temp_length&0x3))? (temp_length>>2) : (temp_length>>2)+1;
	     for (i=0; i<remain_word_num; i++)		
	     {
               *addr_dest++ = READ_PERI_REG(SPI_W0(HSPI)+i*4);
	     }	 
            temp_length = 0;
        }
    }

    return SPI_FLASH_RESULT_OK;
}

void ICACHE_FLASH_ATTR
hspi_overlap_flash_init(void)
{
	 hspi_master_dev_init(SPI_CS0_FLASH,0,0);

	 spi_flash_set_read_func(hspi_overlap_read_flash_data);
}