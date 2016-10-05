/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "rom_fs.h"

#include "asf/common/services/usb/class/msc/device/udi_msc.h"
#include "storage.h"

//! This function tests memory state, and starts memory initialization
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
Ctrl_status rom_fs_test_unit_ready(void)
{
	return CTRL_GOOD;
}

//! This function returns the address of the last valid sector
//! @param uint32_t_nb_sector  Pointer to number of sectors (sector=512 bytes)
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
Ctrl_status rom_fs_read_capacity(uint32_t *uint32_t_nb_sector)
{
	*uint32_t_nb_sector = storage_get_block_count();
	return CTRL_GOOD;
}

//! This function returns the write-protected mode
//!
//! @return true if the memory is protected
//!
bool rom_fs_wr_protect(void)
{
	return false;
}

//! This function informs about the memory type
//!
//! @return true if the memory is removable
//!
bool rom_fs_removal(void)
{
	return true;
}

// TODO(tannewt): Transfer more than a single sector at a time if we need more
// speed.
//! This function transfers the memory data to the USB MSC interface
//!
//! @param addr         Sector address to start read
//! @param nb_sector    Number of sectors to transfer (sector=512 bytes)
//!
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
//!
Ctrl_status rom_fs_usb_read_10(uint32_t addr, volatile uint16_t nb_sector)
{
    uint8_t sector_buffer[FLASH_BLOCK_SIZE];
    for (uint16_t sector = 0; sector < nb_sector; sector++) {
        storage_read_block(sector_buffer, addr + sector);
        if (!udi_msc_trans_block(true, sector_buffer, FLASH_BLOCK_SIZE, NULL)) {
            return CTRL_FAIL; // transfer aborted
        }
    }
    return CTRL_GOOD;
}


//! This function transfers the USB MSC data to the memory
//!
//! @param addr         Sector address to start write
//! @param nb_sector    Number of sectors to transfer (sector=512 bytes)
//!
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
//!
Ctrl_status rom_fs_usb_write_10(uint32_t addr, uint16_t nb_sector)
{
    uint8_t sector_buffer[FLASH_BLOCK_SIZE];
    for (uint16_t sector = 0; sector < nb_sector; sector++) {
        if (!udi_msc_trans_block(false, sector_buffer, FLASH_BLOCK_SIZE, NULL)) {
            return CTRL_FAIL; // transfer aborted
        }
        if (!storage_write_block(sector_buffer, addr + sector)) {
            return CTRL_FAIL;
        }
    }
    return CTRL_GOOD;
}
