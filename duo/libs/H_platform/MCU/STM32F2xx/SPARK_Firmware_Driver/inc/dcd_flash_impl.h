/*
 ******************************************************************************
 *  Copyright (c) 2015 Particle Industries, Inc.  All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************
 */

#include "dcd_flash.h"
#include "dcd.h"
#include "flash_mal.h"
#include "string.h"

/**
 * Implements access to the internal flash, providing the interface expected by dcd.h
 */
class InternalFlashStore
{
public:
    int eraseSector(unsigned address)
    {
        return !FLASH_EraseMemory(FLASH_INTERNAL, address, 1);
    }

    int write(const unsigned offset, const void* data, const unsigned size)
    {
        const uint8_t* data_ptr = (const uint8_t*)data;
        const uint8_t* end_ptr  = data_ptr+size;
        unsigned destination = offset;
        FLASH_Unlock();
        FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

        while (data_ptr < end_ptr)
        {
            FLASH_Status status;
            const int max_tries = 10;
            int tries = 0;

            if ( !(destination & 0x03) && (end_ptr - data_ptr >= 4))  // have a whole word to write
            {
                while ((FLASH_COMPLETE != (status = FLASH_ProgramWord(destination, *(const uint32_t*)data_ptr))) && (tries++ < max_tries));
                destination += 4;
                data_ptr += 4;
            }
            else
            {
                while ((FLASH_COMPLETE != (status = FLASH_ProgramByte(destination, *data_ptr))) && (tries++ < max_tries));
                destination++;
                data_ptr++;
            }

        }
        FLASH_Lock();
        return (memcmp(dataAt(offset), data, size)) ? -1 : 0;
    }

    const uint8_t* dataAt(unsigned address)
    {
        return (const uint8_t*)address;
    }
};

template <typename Store, unsigned sectorSize, unsigned DCD1, unsigned DCD2>
class UpdateDCD : public DCD<Store, sectorSize, DCD1, DCD2>
{
public:
    static const unsigned oldFormatOffset = 7548;

    using base = DCD<Store, sectorSize, DCD1, DCD2>;
    using Sector = typename base::Sector;

    UpdateDCD()
    {
    }

    void migrate()
    {
        if (!this->isInitialized()) {
            // copy data from the old layout
            const uint8_t* sector0 = this->store.dataAt(DCD1);
            const uint8_t* sector1 = this->store.dataAt(DCD2);

            Sector init;
            if (isCurrent(sector0))
                initializeFromSector(sector0, base::Sector_1);
            else if (isCurrent(sector1))
                initializeFromSector(sector1, base::Sector_0);
        }
    }

    void initializeFromSector(const uint8_t* oldSector, Sector newSector)
    {
        this->writeSector(0, oldSector+oldFormatOffset, sectorSize-oldFormatOffset, NULL, newSector);
    }

    inline bool isCurrent(const uint8_t* sector)
    {
        return sector[8]==0 && sector[9]==1;
    }
};

UpdateDCD<InternalFlashStore, 16*1024, 0x8004000, 0x8008000> dcd;

const void* dct_read_app_data (uint32_t offset)
{
    return dcd.read(offset);
}

int dct_write_app_data(const void* data, uint32_t offset, uint32_t size)
{
    return dcd.write(offset, data, size);
}

void dcd_migrate_data()
{
    dcd.migrate();
}
