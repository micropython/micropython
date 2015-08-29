/*
 * @brief Common ring buffer support functions
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include <string.h>
#include "ring_buffer.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

#define RB_INDH(rb)                ((rb)->head & ((rb)->count - 1))
#define RB_INDT(rb)                ((rb)->tail & ((rb)->count - 1))

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Initialize ring buffer */
int RingBuffer_Init(RINGBUFF_T *RingBuff, void *buffer, int itemSize, int count)
{
	RingBuff->data = buffer;
	RingBuff->count = count;
	RingBuff->itemSz = itemSize;
	RingBuff->head = RingBuff->tail = 0;

	return 1;
}

/* Insert a single item into Ring Buffer */
int RingBuffer_Insert(RINGBUFF_T *RingBuff, const void *data)
{
	uint8_t *ptr = RingBuff->data;

	/* We cannot insert when queue is full */
	if (RingBuffer_IsFull(RingBuff))
		return 0;

	ptr += RB_INDH(RingBuff) * RingBuff->itemSz;
	memcpy(ptr, data, RingBuff->itemSz);
	RingBuff->head++;

	return 1;
}

/* Insert multiple items into Ring Buffer */
int RingBuffer_InsertMult(RINGBUFF_T *RingBuff, const void *data, int num)
{
	uint8_t *ptr = RingBuff->data;
	int cnt1, cnt2;

	/* We cannot insert when queue is full */
	if (RingBuffer_IsFull(RingBuff))
		return 0;

	/* Calculate the segment lengths */
	cnt1 = cnt2 = RingBuffer_GetFree(RingBuff);
	if (RB_INDH(RingBuff) + cnt1 >= RingBuff->count)
		cnt1 = RingBuff->count - RB_INDH(RingBuff);
	cnt2 -= cnt1;

	cnt1 = MIN(cnt1, num);
	num -= cnt1;

	cnt2 = MIN(cnt2, num);
	num -= cnt2;

	/* Write segment 1 */
	ptr += RB_INDH(RingBuff) * RingBuff->itemSz;
	memcpy(ptr, data, cnt1 * RingBuff->itemSz);
	RingBuff->head += cnt1;

	/* Write segment 2 */
	ptr = (uint8_t *) RingBuff->data + RB_INDH(RingBuff) * RingBuff->itemSz;
	data = (const uint8_t *) data + cnt1 * RingBuff->itemSz;
	memcpy(ptr, data, cnt2 * RingBuff->itemSz);
	RingBuff->head += cnt2;

	return cnt1 + cnt2;
}

/* Pop single item from Ring Buffer */
int RingBuffer_Pop(RINGBUFF_T *RingBuff, void *data)
{
	uint8_t *ptr = RingBuff->data;

	/* We cannot pop when queue is empty */
	if (RingBuffer_IsEmpty(RingBuff))
		return 0;

	ptr += RB_INDT(RingBuff) * RingBuff->itemSz;
	memcpy(data, ptr, RingBuff->itemSz);
	RingBuff->tail++;

	return 1;
}

/* Pop multiple items from Ring buffer */
int RingBuffer_PopMult(RINGBUFF_T *RingBuff, void *data, int num)
{
	uint8_t *ptr = RingBuff->data;
	int cnt1, cnt2;

	/* We cannot insert when queue is empty */
	if (RingBuffer_IsEmpty(RingBuff))
		return 0;

	/* Calculate the segment lengths */
	cnt1 = cnt2 = RingBuffer_GetCount(RingBuff);
	if (RB_INDT(RingBuff) + cnt1 >= RingBuff->count)
		cnt1 = RingBuff->count - RB_INDT(RingBuff);
	cnt2 -= cnt1;

	cnt1 = MIN(cnt1, num);
	num -= cnt1;

	cnt2 = MIN(cnt2, num);
	num -= cnt2;

	/* Write segment 1 */
	ptr += RB_INDT(RingBuff) * RingBuff->itemSz;
	memcpy(data, ptr, cnt1 * RingBuff->itemSz);
	RingBuff->tail += cnt1;

	/* Write segment 2 */
	ptr = (uint8_t *) RingBuff->data + RB_INDT(RingBuff) * RingBuff->itemSz;
	data = (uint8_t *) data + cnt1 * RingBuff->itemSz;
	memcpy(data, ptr, cnt2 * RingBuff->itemSz);
	RingBuff->tail += cnt2;

	return cnt1 + cnt2;
}
