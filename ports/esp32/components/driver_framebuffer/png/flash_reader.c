#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include <esp_partition.h>

#include "flash_reader.h"

struct lib_flash_reader *
lib_flash_new(const esp_partition_t *part, size_t offset)
{
	struct lib_flash_reader *fr = (struct lib_flash_reader *) malloc(sizeof(struct lib_flash_reader));
	if (fr == NULL)
		return NULL;

	memset(fr, 0, sizeof(struct lib_flash_reader));
	fr->part = part;
	fr->offset = offset;

	return fr;
}

ssize_t
lib_flash_read(struct lib_flash_reader *fr, uint8_t *buf, size_t buf_len)
{
	ssize_t res = 0;
	while (buf_len > 0)
	{
		if (fr->buflen == 0)
		{
			int offset = fr->offset & (SPI_FLASH_SEC_SIZE-1);
			fr->offset &= ~(SPI_FLASH_SEC_SIZE-1);

			esp_err_t err = esp_partition_read(fr->part, fr->offset, fr->buf, SPI_FLASH_SEC_SIZE);
			if (err != ESP_OK)
				return -1;

			fr->bufpos = offset;
			fr->buflen = SPI_FLASH_SEC_SIZE - offset;
			fr->offset += SPI_FLASH_SEC_SIZE;
		}

		if (fr->buflen)
		{
			ssize_t maxcopy = fr->buflen > buf_len ? buf_len : fr->buflen;
			memcpy(buf, &fr->buf[fr->bufpos], maxcopy);
			buf = &buf[maxcopy];
			buf_len -= maxcopy;
			res += maxcopy;
			fr->bufpos += maxcopy;
			fr->buflen -= maxcopy;
		}
	}

	return res;
}

void
lib_flash_destroy(struct lib_flash_reader *fr)
{
	free(fr);
}
