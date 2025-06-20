#ifndef __TI_2DEF2_H__
#define __TI_2DEF2_H__

#include "py/mphal.h"

#define LEN 256


typedef struct shared_iram
{
	uint8_t xport_out_data[LEN];
} shared_iram_s;

typedef struct fifo_descriptor
{
    uint8_t buf[LEN];
	uint16_t head; 
	uint16_t tail;
	uint16_t cnt;
	bool busy;
}fifo_descriptor_t;

#endif
