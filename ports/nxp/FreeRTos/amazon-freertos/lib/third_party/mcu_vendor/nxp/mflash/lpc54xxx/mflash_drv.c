/*
 * Copyright 2017 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_spifi.h"
#include "mflash_drv.h"
#include "pin_mux.h"
#include <stdbool.h>

/* Command ID */
#define COMMAND_NUM                 (6)
#define READ                        (0)
#define PROGRAM_PAGE                (1)
#define GET_STATUS                  (2)
#define ERASE_SECTOR                (3)
#define WRITE_ENABLE                (4)
#define WRITE_REGISTER              (5)


//#ifdef XIP_IMAGE
//#warning NOTE: MFLASH driver expects that application runs from XIP
//#else
//#warning NOTE: MFLASH driver expects that application runs from SRAM
//#endif


/* Temporary sector storage. Use uint32_t type to force 4B alignment and 
 * improve copy operation */
static uint32_t g_flashm_sector[MFLASH_SECTOR_SIZE / sizeof(uint32_t)];


/* Commands definition, taken from SPIFI demo */
static spifi_command_t command[COMMAND_NUM] = {
    /* read */
    {MFLASH_PAGE_SIZE, false, kSPIFI_DataInput, 1, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeAddrThreeBytes, 0x0B},
    /* program */
    {MFLASH_PAGE_SIZE, false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeAddrThreeBytes, 0x2},
    /* status */
    {1, false, kSPIFI_DataInput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeOnly, 0x05},
    /* erase */
    {0, false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeAddrThreeBytes, 0x20},
    /* write enable */
    {0, false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeOnly, 0x06},
    /* write register */
    {4, false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeOnly, 0x01}
};


/* Wait until command finishes */
static inline void mflash_drv_check_if_finish(void)
{
    uint8_t val = 0;
    do
    {
        SPIFI_SetCommand(MFLASH_SPIFI, &command[GET_STATUS]);
        while ((MFLASH_SPIFI->STAT & SPIFI_STAT_INTRQ_MASK) == 0U)
        {
        }
        val = SPIFI_ReadDataByte(MFLASH_SPIFI);
    } while (val & 0x1);
}

/* return offset from sector */
static void mflash_drv_read_mode(void)
{
    /* Switch back to read mode */
    SPIFI_ResetCommand(MFLASH_SPIFI);
    SPIFI_SetMemoryCommand(MFLASH_SPIFI, &command[READ]);
}


/* Initialize SPIFI & flash peripheral, 
 * cannot be invoked directly, requires calling wrapper in non XIP memory */
static int32_t mflash_drv_init_internal(void)
{
    /* NOTE: Multithread access is not supported for SRAM target.
     *       XIP target MUST be protected by disabling global interrupts 
     *       since all ISR (and API that is used inside) is placed at XIP. 
     *       It is necessary to place at least "mflash_drv_drv.o", "fsl_spifi.o" to SRAM */
    /* disable interrupts when running from XIP 
     * TODO: store/restore previous PRIMASK on stack to avoid
     * failure in case of nested critical sections !! */
    __asm("cpsid i");
    spifi_config_t config = {0};

#ifndef XIP_IMAGE
    uint32_t sourceClockFreq;
    BOARD_InitSPIFI();
    /* Reset peripheral */
    RESET_PeripheralReset(kSPIFI_RST_SHIFT_RSTn);
    /* Set SPIFI clock source */
    CLOCK_AttachClk(kFRO_HF_to_SPIFI_CLK);
    sourceClockFreq = CLOCK_GetSpifiClkFreq();
    /* Set the clock divider */
    CLOCK_SetClkDiv(kCLOCK_DivSpifiClk, sourceClockFreq / MFLASH_BAUDRATE, false);
    /* Enable SPIFI clock */
    CLOCK_EnableClock(kCLOCK_Spifi);
#endif

    SPIFI_GetDefaultConfig(&config);
    config.dualMode = kSPIFI_DualMode;
#ifdef XIP_IMAGE
    config.disablePrefetch = false; // true;
    config.disableCachePrefech = false; // true;
#else
    config.disablePrefetch = false; // true;
    config.disableCachePrefech = false; // true;
#endif

    /* Reset the Command register */
    SPIFI_ResetCommand(MFLASH_SPIFI);

    /* Set time delay parameter */
    MFLASH_SPIFI->CTRL = SPIFI_CTRL_TIMEOUT(config.timeout) | SPIFI_CTRL_CSHIGH(config.csHighTime) |
                 SPIFI_CTRL_D_PRFTCH_DIS(config.disablePrefetch) | SPIFI_CTRL_MODE3(config.spiMode) |
                 SPIFI_CTRL_PRFTCH_DIS(config.disableCachePrefech) | SPIFI_CTRL_DUAL(config.dualMode) |
                 SPIFI_CTRL_RFCLK(config.isReadFullClockCycle) | SPIFI_CTRL_FBCLK(config.isFeedbackClock);

    mflash_drv_read_mode();

    __asm("cpsie i");
    return 0;
}


/* API - initialize 'mflash' */
int32_t mflash_drv_init(void)
{
    volatile int32_t result;
    /* Necessary to have double wrapper call in non_xip memory */
    result = mflash_drv_init_internal();
    return result;
}


/* Internal - erase single sector */
static int32_t mflash_drv_sector_erase(uint32_t sector_addr)
{
    if (false == mflash_drv_is_sector_aligned((uint32_t)sector_addr))
        return -1;

    __asm("cpsid i");

    /* Reset the SPIFI to switch to command mode */
    SPIFI_ResetCommand(MFLASH_SPIFI);

    /* Write enable */
    SPIFI_SetCommand(MFLASH_SPIFI, &command[WRITE_ENABLE]);
    /* Set address */
    SPIFI_SetCommandAddress(MFLASH_SPIFI, sector_addr);
    /* Erase sector */
    SPIFI_SetCommand(MFLASH_SPIFI, &command[ERASE_SECTOR]);
    /* Check if finished */
    mflash_drv_check_if_finish();
    /* Switch to read mode to enable interrupts as soon ass possible */
    mflash_drv_read_mode();

    __asm("cpsie i");
    /* Flush pipeline to allow pending interrupts take place 
     * before starting next loop */
    __ISB();

    return 0;
}


/* Internal - write single sector */
static int32_t mflash_drv_sector_program(uint32_t sector_addr, uint32_t *sector_data)
{
    if (false == mflash_drv_is_sector_aligned((uint32_t)sector_addr))
        return -1;

    uint32_t max_page = MFLASH_SECTOR_SIZE / MFLASH_PAGE_SIZE;
    for (uint32_t page_idx = 0, page_i = 0; page_idx < max_page; page_idx++)
    {
        __asm("cpsid i");
        /* Program page */
        SPIFI_ResetCommand(MFLASH_SPIFI);
        SPIFI_SetCommand(MFLASH_SPIFI, &command[WRITE_ENABLE]);
        SPIFI_SetCommandAddress(MFLASH_SPIFI, sector_addr + page_idx * MFLASH_PAGE_SIZE);
        SPIFI_SetCommand(MFLASH_SPIFI, &command[PROGRAM_PAGE]);
        page_i = page_idx * (MFLASH_PAGE_SIZE/sizeof(sector_data[0]));
        /* Store 4B in each loop. Sector has always 4B alignment and size multiple of 4 */
        for (uint32_t i = 0; i < MFLASH_PAGE_SIZE/sizeof(sector_data[0]); i++)
        {
            SPIFI_WriteData(MFLASH_SPIFI, sector_data[page_i + i]);
        }
        mflash_drv_check_if_finish();
        /* Switch to read mode to enable interrupts as soon ass possible */
        mflash_drv_read_mode();
        __asm("cpsie i");
        /* Flush pipeline to allow pending interrupts take place 
         * before starting next loop */
        __ISB();
    }



    return 0;
}


/* Internal - write data of 'data_len' to single sector 'sector_addr', starting from 'sect_off' */
int32_t mflash_drv_sector_write(uint32_t sector_addr, uint32_t sect_off, uint8_t *data, uint32_t data_len)
{
    /* Address not aligned to sector boundary */
    if (false == mflash_drv_is_sector_aligned((uint32_t)sector_addr))
        return -1;
    /* Offset + length exceeed sector size */
    if (sect_off + data_len > MFLASH_SECTOR_SIZE)
        return -1;

    /* Switch back to read mode */
    SPIFI_ResetCommand(MFLASH_SPIFI);
    SPIFI_SetMemoryCommand(MFLASH_SPIFI, &command[READ]);

    /* Copy old sector data by 4B in each loop to buffer */
    for (uint32_t i = 0; i < sizeof(g_flashm_sector)/sizeof(g_flashm_sector[0]); i++)
    {
        g_flashm_sector[i] = *((uint32_t*)(sector_addr) + i);
    }

    /* Copy custom data ( 1B in each loop ) to buffer at specific position */
    for (uint32_t i = 0; i < data_len; i++)
    {
        ((uint8_t*)g_flashm_sector)[sect_off + i] = data[i];
    }

    /* Erase flash */
    if (0 != mflash_drv_sector_erase(sector_addr))
        return -2;

    /* Program data */
    if (0 != mflash_drv_sector_program(sector_addr, g_flashm_sector))
        return -2;

    /* Switch back to read mode */
    SPIFI_ResetCommand(MFLASH_SPIFI);
    SPIFI_SetMemoryCommand(MFLASH_SPIFI, &command[READ]);
    return 0;
}


/* Write data to flash, cannot be invoked directly, requires calling wrapper in non XIP memory */
int32_t mflash_drv_write_internal(void *any_addr, uint8_t *data, uint32_t data_len)
{

    uint32_t sect_size = MFLASH_SECTOR_SIZE;
    /* Interval <0, sector_size) */
    uint32_t to_write = 0;
    /* Interval (data_len, 0>  */
    uint32_t to_remain = data_len;
    int32_t result = 0;

    for (
         /* Calculate address of first sector */
         uint32_t sect_a = mflash_drv_addr_to_sector_addr((uint32_t)any_addr),
         /* and first sector offset */
         sect_of = mflash_drv_addr_to_sector_of((uint32_t)any_addr),
         /* and set first data offset to 0*/
         data_of = 0;
         /* Continue until sector address exceed target adddress + data_length */
         sect_a < ((uint32_t)any_addr) + data_len;
         /* Move to next sector */
         sect_a += sect_size,
         /* and move pointer to data */
         data_of += to_write
    )
    {
        /* If remaining data is exceed 'sector_size', write 'sector_size' length */
        if (to_remain > sect_size - sect_of)
        {
            to_write = sect_size - sect_of;
            to_remain = to_remain - to_write;
        }
        /* else write remaining data length */
        else
        {
            to_write = to_remain;
            to_remain = 0;
        }

        /* Write at 'sect_a' sector, starting at 'sect_of' using '&data[data_of]' of length 'to_write' */
        result = mflash_drv_sector_write(sect_a, sect_of, &data[data_of], to_write);
        if (0 != result)
            return -1;
        /* Only first sector is allowed to have an offset */
        sect_of = 0;
    }

    return 0;
}


/* Calling wrapper for 'mflash_drv_write_internal'.
 * Write 'data' of 'data_len' to 'any_addr' - which doesn't have to be sector aligned.
 * NOTE: Don't try to store constant data that are located in XIP !!
 */
int32_t mflash_drv_write(void *any_addr, uint8_t *data, uint32_t data_len)
{
    volatile int32_t result;
    result = mflash_drv_write_internal(any_addr, data, data_len);
    return result;
}



#if 0
/* Dummy test to prove functionality */
volatile uint32_t lock2 = 1;
char * tmp_string = "Curabitur sit amet justo ac velit consectetur lobortis. Donec porttitor, eros sed sollicitudin viverra, massa enim placerat sapien, et vestibulum lacus quam et leo. Proin gravida";

void mflash_drv_do_test(void)
{
    lock2 = 1;
    while (lock2);

    // place to SRAM !!!
#ifndef XIP_IMAGE
    BOARD_InitSPIFI();
#endif

    /* Reset peripheral */
    lock2 = 1;
    while (lock2);
    mflash_drv_init();

    lock2 = 1;
    while (lock2)
    mflash_drv_write((void*)(0x10800010), tmp_string, 30);//TODO force read to recover

    lock2 = 1;
    while (lock2);
}
#endif



