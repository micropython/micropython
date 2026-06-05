#ifndef LIB_FLASH_READER_H
#define LIB_FLASH_READER_H

#include <stdint.h>
#include <stddef.h>

#define SPI_FLASH_SEC_SIZE 4096

struct lib_flash_reader {
	const esp_partition_t *part;
	size_t offset;
	int buflen;
	int bufpos;
	uint8_t buf[SPI_FLASH_SEC_SIZE];
};

extern struct lib_flash_reader * lib_flash_new(const esp_partition_t *part, size_t offset);
extern ssize_t lib_flash_read(struct lib_flash_reader *fr, uint8_t *buf, size_t buf_len);
extern void lib_flash_destroy(struct lib_flash_reader *fr);

#endif // LIB_FLASH_READER_H
