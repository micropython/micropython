#include <stdint.h>
#include <string.h>
#include <stm32f4xx_hal.h>

#include "misc.h"
#include "systick.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "led.h"
#include "flash.h"
#include "storage.h"

#define CACHE_MEM_START_ADDR (0x10000000) // CCM data RAM, 64k
#define FLASH_PART1_START_BLOCK (0x100)
#define FLASH_PART1_NUM_BLOCKS (224) // 16k+16k+16k+64k=112k
#define FLASH_MEM_START_ADDR (0x08004000) // sector 1, 16k

static bool flash_is_initialised = false;
static bool flash_cache_dirty;
static uint32_t flash_cache_sector_id;
static uint32_t flash_cache_sector_start;
static uint32_t flash_cache_sector_size;
static uint32_t flash_tick_counter_last_write;

static void flash_cache_flush(void) {
    if (flash_cache_dirty) {
        // sync the cache RAM buffer by writing it to the flash page
        flash_write(flash_cache_sector_start, (const uint32_t*)CACHE_MEM_START_ADDR, flash_cache_sector_size / 4);
        flash_cache_dirty = false;
        // indicate a clean cache with LED off
        led_state(PYB_LED_R1, 0);
    }
}

static uint8_t *flash_cache_get_addr_for_write(uint32_t flash_addr) {
    uint32_t flash_sector_start;
    uint32_t flash_sector_size;
    uint32_t flash_sector_id = flash_get_sector_info(flash_addr, &flash_sector_start, &flash_sector_size);
    if (flash_cache_sector_id != flash_sector_id) {
        flash_cache_flush();
        memcpy((void*)CACHE_MEM_START_ADDR, (const void*)flash_sector_start, flash_sector_size);
        flash_cache_sector_id = flash_sector_id;
        flash_cache_sector_start = flash_sector_start;
        flash_cache_sector_size = flash_sector_size;
    }
    flash_cache_dirty = true;
    // indicate a dirty cache with LED on
    led_state(PYB_LED_R1, 1);
    return (uint8_t*)CACHE_MEM_START_ADDR + flash_addr - flash_sector_start;
}

static uint8_t *flash_cache_get_addr_for_read(uint32_t flash_addr) {
    uint32_t flash_sector_start;
    uint32_t flash_sector_size;
    uint32_t flash_sector_id = flash_get_sector_info(flash_addr, &flash_sector_start, &flash_sector_size);
    if (flash_cache_sector_id == flash_sector_id) {
        // in cache, copy from there
        return (uint8_t*)CACHE_MEM_START_ADDR + flash_addr - flash_sector_start;
    }
    // not in cache, copy straight from flash
    return (uint8_t*)flash_addr;
}

void storage_init(void) {
    if (!flash_is_initialised) {
        flash_cache_dirty = false;
        flash_cache_sector_id = 0;
        flash_is_initialised = true;
        flash_tick_counter_last_write = 0;
    }
}

uint32_t storage_get_block_size(void) {
    return FLASH_BLOCK_SIZE;
}

uint32_t storage_get_block_count(void) {
    return FLASH_PART1_START_BLOCK + FLASH_PART1_NUM_BLOCKS;
}

bool storage_needs_flush(void) {
    // wait 2 seconds after last write to flush
    return flash_cache_dirty && sys_tick_has_passed(flash_tick_counter_last_write, 2000);
}

void storage_flush(void) {
    flash_cache_flush();
}

static void build_partition(uint8_t *buf, int boot, int type, uint32_t start_block, uint32_t num_blocks) {
    buf[0] = boot;

    if (num_blocks == 0) {
        buf[1] = 0;
        buf[2] = 0;
        buf[3] = 0;
    } else {
        buf[1] = 0xff;
        buf[2] = 0xff;
        buf[3] = 0xff;
    }

    buf[4] = type;

    if (num_blocks == 0) {
        buf[5] = 0;
        buf[6] = 0;
        buf[7] = 0;
    } else {
        buf[5] = 0xff;
        buf[6] = 0xff;
        buf[7] = 0xff;
    }

    buf[8] = start_block;
    buf[9] = start_block >> 8;
    buf[10] = start_block >> 16;
    buf[11] = start_block >> 24;

    buf[12] = num_blocks;
    buf[13] = num_blocks >> 8;
    buf[14] = num_blocks >> 16;
    buf[15] = num_blocks >> 24;
}

bool storage_read_block(uint8_t *dest, uint32_t block) {
    //printf("RD %u\n", block);
    if (block == 0) {
        // fake the MBR so we can decide on our own partition table

        for (int i = 0; i < 446; i++) {
            dest[i] = 0;
        }

        build_partition(dest + 446, 0, 0x01 /* FAT12 */, FLASH_PART1_START_BLOCK, FLASH_PART1_NUM_BLOCKS);
        build_partition(dest + 462, 0, 0, 0, 0);
        build_partition(dest + 478, 0, 0, 0, 0);
        build_partition(dest + 494, 0, 0, 0, 0);

        dest[510] = 0x55;
        dest[511] = 0xaa;

        return true;

    } else if (FLASH_PART1_START_BLOCK <= block && block < FLASH_PART1_START_BLOCK + FLASH_PART1_NUM_BLOCKS) {
        // non-MBR block, get data from flash memory, possibly via cache
        uint32_t flash_addr = FLASH_MEM_START_ADDR + (block - FLASH_PART1_START_BLOCK) * FLASH_BLOCK_SIZE;
        uint8_t *src = flash_cache_get_addr_for_read(flash_addr);
        memcpy(dest, src, FLASH_BLOCK_SIZE);
        return true;

    } else {
        // bad block number
        return false;
    }
}

bool storage_write_block(const uint8_t *src, uint32_t block) {
    //printf("WR %u\n", block);
    if (block == 0) {
        // can't write MBR, but pretend we did
        return true;

    } else if (FLASH_PART1_START_BLOCK <= block && block < FLASH_PART1_START_BLOCK + FLASH_PART1_NUM_BLOCKS) {
        // non-MBR block, copy to cache
        uint32_t flash_addr = FLASH_MEM_START_ADDR + (block - FLASH_PART1_START_BLOCK) * FLASH_BLOCK_SIZE;
        uint8_t *dest = flash_cache_get_addr_for_write(flash_addr);
        memcpy(dest, src, FLASH_BLOCK_SIZE);
        flash_tick_counter_last_write = HAL_GetTick();
        return true;

    } else {
        // bad block number
        return false;
    }
}
