#include <stdio.h>
#include <string.h>
#include "cc3000_common.h"
#include "nvmem.h"
#include "ccspi.h"
#include "hci.h"
#include "wlan.h"
#include "patch_prog.h"
#define BIT0    0x1
#define BIT1    0x2
#define BIT2    0x4
#define BIT3    0x8
#define BIT4    0x10
#define BIT5    0x20
#define BIT6    0x40
#define BIT7    0x80

static unsigned char ucStatus_Dr;
static unsigned char ucStatus_FW;
static unsigned char return_status = 0xFF;

static signed char mac_status = -1;
static unsigned char counter = 0;

// Array to store RM parameters from EEPROM.
static unsigned char cRMParamsFromEeprom[128];
// Array to store MAC address from EEPROM.
static unsigned char cMacFromEeprom[MAC_ADDR_LEN];
// Smart Config Prefix
static const char aucCC3000_prefix[] = {'T', 'T', 'T'};

static void systick_sleep(unsigned long ms) {
    extern void HAL_Delay(volatile uint32_t Delay);
    HAL_Delay(ms);
}

// 2 dim array to store address and length of new FAT
static const unsigned short aFATEntries[2][NVMEM_RM_FILEID + 1] = 
/*  address 	*/  {{0x50, 	0x1f0, 	0x390, 	0x1390, 	0x2390, 	0x4390, 	0x6390, 	0x63a0, 	0x63b0, 	0x63f0, 	0x6430, 	0x6830},
/*  length	*/	{0x1a0, 	0x1a0, 	0x1000, 	0x1000, 	0x2000, 	0x2000, 	0x10, 	0x10, 	0x40, 	0x40, 	0x400, 	0x200}};
/* 0. NVS */
/* 1. NVS Shadow */
/* 2. Wireless Conf */
/* 3. Wireless Conf Shadow */
/* 4. BT (WLAN driver) Patches */
/* 5. WiLink (Firmware) Patches */
/* 6. MAC addr */
/* 7. Frontend Vars */
/* 8. IP config */
/* 9. IP config Shadow */
/* 10. Bootloader Patches */
/* 11. Radio Module params */
/* 12. AES128 for smart config */
/* 13. user file */
/* 14. user file */
/* 15. user file */

//*****************************************************************************
//
//! sendDriverPatch
//!
//! \param  pointer to the length
//!
//! \return none
//!
//! \brief  The function returns a pointer to the driver patch: 
//!         since there is no patch yet - it returns 0
//
//*****************************************************************************

static char *sendDriverPatch(unsigned long *Length)
{
    *Length = 0;
    return NULL;
}


//*****************************************************************************
//
//! sendBootLoaderPatch
//!
//! \param  pointer to the length
//!
//! \return none
//!
//! \brief  The function returns a pointer to the boot loader patch: 
//!         since there is no patch yet - it returns 0
//
//*****************************************************************************

static char *sendBootLoaderPatch(unsigned long *Length)
{
    *Length = 0;
    return NULL;
}

//*****************************************************************************
//
//! sendWLFWPatch
//!
//! \param  pointer to the length
//!
//! \return none
//!
//! \brief  The function returns a pointer to the FW patch: 
//!         since there is no patch yet - it returns 0
//
//*****************************************************************************

static char *sendWLFWPatch(unsigned long *Length)
{
    *Length = 0;
    return NULL;
}

//*****************************************************************************
//
//! CC3000_UsynchCallback
//!
//! \param  Event type
//!
//! \return none
//!
//! \brief  The function handles asynchronous events that come from CC3000 
//!         device and operates a LED4 to have an on-board indication
//
//*****************************************************************************

static void CC3000_UsynchCallback(long lEventType, char * data, unsigned char length)
{
	
}

//*****************************************************************************
//
//! initDriver
//!
//!  \param[in] cRequestPatch 0 to load with EEPROM patches 
//!             and 1 to load with no patches
//!
//!  \return none
//!
//!  \brief  The function initializes a CC3000 device 
//!          and triggers it to start operation
//
//*****************************************************************************
static int initDriver(unsigned short cRequestPatch)
{
    // WLAN On API Implementation
    wlan_init(CC3000_UsynchCallback, sendWLFWPatch, sendDriverPatch, sendBootLoaderPatch,
              ReadWlanInterruptPin, SpiResumeSpi, SpiPauseSpi, WriteWlanPin);

    // Trigger a WLAN device
    wlan_start(cRequestPatch);
    wlan_smart_config_set_prefix((char*)aucCC3000_prefix);
    wlan_ioctl_set_connection_policy(0, 0, 0);
    wlan_ioctl_del_profile(255);

    // Mask out all non-required events from CC3000
    wlan_set_event_mask(HCI_EVNT_WLAN_KEEPALIVE|
                        HCI_EVNT_WLAN_UNSOL_INIT|
                        HCI_EVNT_WLAN_ASYNC_PING_REPORT);

    //unsolicicted_events_timer_init();
    systick_sleep(100);
    return(0);
}


//*****************************************************************************
//
//! fat_read_content
//!
//! \param[out] is_allocated  array of is_allocated in FAT table:\n
//!             an allocated entry implies the address and length of the 
//!             file are valid.
//!             0: not allocated; 1: allocated.
//! \param[out] is_valid  array of is_valid in FAT table:\n
//!             a valid entry implies the content of the file is relevant.
//!             0: not valid; 1: valid.
//! \param[out] write_protected  array of write_protected in FAT table:\n
//!             a write protected entry implies it is not possible to write
//!             into this entry.
//!             0: not protected; 1: protected.
//! \param[out] file_address  array of file address in FAT table:\n
//!             this is the absolute address of the file in the EEPROM.
//! \param[out] file_length  array of file length in FAT table:\n
//!             this is the upper limit of the file size in the EEPROM.
//!
//! \return on succes 0, error otherwise
//!
//! \brief  parse the FAT table from eeprom 
//
//*****************************************************************************
static unsigned char  __attribute__ ((unused))
fat_read_content(unsigned char *is_allocated, unsigned char *is_valid,
        unsigned char *write_protected, unsigned short *file_address, unsigned short *file_length)
{
    unsigned short  index;
    unsigned char   ucStatus;
    unsigned char   fatTable[48];
    unsigned char*  fatTablePtr = fatTable;

    //
    // Read in 6 parts to work with tiny driver
    //
    for (index = 0; index < 6; index++)
    {
        ucStatus = nvmem_read(16, 8, 4 + 8*index, fatTablePtr); 
        fatTablePtr += 8;
    }
	
    fatTablePtr = fatTable;
	
    for (index = 0; index <= NVMEM_RM_FILEID; index++)
    {
        *is_allocated++ = (*fatTablePtr) & BIT0;
        *is_valid++ = ((*fatTablePtr) & BIT1) >> 1;
        *write_protected++ = ((*fatTablePtr) & BIT2) >> 2;
        *file_address++ = ((*(fatTablePtr+1)<<8) | (*fatTablePtr)) & (BIT4|BIT5|BIT6|BIT7);
        *file_length++ = ((*(fatTablePtr+3)<<8) | (*(fatTablePtr+2))) & (BIT4|BIT5|BIT6|BIT7);
        
        //
        // Move to next file ID
        //
        fatTablePtr += 4;  
    }
    
    return ucStatus;
}

//*****************************************************************************
//
//! fat_write_content
//!
//! \param[in] file_address  array of file address in FAT table:\n
//!            this is the absolute address of the file in the EEPROM.
//! \param[in] file_length  array of file length in FAT table:\n
//!            this is the upper limit of the file size in the EEPROM.
//!
//! \return on succes 0, error otherwise
//!
//! \brief  parse the FAT table from eeprom 
//
//*****************************************************************************
static unsigned char fat_write_content(unsigned short const *file_address,
                                unsigned short const *file_length)
{
    unsigned short  index = 0;
    unsigned char   ucStatus;
    unsigned char   fatTable[48];
    unsigned char*  fatTablePtr = fatTable;

    //
    // First, write the magic number.
    //
    ucStatus = nvmem_write(16, 2, 0, (unsigned char*)"LS"); 
	
    for (; index <= NVMEM_RM_FILEID; index++)
    {
        //
        // Write address low char and mark as allocated.
        //
        *fatTablePtr++ = (unsigned char)(file_address[index] & 0xff) | BIT0;
	
        //	
        // Write address high char.
        //
        *fatTablePtr++ = (unsigned char)((file_address[index]>>8) & 0xff);
		
        //
        // Write length low char.
        //
        *fatTablePtr++ = (unsigned char)(file_length[index] & 0xff);
		
        //
        // Write length high char.
        //
        *fatTablePtr++ = (unsigned char)((file_length[index]>>8) & 0xff);		
    }
	
    //
    // Second, write the FAT.
    // Write in two parts to work with tiny driver.
    //
    ucStatus = nvmem_write(16, 24, 4, fatTable); 
    ucStatus = nvmem_write(16, 24, 24+4, &fatTable[24]); 
	
    //
    // Third, we want to erase any user files.
    //
    memset(fatTable, 0, sizeof(fatTable));
    ucStatus = nvmem_write(16, 16, 52, fatTable); 
	
    return ucStatus;
}

void patch_prog_start()
{
    unsigned short  index;
    unsigned char   *pRMParams;

    printf("Initializing module...\n");

    // Init module and request to load with no patches.
    // This is in order to overwrite restrictions to
    // write to specific places in EEPROM.
    initDriver(1);

    // Read MAC address.
    mac_status = nvmem_get_mac_address(cMacFromEeprom);

    return_status = 1;

    printf("Reading RM parameters...\n");
    while ((return_status) && (counter < 3)) {
        // Read RM parameters.
        // Read in 16 parts to work with tiny driver.
        return_status = 0;
        pRMParams = cRMParamsFromEeprom;
        for (index = 0; index < 16; index++) {
            return_status |= nvmem_read(NVMEM_RM_FILEID, 8, 8*index, pRMParams);
            pRMParams += 8;
        }
        counter++;
    }

    // If RM file is not valid, load the default one.
    if (counter == 3) {
        printf("RM is not valid, loading default one...\n");
        pRMParams = (unsigned char *)cRMdefaultParams;
    } else {
        printf("RM is valid.\n");
        pRMParams = cRMParamsFromEeprom;
    }

    return_status = 1;

    printf("Writing new FAT\n");
    while (return_status) {
        // Write new FAT.
        return_status = fat_write_content(aFATEntries[0], aFATEntries[1]);
    }

    return_status = 1;

    printf("Writing RM parameters...\n");
    while (return_status) {
        // Write RM parameters.
        // Write in 4 parts to work with tiny driver.
        return_status = 0;

        for (index = 0; index < 4; index++) {
            return_status |= nvmem_write(NVMEM_RM_FILEID,
                                         32,
                                         32*index,
                                         (pRMParams + 32*index)); 
        }
    }

    return_status = 1;

    // Write back the MAC address, only if exists.
    if (mac_status == 0) {
        // Zero out MCAST bit if set.
        cMacFromEeprom[0] &= 0xfe;
        printf("Writing back MAC address..\n");
        while (return_status) {
            return_status = nvmem_set_mac_address(cMacFromEeprom);
        }
    }

    // Update driver
    ucStatus_Dr = 1;
    printf("Updating driver patch...\n");
    while (ucStatus_Dr) {
        // Writing driver patch to EEPRROM - PROTABLE CODE
        // Note that the array itself is changing between the
        // different Service Packs.
        ucStatus_Dr = nvmem_write_patch(NVMEM_WLAN_DRIVER_SP_FILEID,
                                        drv_length,
                                        wlan_drv_patch);
    }

    // Update firmware
    ucStatus_FW = 1;
    printf("Updating firmware patch...\n");
    while (ucStatus_FW) {
        // Writing FW patch to EEPRROM - PROTABLE CODE
        // Note that the array itself is changing between the
        // different Service Packs.
        ucStatus_FW = nvmem_write_patch(NVMEM_WLAN_FW_SP_FILEID,
                                        fw_length,
                                        fw_patch);
    }

    printf("Update complete, resetting module\n"\
           "If this doesn't work, reset manually...\n");

    wlan_stop();
    systick_sleep(500);

    // Re-Init module and request to load with patches.
    initDriver(0);

    // If MAC does not exist, it is recommended
    // that the user will write a valid mac address.
    if (mac_status != 0) {
        printf("MAC address is not valid, please write a new one\n");
    }

    // Patch update done
    printf("All done, call wlan.patch_version()\n");
}
