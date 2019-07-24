/*
 * Amazon FreeRTOS Demo Bootloader V1.4.6
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */


/**
 * @file aws_boot_partition.h
 * @brief Boot Partition header..
 */

#ifndef _AWS_BOOT_PARTITION_H_
#define _AWS_BOOT_PARTITION_H_

/*Microchip includes.*/
#include "system_definitions.h"

/* Bootloader includes.*/
#include "aws_boot_loader.h"
#include "aws_boot_flash_info.h"

/**
 * @brief Magic code for partition entry.
 */
#define FLASH_PARTITION_MAGIC_CODE            0xCA5CADED

/**
 * @brief Maximum size of partition label.
 */
#define FLASH_PARTITION_LABEL_SIZE            ( 16U )

/**
 * @brief Types and subtypes of partitions.
 */
#define FLASH_PARTITION_TYPE_APP              ( 0U )
#define FLASH_PARTITION_SUBTYPE_APP_OTA       ( 1U )
#define FLASH_PARTITION_SUBTYPE_APP_DEFAULT   ( 2U )

#define FLASH_PARTITION_TYPE_DATA             ( 1U )
#define FLASH_PARTITION_SUBTYPE_DATA_WIFI     ( 1U )
#define FLASH_PARTITION_SUBTYPE_DATA_USER     ( 2U )

/**
 * @brief Structure definition for flash area.
 */
typedef struct
{
    uint8_t ucFlashAreaId;   /**< Flash area ID.*/
    uint8_t ucFlashDeviceId; /**< Flash device ID.*/
    uint32_t ulOffset;       /**< Offset of flash area from base.*/
    uint32_t ulSize;         /**< Size of the flash area.*/
} FLASHArea_t;

/**
 * @brief Structure definition for partition.
 */
typedef struct
{
    uint32_t ulMagic;                                       /**< Magic code for partition entry.*/
    uint8_t ucPartitionLabel[ FLASH_PARTITION_LABEL_SIZE ]; /**< Partition label.*/
    uint8_t ucPartitionType;                                /**< Type of partition.*/
    uint8_t ucPartitionSubType;                             /**< subtype of partition.*/
    FLASHArea_t xFlashArea;                                 /**< Flash area info for partition.*/
    uint32_t ulFlags;                                       /**< partition flags.*/
} FLASHPartition_t;

/**
 * @brief Structure definition for partition info on platform.
 */
typedef struct
{
    void * pvDefaultAppExecAddress;                                          /**< Default address to execute.*/
    BOOTImageDescriptor_t * paxOTAAppDescriptor[ FLASH_PARTITIONS_OTA_MAX ]; /**< Descriptor handles for OTA banks.*/
    uint8_t ucNumOfApps;                                                     /**< Number of applications.*/
} BOOTPartition_Info_t;

#endif /* ifndef _AWS_BOOT_PARTITION_H_ */
