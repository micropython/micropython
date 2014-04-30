/*****************************************************************************
*
*  nvmem.h  - CC3000 Host Driver Implementation.
*  Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
*
* Adapted for use with the Arduino/AVR by KTOWN (Kevin Townsend) 
* & Limor Fried for Adafruit Industries
* This library works with the Adafruit CC3000 breakout 
*	----> https://www.adafruit.com/products/1469
* Adafruit invests time and resources providing this open source code,
* please support Adafruit and open-source hardware by purchasing
* products from Adafruit!
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/
#ifndef __NVRAM_H__
#define __NVRAM_H__

#include "cc3000_common.h"


//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef  __cplusplus
extern "C" {
#endif


//*****************************************************************************
//
//! \addtogroup nvmem_api
//! @{
//
//*****************************************************************************

/****************************************************************************
**
**	Definitions for File IDs
**	
****************************************************************************/
/* NVMEM file ID - system files*/
#define NVMEM_NVS_FILEID 							(0)
#define NVMEM_NVS_SHADOW_FILEID 					(1)
#define NVMEM_WLAN_CONFIG_FILEID 					(2)
#define NVMEM_WLAN_CONFIG_SHADOW_FILEID 			(3)
#define NVMEM_WLAN_DRIVER_SP_FILEID					(4)
#define NVMEM_WLAN_FW_SP_FILEID						(5)
#define NVMEM_MAC_FILEID 							(6)
#define NVMEM_FRONTEND_VARS_FILEID 					(7)
#define NVMEM_IP_CONFIG_FILEID 						(8)
#define NVMEM_IP_CONFIG_SHADOW_FILEID 				(9)
#define NVMEM_BOOTLOADER_SP_FILEID 					(10)
#define NVMEM_RM_FILEID			 					(11)

/* NVMEM file ID - user files*/
#define NVMEM_AES128_KEY_FILEID	 					(12)
#define NVMEM_SHARED_MEM_FILEID	 					(13)

/*  max entry in order to invalid nvmem              */
#define NVMEM_MAX_ENTRY                              (16)


//*****************************************************************************
//
//!  nvmem_read
//!
//!  @param  ulFileId   nvmem file id:\n
//!                     NVMEM_NVS_FILEID, NVMEM_NVS_SHADOW_FILEID,
//!                     NVMEM_WLAN_CONFIG_FILEID, NVMEM_WLAN_CONFIG_SHADOW_FILEID,
//!                     NVMEM_WLAN_DRIVER_SP_FILEID, NVMEM_WLAN_FW_SP_FILEID,
//!                     NVMEM_MAC_FILEID, NVMEM_FRONTEND_VARS_FILEID,
//!                     NVMEM_IP_CONFIG_FILEID, NVMEM_IP_CONFIG_SHADOW_FILEID,
//!                     NVMEM_BOOTLOADER_SP_FILEID, NVMEM_RM_FILEID,
//!                     and user files 12-15.
//!  @param  ulLength    number of bytes to read 
//!  @param  ulOffset    ulOffset in file from where to read  
//!  @param  buff        output buffer pointer
//!
//!  @return       number of bytes read, otherwise error.
//!
//!  @brief       Reads data from the file referred by the ulFileId parameter. 
//!               Reads data from file ulOffset till length. Err if the file can't
//!               be used, is invalid, or if the read is out of bounds. 
//!	 
//*****************************************************************************

extern signed long nvmem_read(unsigned long file_id, unsigned long length, unsigned long offset, unsigned char *buff);

//*****************************************************************************
//
//!  nvmem_write
//!
//!  @param  ulFileId nvmem file id:\n
//!                   NVMEM_WLAN_DRIVER_SP_FILEID, NVMEM_WLAN_FW_SP_FILEID,
//!                   NVMEM_MAC_FILEID, NVMEM_BOOTLOADER_SP_FILEID,
//!                   and user files 12-15.
//!  @param  ulLength       number of bytes to write  
//!  @param  ulEntryOffset  offset in file to start write operation from 
//!  @param  buff           data to write
//!
//!  @return       on success 0, error otherwise.
//!
//!  @brief       Write data to nvmem.
//!               writes data to file referred by the ulFileId parameter. 
//!               Writes data to file ulOffset till ulLength.The file id will be 
//!               marked invalid till the write is done. The file entry doesn't
//!               need to be valid - only allocated.
//!	 
//*****************************************************************************

extern signed long nvmem_write(unsigned long ulFileId, unsigned long ulLength, unsigned long ulEntryOffset, unsigned char *buff);


//*****************************************************************************
//
//!  nvmem_set_mac_address
//!
//!  @param  mac   mac address to be set
//!
//!  @return       on success 0, error otherwise.
//!
//!  @brief       Write MAC address to EEPROM. 
//!               mac address as appears over the air (OUI first)
//!	 
//*****************************************************************************
extern	unsigned char nvmem_set_mac_address(unsigned char *mac);


//*****************************************************************************
//
//!  nvmem_get_mac_address
//!
//!  @param[out]  mac   mac address  
//!
//!  @return       on success 0, error otherwise.
//!
//!  @brief       Read MAC address from EEPROM. 
//!               mac address as appears over the air (OUI first)
//!	 
//*****************************************************************************
extern	unsigned char nvmem_get_mac_address(unsigned char *mac);


//*****************************************************************************
//
//!  nvmem_write_patch
//!
//!  @param  ulFileId   nvmem file id:\n
//!                     NVMEM_WLAN_DRIVER_SP_FILEID, NVMEM_WLAN_FW_SP_FILEID,
//!  @param  spLength   number of bytes to write 
//!  @param  spData     SP data to write
//!
//!  @return       on success 0, error otherwise.
//!
//!  @brief      program a patch to a specific file ID. 
//!              The SP data is assumed to be organized in 2-dimensional.
//!              Each line is SP_PORTION_SIZE bytes long. Actual programming is 
//!              applied in SP_PORTION_SIZE bytes portions.
//!	 
//*****************************************************************************
extern	unsigned char nvmem_write_patch(unsigned long ulFileId, unsigned long spLength, const unsigned char *spData);


//*****************************************************************************
//
//!  nvmem_read_sp_version
//!
//!  @param[out]  patchVer    first number indicates package ID and the second 
//!                           number indicates package build number   
//!
//!  @return       on success 0, error otherwise.
//!
//!  @brief      Read patch version. read package version (WiFi FW patch, 
//!              driver-supplicant-NS patch, bootloader patch)
//!	 
//*****************************************************************************
#ifndef CC3000_TINY_DRIVER 
extern	unsigned char nvmem_read_sp_version(unsigned char* patchVer);
#endif

//*****************************************************************************
//
//!  nvmem_create_entry
//!
//!  @param       ulFileId    nvmem file Id:\n
//!                           * NVMEM_AES128_KEY_FILEID: 12
//!                           * NVMEM_SHARED_MEM_FILEID: 13
//!                           * and fileIDs 14 and 15
//!  @param       ulNewLen    entry ulLength  
//!
//!  @return       on success 0, error otherwise.
//!
//!  @brief      Create new file entry and allocate space on the NVMEM. 
//!              Applies only to user files.
//!              Modify the size of file.
//!              If the entry is unallocated - allocate it to size 
//!              ulNewLen (marked invalid).
//!              If it is allocated then deallocate it first.
//!              To just mark the file as invalid without resizing - 
//!              set ulNewLen=0.
//!	 
//*****************************************************************************
extern int8_t nvmem_create_entry(unsigned long file_id, unsigned long newlen);


//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************


//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************


#ifdef  __cplusplus
}
#endif // __cplusplus

#endif // __NVRAM_H__
