/*
 * Copyright 2017-2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_DCP_H_
#define _FSL_DCP_H_

#include "fsl_common.h"

/*! @brief DCP status return codes. */
enum _dcp_status
{
    kStatus_DCP_Again = MAKE_STATUS(kStatusGroup_DCP, 0), /*!< Non-blocking function shall be called again. */
};

/*******************************************************************************
 * Definitions
 *******************************************************************************/

/*!
 * @addtogroup dcp_driver
 * @{
 */
/*! @name Driver version */
/*@{*/
/*! @brief DCP driver version. Version 2.1.1.
 *
 * Current version: 2.1.1
 *
 * Change log:
 * - Version 2.1.1
 *   - Add DCP status clearing when channel operation is complete
 *
 * - 2.1.0
 *   - Add byte/word swap feature for key, input and output data
 *
 * - Version 2.0.0
 *   - Initial version
 */
#define FSL_DCP_DRIVER_VERSION (MAKE_VERSION(2, 1, 1))
/*@}*/

/*! @brief DCP channel enable.
 *
 */
typedef enum _dcp_ch_enable
{
    kDCP_chDisable   = 0U,  /*!< DCP channel disable */
    kDCP_ch0Enable   = 1U,  /*!< DCP channel 0 enable */
    kDCP_ch1Enable   = 2U,  /*!< DCP channel 1 enable */
    kDCP_ch2Enable   = 4U,  /*!< DCP channel 2 enable */
    kDCP_ch3Enable   = 8U,  /*!< DCP channel 3 enable */
    kDCP_chEnableAll = 15U, /*!< DCP channel enable all */
} _dcp_ch_enable_t;

/*! @brief DCP interrupt enable.
 *
 */
typedef enum _dcp_ch_int_enable
{
    kDCP_chIntDisable = 0U, /*!< DCP interrupts disable */
    kDCP_ch0IntEnable = 1U, /*!< DCP channel 0 interrupt enable */
    kDCP_ch1IntEnable = 2U, /*!< DCP channel 1 interrupt enable */
    kDCP_ch2IntEnable = 4U, /*!< DCP channel 2 interrupt enable */
    kDCP_ch3IntEnable = 8U, /*!< DCP channel 3 interrupt enable */
} _dcp_ch_int_enable_t;

/*! @brief DCP channel selection.
 *
 */
typedef enum _dcp_channel
{
    kDCP_Channel0 = (1u << 16), /*!< DCP channel 0. */
    kDCP_Channel1 = (1u << 17), /*!< DCP channel 1. */
    kDCP_Channel2 = (1u << 18), /*!< DCP channel 2. */
    kDCP_Channel3 = (1u << 19), /*!< DCP channel 3. */
} dcp_channel_t;

/*! @brief DCP key slot selection.
 *
 */
typedef enum _dcp_key_slot
{
    kDCP_KeySlot0     = 0U, /*!< DCP key slot 0. */
    kDCP_KeySlot1     = 1U, /*!< DCP key slot 1. */
    kDCP_KeySlot2     = 2U, /*!< DCP key slot 2.*/
    kDCP_KeySlot3     = 3U, /*!< DCP key slot 3. */
    kDCP_OtpKey       = 4U, /*!< DCP OTP key. */
    kDCP_OtpUniqueKey = 5U, /*!< DCP unique OTP key. */
    kDCP_PayloadKey   = 6U, /*!< DCP payload key. */
} dcp_key_slot_t;

/*! @brief DCP key, input & output swap options
 *
 */
typedef enum _dcp_swap
{
    kDCP_NoSwap         = 0x0U,
    kDCP_KeyByteSwap    = 0x40000U,
    kDCP_KeyWordSwap    = 0x80000U,
    kDCP_InputByteSwap  = 0x100000U,
    kDCP_InputWordSwap  = 0x200000U,
    kDCP_OutputByteSwap = 0x400000U,
    kDCP_OutputWordSwap = 0x800000U,
} dcp_swap_t;

/*! @brief DCP's work packet. */
typedef struct _dcp_work_packet
{
    uint32_t nextCmdAddress;
    uint32_t control0;
    uint32_t control1;
    uint32_t sourceBufferAddress;
    uint32_t destinationBufferAddress;
    uint32_t bufferSize;
    uint32_t payloadPointer;
    uint32_t status;
} dcp_work_packet_t;

/*! @brief Specify DCP's key resource and DCP channel. */
typedef struct _dcp_handle
{
    dcp_channel_t channel;  /*!< Specify DCP channel. */
    dcp_key_slot_t keySlot; /*!< For operations with key (such as AES encryption/decryption), specify DCP key slot. */
    uint32_t swapConfig;    /*!< For configuration of key, input, output byte/word swap options */
    uint32_t keyWord[4];
    uint32_t iv[4];
} dcp_handle_t;

/*! @brief DCP's context buffer, used by DCP for context switching between channels. */
typedef struct _dcp_context
{
    uint32_t x[208 / sizeof(uint32_t)];
} dcp_context_t;

/*! @brief DCP's configuration structure. */
typedef struct _dcp_config
{
    bool gatherResidualWrites;      /*!< Enable the ragged writes to the unaligned buffers. */
    bool enableContextCaching;      /*!< Enable the caching of contexts between the operations. */
    bool enableContextSwitching;    /*!< Enable automatic context switching for the channels. */
    uint8_t enableChannel;          /*!< DCP channel enable. */
    uint8_t enableChannelInterrupt; /*!< Per-channel interrupt enable. */
} dcp_config_t;

/*! @} */

/*******************************************************************************
 * AES Definitions
 *******************************************************************************/

/*!
 * @addtogroup dcp_driver_aes
 * @{
 */

/*! AES block size in bytes */
#define DCP_AES_BLOCK_SIZE 16

/*!
 *@}
 */ /* end of dcp_driver_aes */

/*******************************************************************************
 * HASH Definitions
 ******************************************************************************/
/*!
 * @addtogroup dcp_driver_hash
 * @{
 */

/* DCP cannot correctly compute hash for message with zero size. When enabled, driver bypases DCP and returns correct
 * hash value. If you are sure, that the driver will never be called with zero sized message, you can disable this
 * feature to reduce code size */
#define DCP_HASH_CAVP_COMPATIBLE

/*! @brief Supported cryptographic block cipher functions for HASH creation */
typedef enum _dcp_hash_algo_t
{
    kDCP_Sha1,   /*!< SHA_1 */
    kDCP_Sha256, /*!< SHA_256 */
    kDCP_Crc32,  /*!< CRC_32 */
} dcp_hash_algo_t;

/*! @brief DCP HASH Context size. */
#define DCP_SHA_BLOCK_SIZE 128                 /*!< internal buffer block size  */
#define DCP_HASH_BLOCK_SIZE DCP_SHA_BLOCK_SIZE /*!< DCP hash block size  */

/*! @brief DCP HASH Context size. */
#define DCP_HASH_CTX_SIZE 58

/*! @brief Storage type used to save hash context. */
typedef struct _dcp_hash_ctx_t
{
    uint32_t x[DCP_HASH_CTX_SIZE];
} dcp_hash_ctx_t;

/*!
 *@}
 */ /* end of dcp_driver_hash */

/*******************************************************************************
 * API
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @addtogroup dcp_driver
 * @{
 */

/*!
 * @brief   Enables clock to and enables DCP
 *
 * Enable DCP clock and configure DCP.
 *
 * @param base DCP base address
 * @param config Pointer to configuration structure.
 */
void DCP_Init(DCP_Type *base, const dcp_config_t *config);

/*!
 * @brief   Disable DCP clock
 *
 * Reset DCP and Disable DCP clock.
 *
 * @param base DCP base address
 */
void DCP_Deinit(DCP_Type *base);

/*!
 * @brief Gets the default configuration structure.
 *
 * This function initializes the DCP configuration structure to a default value. The default
 * values are as follows.
 *   dcpConfig->gatherResidualWrites = true;
 *   dcpConfig->enableContextCaching = true;
 *   dcpConfig->enableContextSwitching = true;
 *   dcpConfig->enableChannnel = kDCP_chEnableAll;
 *   dcpConfig->enableChannelInterrupt = kDCP_chIntDisable;
 *
 * @param[out] config Pointer to configuration structure.
 */
void DCP_GetDefaultConfig(dcp_config_t *config);

/*!
 * @brief Poll and wait on DCP channel.
 *
 * Polls the specified DCP channel until current it completes activity.
 *
 * @param   base DCP peripheral base address.
 * @param   handle Specifies DCP channel.
 * @return  kStatus_Success When data processing completes without error.
 * @return  kStatus_Fail When error occurs.
 */
status_t DCP_WaitForChannelComplete(DCP_Type *base, dcp_handle_t *handle);

/*!
 *@}
 */ /* end of dcp_driver */

/*******************************************************************************
 * AES API
 ******************************************************************************/

/*!
 * @addtogroup dcp_driver_aes
 * @{
 */

/*!
 * @brief Set AES key to dcp_handle_t struct and optionally to DCP.
 *
 * Sets the AES key for encryption/decryption with the dcp_handle_t structure.
 * The dcp_handle_t input argument specifies keySlot.
 * If the keySlot is kDCP_OtpKey, the function will check the OTP_KEY_READY bit and will return it's ready to use
 * status.
 * For other keySlot selections, the function will copy and hold the key in dcp_handle_t struct.
 * If the keySlot is one of the four DCP SRAM-based keys (one of kDCP_KeySlot0, kDCP_KeySlot1, kDCP_KeySlot2,
 * kDCP_KeySlot3),
 * this function will also load the supplied key to the specified keySlot in DCP.
 *
 * @param   base DCP peripheral base address.
 * @param   handle Handle used for the request.
 * @param   key 0-mod-4 aligned pointer to AES key.
 * @param   keySize AES key size in bytes. Shall equal 16.
 * @return  status from set key operation
 */
status_t DCP_AES_SetKey(DCP_Type *base, dcp_handle_t *handle, const uint8_t *key, size_t keySize);

/*!
 * @brief Encrypts AES on one or multiple 128-bit block(s).
 *
 * Encrypts AES.
 * The source plaintext and destination ciphertext can overlap in system memory.
 *
 * @param base DCP peripheral base address
 * @param handle Handle used for this request.
 * @param plaintext Input plain text to encrypt
 * @param[out] ciphertext Output cipher text
 * @param size Size of input and output data in bytes. Must be multiple of 16 bytes.
 * @return Status from encrypt operation
 */
status_t DCP_AES_EncryptEcb(
    DCP_Type *base, dcp_handle_t *handle, const uint8_t *plaintext, uint8_t *ciphertext, size_t size);

/*!
 * @brief Decrypts AES on one or multiple 128-bit block(s).
 *
 * Decrypts AES.
 * The source ciphertext and destination plaintext can overlap in system memory.
 *
 * @param base DCP peripheral base address
 * @param handle Handle used for this request.
 * @param ciphertext Input plain text to encrypt
 * @param[out] plaintext Output cipher text
 * @param size Size of input and output data in bytes. Must be multiple of 16 bytes.
 * @return Status from decrypt operation
 */
status_t DCP_AES_DecryptEcb(
    DCP_Type *base, dcp_handle_t *handle, const uint8_t *ciphertext, uint8_t *plaintext, size_t size);

/*!
 * @brief Encrypts AES using CBC block mode.
 *
 * Encrypts AES using CBC block mode.
 * The source plaintext and destination ciphertext can overlap in system memory.
 *
 * @param base DCP peripheral base address
 * @param handle Handle used for this request.
 * @param plaintext Input plain text to encrypt
 * @param[out] ciphertext Output cipher text
 * @param size Size of input and output data in bytes. Must be multiple of 16 bytes.
 * @param iv Input initial vector to combine with the first input block.
 * @return Status from encrypt operation
 */
status_t DCP_AES_EncryptCbc(DCP_Type *base,
                            dcp_handle_t *handle,
                            const uint8_t *plaintext,
                            uint8_t *ciphertext,
                            size_t size,
                            const uint8_t iv[16]);

/*!
 * @brief Decrypts AES using CBC block mode.
 *
 * Decrypts AES using CBC block mode.
 * The source ciphertext and destination plaintext can overlap in system memory.
 *
 * @param base DCP peripheral base address
 * @param handle Handle used for this request.
 * @param ciphertext Input cipher text to decrypt
 * @param[out] plaintext Output plain text
 * @param size Size of input and output data in bytes. Must be multiple of 16 bytes.
 * @param iv Input initial vector to combine with the first input block.
 * @return Status from decrypt operation
 */
status_t DCP_AES_DecryptCbc(DCP_Type *base,
                            dcp_handle_t *handle,
                            const uint8_t *ciphertext,
                            uint8_t *plaintext,
                            size_t size,
                            const uint8_t iv[16]);

/*!
 *@}
 */ /* end of dcp_driver_aes */

/*!
 * @addtogroup dcp_nonblocking_driver_aes
 * @{
 */
/*!
 * @brief Encrypts AES using the ECB block mode.
 *
 * Puts AES ECB encrypt work packet to DCP channel.
 *
 * @param base DCP peripheral base address
 * @param handle Handle used for this request.
 * @param[out] dcpPacket Memory for the DCP work packet.
 * @param plaintext Input plain text to encrypt.
 * @param[out] ciphertext Output cipher text
 * @param size Size of input and output data in bytes. Must be multiple of 16 bytes.
 * @return kStatus_Success The work packet has been scheduled at DCP channel.
 * @return kStatus_DCP_Again The DCP channel is busy processing previous request.
 */
status_t DCP_AES_EncryptEcbNonBlocking(DCP_Type *base,
                                       dcp_handle_t *handle,
                                       dcp_work_packet_t *dcpPacket,
                                       const uint8_t *plaintext,
                                       uint8_t *ciphertext,
                                       size_t size);

/*!
 * @brief Decrypts AES using ECB block mode.
 *
 * Puts AES ECB decrypt dcpPacket to DCP input job ring.
 *
 * @param base DCP peripheral base address
 * @param handle Handle used for this request.
 * @param[out] dcpPacket Memory for the DCP work packet.
 * @param ciphertext Input cipher text to decrypt
 * @param[out] plaintext Output plain text
 * @param size Size of input and output data in bytes. Must be multiple of 16 bytes.
 * @return kStatus_Success The work packet has been scheduled at DCP channel.
 * @return kStatus_DCP_Again The DCP channel is busy processing previous request.
 */
status_t DCP_AES_DecryptEcbNonBlocking(DCP_Type *base,
                                       dcp_handle_t *handle,
                                       dcp_work_packet_t *dcpPacket,
                                       const uint8_t *ciphertext,
                                       uint8_t *plaintext,
                                       size_t size);

/*!
 * @brief Encrypts AES using CBC block mode.
 *
 * Puts AES CBC encrypt dcpPacket to DCP input job ring.
 *
 * @param base DCP peripheral base address
 * @param handle Handle used for this request. Specifies jobRing.
 * @param[out] dcpPacket Memory for the DCP work packet.
 * @param plaintext Input plain text to encrypt
 * @param[out] ciphertext Output cipher text
 * @param size Size of input and output data in bytes. Must be multiple of 16 bytes.
 * @param iv Input initial vector to combine with the first input block.
 * @return kStatus_Success The work packet has been scheduled at DCP channel.
 * @return kStatus_DCP_Again The DCP channel is busy processing previous request.
 */
status_t DCP_AES_EncryptCbcNonBlocking(DCP_Type *base,
                                       dcp_handle_t *handle,
                                       dcp_work_packet_t *dcpPacket,
                                       const uint8_t *plaintext,
                                       uint8_t *ciphertext,
                                       size_t size,
                                       const uint8_t *iv);

/*!
 * @brief Decrypts AES using CBC block mode.
 *
 * Puts AES CBC decrypt dcpPacket to DCP input job ring.
 *
 * @param base DCP peripheral base address
 * @param handle Handle used for this request. Specifies jobRing.
 * @param[out] dcpPacket Memory for the DCP work packet.
 * @param ciphertext Input cipher text to decrypt
 * @param[out] plaintext Output plain text
 * @param size Size of input and output data in bytes. Must be multiple of 16 bytes.
 * @param iv Input initial vector to combine with the first input block.
 * @return kStatus_Success The work packet has been scheduled at DCP channel.
 * @return kStatus_DCP_Again The DCP channel is busy processing previous request.
 */
status_t DCP_AES_DecryptCbcNonBlocking(DCP_Type *base,
                                       dcp_handle_t *handle,
                                       dcp_work_packet_t *dcpPacket,
                                       const uint8_t *ciphertext,
                                       uint8_t *plaintext,
                                       size_t size,
                                       const uint8_t *iv);

/*!
 *@}
 */ /* end of dcp_nonblocking_driver_aes */

/*******************************************************************************
 * HASH API
 ******************************************************************************/

/*!
 * @addtogroup dcp_driver_hash
 * @{
 */
/*!
 * @brief Initialize HASH context
 *
 * This function initializes the HASH.
 *
 * @param base DCP peripheral base address
 * @param handle Specifies the DCP channel used for hashing.
 * @param[out] ctx Output hash context
 * @param algo Underlaying algorithm to use for hash computation.
 * @return Status of initialization
 */
status_t DCP_HASH_Init(DCP_Type *base, dcp_handle_t *handle, dcp_hash_ctx_t *ctx, dcp_hash_algo_t algo);

/*!
 * @brief Add data to current HASH
 *
 * Add data to current HASH. This can be called repeatedly with an arbitrary amount of data to be
 * hashed. The functions blocks. If it returns kStatus_Success, the running hash
 * has been updated (DCP has processed the input data), so the memory at @ref input pointer
 * can be released back to system. The DCP context buffer is updated with the running hash
 * and with all necessary information to support possible context switch.
 *
 * @param base DCP peripheral base address
 * @param[in,out] ctx HASH context
 * @param input Input data
 * @param inputSize Size of input data in bytes
 * @return Status of the hash update operation
 */
status_t DCP_HASH_Update(DCP_Type *base, dcp_hash_ctx_t *ctx, const uint8_t *input, size_t inputSize);

/*!
 * @brief Finalize hashing
 *
 * Outputs the final hash (computed by DCP_HASH_Update()) and erases the context.
 *
 * @param[in,out] ctx Input hash context
 * @param[out] output Output hash data
 * @param[in,out] outputSize Optional parameter (can be passed as NULL). On function entry, it specifies the size of
 * output[] buffer. On function return, it stores the number of updated output bytes.
 * @return Status of the hash finish operation
 */
status_t DCP_HASH_Finish(DCP_Type *base, dcp_hash_ctx_t *ctx, uint8_t *output, size_t *outputSize);

/*!
 * @brief Create HASH on given data
 *
 * Perform the full SHA or CRC32 in one function call. The function is blocking.
 *
 * @param base DCP peripheral base address
 * @param handle Handle used for the request.
 * @param algo Underlaying algorithm to use for hash computation.
 * @param input Input data
 * @param inputSize Size of input data in bytes
 * @param[out] output Output hash data
 * @param[out] outputSize Output parameter storing the size of the output hash in bytes
 * @return Status of the one call hash operation.
 */
status_t DCP_HASH(DCP_Type *base,
                  dcp_handle_t *handle,
                  dcp_hash_algo_t algo,
                  const uint8_t *input,
                  size_t inputSize,
                  uint8_t *output,
                  size_t *outputSize);

/*!
 *@}
 */ /* end of dcp_driver_hash */

#if defined(__cplusplus)
}
#endif

#endif /* _FSL_DCP_H_ */
