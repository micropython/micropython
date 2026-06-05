#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "mem_reader.h"

struct lib_mem_reader *
lib_mem_new(const uint8_t *data, size_t len)
{
	struct lib_mem_reader *mr = (struct lib_mem_reader *) malloc(sizeof(struct lib_mem_reader));
	if (mr == NULL)
		return NULL;

	memset(mr, 0, sizeof(struct lib_mem_reader));
	mr->data = data;
	mr->len = len;

	return mr;
}

ssize_t
lib_mem_read(struct lib_mem_reader *mr, uint8_t *buf, size_t buf_len)
{
	ssize_t len = mr->len > buf_len ? buf_len : mr->len;

	memcpy(buf, mr->data, len);
	mr->data = &mr->data[len];
	mr->len -= len;

	return len;
}

void
lib_mem_destroy(struct lib_mem_reader *mr)
{
	free(mr);
}
