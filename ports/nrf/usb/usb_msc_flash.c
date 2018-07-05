/**************************************************************************/
/*!
    @file     usb_msc_flash.c
    @author   hathach (tinyusb.org)

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2018, Adafruit Industries (adafruit.com)
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/

#ifdef NRF52840_XXAA

#include "tusb.h"
#include "internal_flash.h"

/*------------------------------------------------------------------*/
/* MACRO TYPEDEF CONSTANT ENUM
 *------------------------------------------------------------------*/
#define MSC_FLASH_ADDR_END      0xED000
#define MSC_FLASH_SIZE          (256*1024)
#define MSC_FLASH_ADDR_START    (MSC_FLASH_ADDR_END-MSC_FLASH_SIZE)
#define MSC_FLASH_BLOCK_SIZE    512

#define FL_PAGE_SZ              4096

/*------------------------------------------------------------------*/
/* VARIABLES
 *------------------------------------------------------------------*/
static scsi_sense_fixed_data_t mscd_sense_data =
{
    .response_code        = 0x70,
    .sense_key            = 0, // no errors
    .additional_sense_len = sizeof(scsi_sense_fixed_data_t) - 8
};

static scsi_mode_parameters_t const msc_dev_mode_para =
{
    .mode_data_length        = 3,
    .medium_type             = 0,
    .device_specific_para    = 0,
    .block_descriptor_length = 0
};

/*------------------------------------------------------------------*/
/*
 *------------------------------------------------------------------*/

//--------------------------------------------------------------------+
// tinyusb callbacks
//--------------------------------------------------------------------+
int32_t tud_msc_scsi_cb (uint8_t rhport, uint8_t lun, uint8_t const scsi_cmd[16], void* buffer, uint16_t bufsize)
{
  // read10 & write10 has their own callback and MUST not be handled here

  void const* ptr = NULL;
  uint16_t len = 0;

  // most scsi handled is input
  bool in_xfer = true;

  switch (scsi_cmd[0])
  {
    case SCSI_CMD_REQUEST_SENSE:
      ptr = &mscd_sense_data;
      len = sizeof(scsi_sense_fixed_data_t);
    break;

    case SCSI_CMD_MODE_SENSE_6:
      ptr = &msc_dev_mode_para;
      len = sizeof(msc_dev_mode_para);
    break;

    case SCSI_CMD_TEST_UNIT_READY:
      ptr = NULL;
      len = 0;
    break;

    case SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL:
      ptr = NULL;
      len = 0;
    break;

    default:
      // negative is error -> Data stage is STALL, status = failed
      return -1;
  }

  // return len must not larger than bufsize
  TU_ASSERT( bufsize >= len );

  if ( ptr && len )
  {
    if(in_xfer)
    {
      memcpy(buffer, ptr, len);
    }else
    {
      // SCSI output
    }
  }

  //------------- clear sense data if it is not request sense command -------------//
  if ( SCSI_CMD_REQUEST_SENSE != scsi_cmd[0] )
  {
    mscd_sense_data.sense_key                  = SCSI_SENSEKEY_NONE;
    mscd_sense_data.additional_sense_code      = 0;
    mscd_sense_data.additional_sense_qualifier = 0;
  }

  return len;
}

/*------------------------------------------------------------------*/
/* Tinyusb Flash READ10 & WRITE10
 *------------------------------------------------------------------*/
int32_t tud_msc_read10_cb (uint8_t rhport, uint8_t lun, uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize)
{
    (void) rhport; (void) lun; (void) offset;

    uint32_t const block_count = bufsize/MSC_FLASH_BLOCK_SIZE;

    internal_flash_read_blocks(buffer, lba, block_count);

    return block_count*MSC_FLASH_BLOCK_SIZE;
}

int32_t tud_msc_write10_cb (uint8_t rhport, uint8_t lun, uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize)
{
    (void) rhport; (void) lun; (void) offset;

    uint32_t const block_count = bufsize/MSC_FLASH_BLOCK_SIZE;

    // bufsize <= CFG_TUD_MSC_BUFSIZE (4096)
    internal_flash_write_blocks(buffer, lba, block_count);

    return block_count*MSC_FLASH_BLOCK_SIZE;
}

void tud_msc_write10_complete_cb(uint8_t rhport, uint8_t lun)
{
    (void) rhport; (void) lun;

    // flush pending cache when write10 is complete
    internal_flash_flush();
}

#endif
