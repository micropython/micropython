/*
 * Copyright  2017 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_video_common.h"
#if defined(FSL_RTOS_FREE_RTOS)
#include "FreeRTOS.h"
#include "task.h"
#endif

/*******************************************************************************
 * Code
 ******************************************************************************/

bool VIDEO_IsYUV(video_pixel_format_t format)
{
    if ((kVIDEO_PixelFormatYUYV == format) || (kVIDEO_PixelFormatYVYU == format) ||
        (kVIDEO_PixelFormatUYVY == format) || (kVIDEO_PixelFormatVYUY == format) ||
        (kVIDEO_PixelFormatXYVU == format) || (kVIDEO_PixelFormatXYUV == format))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void VIDEO_DelayMs(uint32_t ms)
{
#if defined(FSL_RTOS_FREE_RTOS)
    TickType_t tick;

    tick = ms * configTICK_RATE_HZ / 1000;

    tick = (0 == tick) ? 1 : tick;

    vTaskDelay(tick);
#else
    uint32_t i;
    uint32_t loopPerMs = SystemCoreClock / 3000;

    while (ms--)
    {
        i = loopPerMs;
        while (i--)
        {
            __NOP();
        }
    }
#endif
}

uint8_t VIDEO_GetPixelSizeBits(video_pixel_format_t pixelFormat)
{
    uint8_t ret;

    switch (pixelFormat)
    {
        case kVIDEO_PixelFormatXRGB8888:
        case kVIDEO_PixelFormatRGBX8888:
        case kVIDEO_PixelFormatXBGR8888:
        case kVIDEO_PixelFormatBGRX8888:
        case kVIDEO_PixelFormatXYUV:
        case kVIDEO_PixelFormatXYVU:
            ret = 32;
            break;

        case kVIDEO_PixelFormatRGB888:
        case kVIDEO_PixelFormatBGR888:
            ret = 24;
            break;

        case kVIDEO_PixelFormatRGB565:
        case kVIDEO_PixelFormatBGR565:
        case kVIDEO_PixelFormatXRGB1555:
        case kVIDEO_PixelFormatRGBX5551:
        case kVIDEO_PixelFormatXBGR1555:
        case kVIDEO_PixelFormatBGRX5551:
        case kVIDEO_PixelFormatXRGB4444:
        case kVIDEO_PixelFormatRGBX4444:
        case kVIDEO_PixelFormatXBGR4444:
        case kVIDEO_PixelFormatBGRX4444:
        case kVIDEO_PixelFormatYUYV:
        case kVIDEO_PixelFormatYVYU:
        case kVIDEO_PixelFormatUYVY:
        case kVIDEO_PixelFormatVYUY:
            ret = 16;
            break;

        default:
            ret = 0;
            break;
    }

    return ret;
}

status_t VIDEO_RINGBUF_Init(video_ringbuf_t *ringbuf, void **buf, uint32_t size)
{
    assert(ringbuf);

    ringbuf->rear  = 0;
    ringbuf->front = 0;
    ringbuf->size  = size;
    ringbuf->buf   = buf;

    return kStatus_Success;
}

status_t VIDEO_RINGBUF_Get(video_ringbuf_t *ringbuf, void **item)
{
    uint32_t front_next;

    if (ringbuf->rear != ringbuf->front)
    {
        *item = ringbuf->buf[ringbuf->front];

        /*
         * Here don't use ringbuf->front = (ringbuf->front + 1) % ringbuf->size,
         * because mod operation might be slow.
         */
        front_next = (ringbuf->front + 1);

        /* Use two steps to make sure ringbuf->front is always a valid value. */
        ringbuf->front = (front_next == ringbuf->size) ? 0 : front_next;

        return kStatus_Success;
    }
    else
    {
        return kStatus_Fail;
    }
}

status_t VIDEO_RINGBUF_Put(video_ringbuf_t *ringbuf, void *item)
{
    /*
     * Here don't use ringbuf->rear = (ringbuf->rear + 1) % ringbuf->size,
     * because mod operation might be slow.
     */
    uint32_t rear_next = ringbuf->rear + 1;

    rear_next = (rear_next == ringbuf->size) ? 0 : rear_next;

    if (rear_next != ringbuf->front)
    {
        ringbuf->buf[ringbuf->rear] = item;
        ringbuf->rear               = rear_next;

        return kStatus_Success;
    }
    /* No room. */
    else
    {
        return kStatus_Fail;
    }
}

uint32_t VIDEO_RINGBUF_GetLength(video_ringbuf_t *ringbuf)
{
    uint32_t ret;

    ret = (ringbuf->rear + ringbuf->size) - ringbuf->front;

    if (ret >= ringbuf->size)
    {
        ret -= ringbuf->size;
    }

    return ret;
}

bool VIDEO_RINGBUF_IsEmpty(video_ringbuf_t *ringbuf)
{
    if (ringbuf->rear == ringbuf->front)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool VIDEO_RINGBUF_IsFull(video_ringbuf_t *ringbuf)
{
    uint32_t rear  = ringbuf->rear;
    uint32_t front = ringbuf->front;

    rear++;

    if (rear >= ringbuf->size)
    {
        rear = 0;
    }

    if (rear == front)
    {
        return true;
    }
    else
    {
        return false;
    }
}

status_t VIDEO_MEMPOOL_Init(video_mempool_t *mempool, void *initMem, uint32_t size, uint32_t count)
{
    memset(mempool, 0, sizeof(video_mempool_t));

    while (count--)
    {
        VIDEO_MEMPOOL_Put(mempool, initMem);
        initMem = (void *)(((uint32_t)initMem) + size);
    }

    return kStatus_Success;
}

void VIDEO_MEMPOOL_InitEmpty(video_mempool_t *mempool)
{
    mempool->pool = NULL;
    mempool->cnt  = 0;
}

void VIDEO_MEMPOOL_Put(video_mempool_t *mempool, void *mem)
{
    *(void **)mem = mempool->pool;
    mempool->pool = mem;
    mempool->cnt++;
}

void *VIDEO_MEMPOOL_Get(video_mempool_t *mempool)
{
    void *mem = mempool->pool;

    if (NULL != mem)
    {
        mempool->cnt--;
        mempool->pool = *(void **)mem;
    }

    return mem;
}

uint32_t VIDEO_MEMPOOL_GetCount(video_mempool_t *mempool)
{
    return mempool->cnt;
}
