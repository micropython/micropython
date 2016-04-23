
#ifndef SYSTEM_UPDATE_H
#define	SYSTEM_UPDATE_H

#include "file_transfer.h"
#include "static_assert.h"
#include "appender.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef class Stream Stream;
#include <stdint.h>

typedef bool (*ymodem_serial_flash_update_handler)(Stream *serialObj, FileTransfer::Descriptor& file, void*);
#if (PLATFORM_ID==88)
typedef bool (*avrdude_serial_flash_update_handler)(Stream *serialObj, FileTransfer::Descriptor& file, void*);
#endif

void set_ymodem_serial_flash_update_handler(ymodem_serial_flash_update_handler handler);

void set_start_dfu_flasher_serial_speed(uint32_t speed);
void set_start_ymodem_flasher_serial_speed(uint32_t speed);

#if (PLATFORM_ID==88)
void set_avrdude_serial_flash_update_handle(avrdude_serial_flash_update_handler handler);
void set_start_avrdude_flasher_serial_speed(uint32_t speed);
#endif

/**
 * Updates firmware via ymodem from a given stream.
 * @param stream
 * @return true on successful update.
 */
bool system_firmwareUpdate(Stream* stream, void* reserved=NULL);
#if (PLATFORM_ID==88)
bool system_avrdudeFirmwareUpdate(Stream* stream, void* reserved=NULL);
#endif

struct system_file_transfer_t {
    system_file_transfer_t() {
        memset(this, 0, sizeof(*this));
        size = sizeof(*this);
    }

    uint16_t size;
    uint16_t padding;   // reuse if possible
    Stream* stream;
    FileTransfer::Descriptor descriptor;
};

STATIC_ASSERT(system_file_transfer_size, sizeof(system_file_transfer_t)==sizeof(FileTransfer::Descriptor)+8 || sizeof(void*)!=4);

bool system_fileTransfer(system_file_transfer_t* transfer, void* reserved=NULL);

#if (PLATFORM_ID==88)
bool system_avrdudeFileTransfer(system_file_transfer_t* tx, void* reserved=NULL);
#endif

void system_lineCodingBitRateHandler(uint32_t bitrate);

bool system_module_info(appender_fn appender, void* append_data, void* reserved=NULL);
bool append_system_version_info(Appender* appender);

/**
 *
 * @param file
 * @param flags bit 0 set (1) means it's a dry run to check parameters. bit 0 cleared means it's the real thing.
 * @param reserved NULL
 * @return 0 on success.
 */
int Spark_Prepare_For_Firmware_Update(FileTransfer::Descriptor& file, uint32_t flags, void* reserved);

/**
 *
 * @param file
 * @param flags 1 if the image was successfully received. 0 if there was interruption or other error.
 * @param reserved NULL
 * @return 0 on success.
 */
int Spark_Finish_Firmware_Update(FileTransfer::Descriptor& file, uint32_t flags, void* reserved);

/**
 * Provides a chunk of the file data.
 * @param file
 * @param chunk     The chunk data
 * @param reserved
 * @return
 */
int Spark_Save_Firmware_Chunk(FileTransfer::Descriptor& file, const uint8_t* chunk, void* reserved);

typedef enum
{
    /**
     * When 0, no OTA update is pending.
     * When 1, an OTA update is pending, and will start when the SYSTEM_FLAG_OTA_UPDATES_FLAG
     * is set.
     */
    SYSTEM_FLAG_OTA_UPDATE_PENDING=0,

    /**
     * When 0, OTA updates are not started.
     * When 1, OTA updates are started. Default.
     */
    SYSTEM_FLAG_OTA_UPDATE_ENABLED=1,

    /*
     * When 0, no reset is pending.
     * When 1, a reset is pending. The system will perform the reset
     * when SYSTEM_FLAG_RESET_ENABLED is set to 1.
     */
    SYSTEM_FLAG_RESET_PENDING=2,

    /**
     * When 0, the system is not able to perform a system reset.
     * When 1, thee system will reset the device when a reset is pending.
     */
    SYSTEM_FLAG_RESET_ENABLED=3,

    /**
     * When 0, the application code is not paused.
     * When 1, the application code is paused.
     */
    //SYSTEM_FLAG_APPLICATION_PAUSED=4,
    SYSTEM_FLAG_MAX = 4

} system_flag_t;

void system_shutdown_if_needed();
void system_pending_shutdown();

int system_set_flag(system_flag_t flag, uint8_t value, void* reserved);
int system_get_flag(system_flag_t flag, uint8_t* value,void* reserved);

#ifdef __cplusplus
}
#endif


#endif	/* SYSTEM_UPDATE_H */

