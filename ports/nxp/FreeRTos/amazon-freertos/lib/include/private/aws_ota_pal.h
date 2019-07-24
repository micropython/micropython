/*
 * Amazon FreeRTOS
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

#ifndef _AWS_OTA_PAL_H_
#define _AWS_OTA_PAL_H_

#include "aws_ota_types.h"
#include "aws_ota_agent.h"

typedef enum {
    eOTA_PAL_ImageState_Unknown = 0,
    eOTA_PAL_ImageState_PendingCommit,
    eOTA_PAL_ImageState_Valid,
    eOTA_PAL_ImageState_Invalid,
} OTA_PAL_ImageState_t;


/** 
 * @brief Abort an OTA transfer. 
 * 
 * Aborts access to an existing open file represented by the OTA file context C. This is only valid
 * for jobs that started successfully.
 * 
 * @note The input OTA_FileContext_t C is checked for NULL by the OTA agent before this
 * function is called. 
 * This function may be called before the file is opened, so the file pointer C->lFileHandle may be NULL 
 * when this function is called.
 * 
 * @param[in] C OTA file context information.
 * 
 * @return The OTA PAL layer error code combined with the MCU specific error code. See OTA Agent 
 * error codes information in aws_ota_agent.h.
 * 
 * The file pointer will be set to NULL after this function returns.
 * kOTA_Err_None is returned when aborting access to the open file was successful.
 * kOTA_Err_FileAbort is returned when aborting access to the open file context was unsuccessful.
 */
OTA_Err_t prvPAL_Abort( OTA_FileContext_t * const C );

/** 
 * @brief Create a new receive file for the data chunks as they come in. 
 * 
 * @note Opens the file indicated in the OTA file context in the MCU file system.
 * 
 * @note The previous image may be present in the designated image download partition or file, so the partition or file
 * must be completely erased or overwritten in this routine.
 * 
 * @note The input OTA_FileContext_t C is checked for NULL by the OTA agent before this
 * function is called. 
 * The device file path is a required field in the OTA job document, so C->pucFilePath is 
 * checked for NULL by the OTA agent before this function is called.
 * 
 * @param[in] C OTA file context information.
 * 
 * @return The OTA PAL layer error code combined with the MCU specific error code. See OTA Agent
 * error codes information in aws_ota_agent.h.
 * 
 * kOTA_Err_None is returned when file creation is successful.
 * kOTA_Err_RxFileTooLarge is returned if the file to be created exceeds the device's non-volatile memory size contraints.
 * kOTA_Err_BootInfoCreateFailed is returned if the bootloader information file creation fails.
 * kOTA_Err_RxFileCreateFailed is returned for other errors creating the file in the device's non-volatile memory.
 */
OTA_Err_t prvPAL_CreateFileForRx( OTA_FileContext_t * const C );

/* @brief Authenticate and close the underlying receive file in the specified OTA context.
 * 
 * @note The input OTA_FileContext_t C is checked for NULL by the OTA agent before this
 * function is called. This function is called only at the end of block ingestion.
 * prvPAL_CreateFileForRx() must succeed before this function is reached, so 
 * C->lFileHandle(or C->pucFile) is never NULL. 
 * The certificate path on the device is a required job document field in the OTA Agent, 
 * so C->pucCertFilepath is never NULL.
 * The file signature key is required job document field in the OTA Agent, so C->pxSignature will
 * never be NULL.
 * 
 * If the signature verification fails, file close should still be attempted.
 * 
 * @param[in] C OTA file context information.
 * 
 * @return The OTA PAL layer error code combined with the MCU specific error code. See OTA Agent 
 * error codes information in aws_ota_agent.h.
 * 
 * kOTA_Err_None is returned on success.
 * kOTA_Err_SignatureCheckFailed is returned when cryptographic signature verification fails.
 * kOTA_Err_BadSignerCert is returned for errors in the certificate itself.
 * kOTA_Err_FileClose is returned when closing the file fails.
 */
OTA_Err_t prvPAL_CloseFile( OTA_FileContext_t * const C );

/**
 * @brief Write a block of data to the specified file at the given offset.
 * 
 * @note The input OTA_FileContext_t C is checked for NULL by the OTA agent before this
 * function is called.
 * The file pointer/handl,e C->pucFile, is checked for NULL by the OTA agent before this 
 * function is called.
 * pacData is checked for NULL by the OTA agent before this function is called.
 * ulBlockSize is validated for range by the OTA agent before this function is called.
 * ulBlockIndex is validated by the OTA agent before this function is called.
 * 
 * @param[in] C OTA file context information.
 * @param[in] ulOffset Byte offset to write to from the beginning of the file.
 * @param[in] pacData Pointer to the byte array of data to write.
 * @param[in] ulBlockSize The number of bytes to write.
 * 
 * @return The number of bytes written on a success, or a negative error code from the platform abstraction layer.
 */
int16_t prvPAL_WriteBlock( OTA_FileContext_t * const C, uint32_t ulOffset, uint8_t * const pcData, uint32_t ulBlockSize );

/** 
 * @brief Activate the newest MCU image received via OTA.
 * 
 * This function shall do whatever is necessary to activate the newest MCU
 * firmware received via OTA. It is typically just a reset of the device.
 * 
 * @note This function SHOULD not return. If it does, the platform doesn't support
 * an automatic reset or an error occurred.
 * 
 * @return The OTA PAL layer error code combined with the MCU specific error code. See OTA Agent 
 * error codes information in aws_ota_agent.h.
 */
OTA_Err_t prvPAL_ActivateNewImage( void );

/**
 * @brief Reset the device.
 *
 * This function shall reset the MCU and cause a reboot of the system.
 *
 * @note This function SHOULD not return. If it does, the platform doesn't support
 * an automatic reset or an error occurred.
 *
 * @return The OTA PAL layer error code combined with the MCU specific error code. See OTA Agent
 * error codes information in aws_ota_agent.h.
 */

OTA_Err_t prvPAL_ResetDevice( void );

/**
 * @brief Attempt to set the state of the OTA update image.
 * 
 * Do whatever is required by the platform to Accept/Reject the OTA update image (or bundle).
 * Refer to the PAL implementation to determine what happens on your platform.
 * 
 * @param[in] eState The desired state of the OTA update image.
 * 
 * @return The OTA_Err_t error code combined with the MCU specific error code. See aws_ota_agent.h for
 *         OTA major error codes and your specific PAL implementation for the sub error code.
 * 
 * Major error codes returned are:
 *
 *   kOTA_Err_None on success.
 *   kOTA_Err_BadImageState: if you specify an invalid OTA_ImageState_t. No sub error code.
 *   kOTA_Err_AbortFailed: failed to roll back the update image as requested by eOTA_ImageState_Aborted.
 *   kOTA_Err_RejectFailed: failed to roll back the update image as requested by eOTA_ImageState_Rejected.
 *   kOTA_Err_CommitFailed: failed to make the update image permanent as requested by eOTA_ImageState_Accepted.
 */
OTA_Err_t prvPAL_SetPlatformImageState ( OTA_ImageState_t eState );

/**
 * @brief Get the state of the OTA update image.
 *
 * We read this at OTA_Init time and when the latest OTA job reports itself in self
 * test. If the update image is in the "pending commit" state, we start a self test
 * timer to assure that we can successfully connect to the OTA services and accept
 * the OTA update image within a reasonable amount of time (user configurable). If
 * we don't satisfy that requirement, we assume there is something wrong with the
 * firmware and automatically reset the device, causing it to roll back to the
 * previously known working code.
 *
 * If the update image state is not in "pending commit," the self test timer is
 * not started.
 *
 * @return An OTA_PAL_ImageState_t. One of the following:
 *   eOTA_PAL_ImageState_PendingCommit (the new firmware image is in the self test phase)
 *   eOTA_PAL_ImageState_Valid         (the new firmware image is already committed)
 *   eOTA_PAL_ImageState_Invalid       (the new firmware image is invalid or non-existent)
 *
 *   NOTE: eOTA_PAL_ImageState_Unknown should NEVER be returned and indicates an implementation error.
 */
OTA_PAL_ImageState_t prvPAL_GetPlatformImageState ( void );

#endif


