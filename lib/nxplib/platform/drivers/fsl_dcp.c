/*
 * Copyright 2017-2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_dcp.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.dcp"
#endif

/*! Compile time sizeof() check */
#define BUILD_ASSURE(condition, msg) extern int msg[1 - 2 * (!(condition))] __attribute__((unused))

#define dcp_memcpy memcpy

/*! Internal states of the HASH creation process */
typedef enum _dcp_hash_algo_state
{
    kDCP_StateHashInit = 1u, /*!< Init state. */
    kDCP_StateHashUpdate,    /*!< Update state. */
} dcp_hash_algo_state_t;

/*! multiple of 64-byte block represented as byte array of 32-bit words */
typedef union _dcp_hash_block
{
    uint32_t w[DCP_HASH_BLOCK_SIZE / 4]; /*!< array of 32-bit words */
    uint8_t b[DCP_HASH_BLOCK_SIZE];      /*!< byte array */
} dcp_hash_block_t;

/*! internal dcp_hash context structure */
typedef struct _dcp_hash_ctx_internal
{
    dcp_hash_block_t blk;        /*!< memory buffer. only full blocks are written to DCP during hash updates */
    size_t blksz;                /*!< number of valid bytes in memory buffer */
    dcp_hash_algo_t algo;        /*!< selected algorithm from the set of supported algorithms */
    dcp_hash_algo_state_t state; /*!< finite machine state of the hash software process */
    uint32_t fullMessageSize;    /*!< track message size */
    uint32_t ctrl0;              /*!< HASH_INIT and HASH_TERM flags */
    uint32_t runningHash[9];     /*!< running hash. up to SHA-256 plus size, that is 36 bytes. */
    dcp_handle_t *handle;
} dcp_hash_ctx_internal_t;

/*!< SHA-1/SHA-2 digest length in bytes  */
enum _dcp_hash_digest_len
{
    kDCP_OutLenSha1   = 20u,
    kDCP_OutLenSha256 = 32u,
    kDCP_OutLenCrc32  = 4u,
};

enum _dcp_work_packet_bit_definitions
{
    kDCP_CONTROL0_DECR_SEMAPHOR      = 1u << 1,  /* DECR_SEMAPHOR */
    kDCP_CONTROL0_ENABLE_HASH        = 1u << 6,  /* ENABLE_HASH */
    kDCP_CONTROL0_HASH_INIT          = 1u << 12, /* HASH_INIT */
    kDCP_CONTROL0_HASH_TERM          = 1u << 13, /* HASH_TERM */
    kDCP_CONTROL1_HASH_SELECT_SHA256 = 2u << 16,
    kDCP_CONTROL1_HASH_SELECT_SHA1   = 0u << 16,
    kDCP_CONTROL1_HASH_SELECT_CRC32  = 1u << 16,
};

/*! 64-byte block represented as byte array of 16 32-bit words */
typedef union _dcp_sha_block
{
    uint32_t w[64 / 4]; /*!< array of 32-bit words */
    uint8_t b[64];      /*!< byte array */
} dcp_sha_block_t;

#if defined(DCP_HASH_CAVP_COMPATIBLE)
/* result of sha1 hash for message with zero size */
static uint8_t s_nullSha1[] = {0xda, 0x39, 0xa3, 0xee, 0x5e, 0x6b, 0x4b, 0x0d, 0x32, 0x55,
                               0xbf, 0xef, 0x95, 0x60, 0x18, 0x90, 0xaf, 0xd8, 0x07, 0x09};
/* result of sha256 hash for message with zero size */
static uint8_t s_nullSha256[] = {0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14, 0x9a, 0xfb, 0xf4,
                                 0xc8, 0x99, 0x6f, 0xb9, 0x24, 0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b,
                                 0x93, 0x4c, 0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55};
#endif /* DCP_HASH_CAVP_COMPATIBLE */

/*******************************************************************************
 * Variables
 ******************************************************************************/
static dcp_context_t s_dcpContextSwitchingBuffer;

/*******************************************************************************
 * Code
 ******************************************************************************/

static void dcp_reverse_and_copy(uint8_t *src, uint8_t *dest, size_t src_len)
{
    for (int i = 0; i < src_len; i++)
    {
        dest[i] = src[src_len - 1 - i];
    }
}

static status_t dcp_get_channel_status(DCP_Type *base, dcp_channel_t channel)
{
    uint32_t statReg = 0;
    uint32_t semaReg = 0;
    status_t status  = kStatus_Fail;

    switch (channel)
    {
        case kDCP_Channel0:
            statReg = base->CH0STAT;
            semaReg = base->CH0SEMA;
            break;

        case kDCP_Channel1:
            statReg = base->CH1STAT;
            semaReg = base->CH1SEMA;
            break;

        case kDCP_Channel2:
            statReg = base->CH2STAT;
            semaReg = base->CH2SEMA;
            break;

        case kDCP_Channel3:
            statReg = base->CH3STAT;
            semaReg = base->CH3SEMA;
            break;

        default:
            break;
    }

    if (!((semaReg & DCP_CH0SEMA_VALUE_MASK) || (statReg & DCP_CH0STAT_ERROR_CODE_MASK)))
    {
        status = kStatus_Success;
    }

    return status;
}

static void dcp_clear_status(DCP_Type *base)
{
    volatile uint32_t *dcpStatClrPtr = &base->STAT + 2u;
    *dcpStatClrPtr                   = 0xFFu;

    while(base->STAT & 0xffu)
    {
    }
}

static void dcp_clear_channel_status(DCP_Type *base, uint32_t mask)
{
    volatile uint32_t *chStatClrPtr;

    if (mask & kDCP_Channel0)
    {
        chStatClrPtr  = &base->CH0STAT + 2u;
        *chStatClrPtr = 0xFFu;
    }
    if (mask & kDCP_Channel1)
    {
        chStatClrPtr  = &base->CH1STAT + 2u;
        *chStatClrPtr = 0xFFu;
    }
    if (mask & kDCP_Channel2)
    {
        chStatClrPtr  = &base->CH2STAT + 2u;
        *chStatClrPtr = 0xFFu;
    }
    if (mask & kDCP_Channel3)
    {
        chStatClrPtr  = &base->CH3STAT + 2u;
        *chStatClrPtr = 0xFFu;
    }
}

static status_t dcp_aes_set_sram_based_key(DCP_Type *base, dcp_handle_t *handle, const uint8_t *key)
{
    base->KEY = DCP_KEY_INDEX(handle->keySlot) | DCP_KEY_SUBWORD(0);
    /* move the key by 32-bit words */
    int i          = 0;
    size_t keySize = 16u;
    while (keySize)
    {
        keySize -= sizeof(uint32_t);
        base->KEYDATA = ((uint32_t *)(uintptr_t)key)[i];
        i++;
    }
    return kStatus_Success;
}

/* Disable optimizations for GCC to prevent instruction reordering */
#if defined(__GNUC__)
#pragma GCC push_options
#pragma GCC optimize ("O0")
#endif
static status_t dcp_schedule_work(DCP_Type *base, dcp_handle_t *handle, dcp_work_packet_t *dcpPacket)
{
    status_t status;

    /* check if our channel is active */
    if ((base->STAT & (uint32_t)handle->channel) != handle->channel)
    {
        /* disable global interrupt */
        uint32_t currPriMask = DisableGlobalIRQ();

        /* re-check if our channel is still available */
        if ((base->STAT & (uint32_t)handle->channel) == 0)
        {
            volatile uint32_t *cmdptr = NULL;
            volatile uint32_t *chsema = NULL;

            switch (handle->channel)
            {
                case kDCP_Channel0:
                    cmdptr = &base->CH0CMDPTR;
                    chsema = &base->CH0SEMA;
                    break;

                case kDCP_Channel1:
                    cmdptr = &base->CH1CMDPTR;
                    chsema = &base->CH1SEMA;
                    break;

                case kDCP_Channel2:
                    cmdptr = &base->CH2CMDPTR;
                    chsema = &base->CH2SEMA;
                    break;

                case kDCP_Channel3:
                    cmdptr = &base->CH3CMDPTR;
                    chsema = &base->CH3SEMA;
                    break;

                default:
                    break;
            }

            if (cmdptr && chsema)
            {
                /* set out packet to DCP CMDPTR */
                *cmdptr = (uint32_t)dcpPacket;

                /* Make sure that all data memory accesses are completed before starting of the job */
                __DSB();
                __ISB();

                /* set the channel semaphore to start the job */
                *chsema = 1u;
            }

            status = kStatus_Success;
        }

        else
        {
            status = kStatus_DCP_Again;
        }
        /* global interrupt enable */
        EnableGlobalIRQ(currPriMask);
    }

    else
    {
        return kStatus_DCP_Again;
    }

    return status;
}
#if defined(__GNUC__)
#pragma GCC pop_options
#endif

/*!
 * brief Set AES key to dcp_handle_t struct and optionally to DCP.
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
 * param   base DCP peripheral base address.
 * param   handle Handle used for the request.
 * param   key 0-mod-4 aligned pointer to AES key.
 * param   keySize AES key size in bytes. Shall equal 16.
 * return  status from set key operation
 */
status_t DCP_AES_SetKey(DCP_Type *base, dcp_handle_t *handle, const uint8_t *key, size_t keySize)
{
    status_t status = kStatus_Fail;

    if ((kDCP_OtpKey == handle->keySlot) || (kDCP_OtpUniqueKey == handle->keySlot))
    {
        /* for AES OTP and unique key, check and return read from fuses status */
        if ((base->STAT & DCP_STAT_OTP_KEY_READY_MASK) == DCP_STAT_OTP_KEY_READY_MASK)
        {
            status = kStatus_Success;
        }
    }
    else
    {
        /* only work with aligned key[] */
        if (0x3U & (uintptr_t)key)
        {
            return kStatus_InvalidArgument;
        }

        /* keySize must be 16. */
        if (keySize != 16U)
        {
            return kStatus_InvalidArgument;
        }

        /* move the key by 32-bit words */
        int i = 0;
        while (keySize)
        {
            keySize -= sizeof(uint32_t);
            handle->keyWord[i] = ((uint32_t *)(uintptr_t)key)[i];
            i++;
        }

        if (kDCP_PayloadKey != handle->keySlot)
        {
            /* move the key by 32-bit words to DCP SRAM-based key storage */
            status = dcp_aes_set_sram_based_key(base, handle, key);
        }
        else
        {
            /* for PAYLOAD_KEY, just return Ok status now */
            status = kStatus_Success;
        }
    }

    return status;
}

/*!
 * brief Encrypts AES on one or multiple 128-bit block(s).
 *
 * Encrypts AES.
 * The source plaintext and destination ciphertext can overlap in system memory.
 *
 * param base DCP peripheral base address
 * param handle Handle used for this request.
 * param plaintext Input plain text to encrypt
 * param[out] ciphertext Output cipher text
 * param size Size of input and output data in bytes. Must be multiple of 16 bytes.
 * return Status from encrypt operation
 */
status_t DCP_AES_EncryptEcb(
    DCP_Type *base, dcp_handle_t *handle, const uint8_t *plaintext, uint8_t *ciphertext, size_t size)
{
    status_t completionStatus = kStatus_Fail;
    dcp_work_packet_t dcpWork = {0};

    do
    {
        completionStatus = DCP_AES_EncryptEcbNonBlocking(base, handle, &dcpWork, plaintext, ciphertext, size);
    } while (completionStatus == kStatus_DCP_Again);

    if (completionStatus != kStatus_Success)
    {
        return completionStatus;
    }

    return DCP_WaitForChannelComplete(base, handle);
}

/*!
 * brief Encrypts AES using the ECB block mode.
 *
 * Puts AES ECB encrypt work packet to DCP channel.
 *
 * param base DCP peripheral base address
 * param handle Handle used for this request.
 * param[out] dcpPacket Memory for the DCP work packet.
 * param plaintext Input plain text to encrypt.
 * param[out] ciphertext Output cipher text
 * param size Size of input and output data in bytes. Must be multiple of 16 bytes.
 * return kStatus_Success The work packet has been scheduled at DCP channel.
 * return kStatus_DCP_Again The DCP channel is busy processing previous request.
 */
status_t DCP_AES_EncryptEcbNonBlocking(DCP_Type *base,
                                       dcp_handle_t *handle,
                                       dcp_work_packet_t *dcpPacket,
                                       const uint8_t *plaintext,
                                       uint8_t *ciphertext,
                                       size_t size)
{
    /* Size must be 16-byte multiple */
    if ((size < 16u) || (size % 16u))
    {
        return kStatus_InvalidArgument;
    }

    dcpPacket->control0 =
        0x122u | (handle->swapConfig & 0xFC0000u); /* CIPHER_ENCRYPT | ENABLE_CIPHER | DECR_SEMAPHORE */
    dcpPacket->sourceBufferAddress      = (uint32_t)plaintext;
    dcpPacket->destinationBufferAddress = (uint32_t)ciphertext;
    dcpPacket->bufferSize               = (uint32_t)size;

    if (handle->keySlot == kDCP_OtpKey)
    {
        dcpPacket->control0 |= (1u << 10);  /* OTP_KEY */
        dcpPacket->control1 = (0xFFu << 8); /* KEY_SELECT = OTP_KEY */
    }
    else if (handle->keySlot == kDCP_OtpUniqueKey)
    {
        dcpPacket->control0 |= (1u << 10);  /* OTP_KEY */
        dcpPacket->control1 = (0xFEu << 8); /* KEY_SELECT = UNIQUE_KEY */
    }
    else if (handle->keySlot == kDCP_PayloadKey)
    {
        /* ECB does not have IV, so we can point payload directly to keyWord[] stored in handle. */
        dcpPacket->payloadPointer = (uint32_t)&handle->keyWord[0];
        dcpPacket->control0 |= (1u << 11); /* PAYLOAD_KEY */
    }
    else
    {
        dcpPacket->control1 = (handle->keySlot << 8); /* KEY_SELECT = keySlot */
    }

    return dcp_schedule_work(base, handle, dcpPacket);
}

/*!
 * brief Decrypts AES on one or multiple 128-bit block(s).
 *
 * Decrypts AES.
 * The source ciphertext and destination plaintext can overlap in system memory.
 *
 * param base DCP peripheral base address
 * param handle Handle used for this request.
 * param ciphertext Input plain text to encrypt
 * param[out] plaintext Output cipher text
 * param size Size of input and output data in bytes. Must be multiple of 16 bytes.
 * return Status from decrypt operation
 */
status_t DCP_AES_DecryptEcb(
    DCP_Type *base, dcp_handle_t *handle, const uint8_t *ciphertext, uint8_t *plaintext, size_t size)
{
    status_t completionStatus = kStatus_Fail;
    dcp_work_packet_t dcpWork = {0};

    do
    {
        completionStatus = DCP_AES_DecryptEcbNonBlocking(base, handle, &dcpWork, ciphertext, plaintext, size);
    } while (completionStatus == kStatus_DCP_Again);

    if (completionStatus != kStatus_Success)
    {
        return completionStatus;
    }

    return DCP_WaitForChannelComplete(base, handle);
}

/*!
 * brief Decrypts AES using ECB block mode.
 *
 * Puts AES ECB decrypt dcpPacket to DCP input job ring.
 *
 * param base DCP peripheral base address
 * param handle Handle used for this request.
 * param[out] dcpPacket Memory for the DCP work packet.
 * param ciphertext Input cipher text to decrypt
 * param[out] plaintext Output plain text
 * param size Size of input and output data in bytes. Must be multiple of 16 bytes.
 * return kStatus_Success The work packet has been scheduled at DCP channel.
 * return kStatus_DCP_Again The DCP channel is busy processing previous request.
 */
status_t DCP_AES_DecryptEcbNonBlocking(DCP_Type *base,
                                       dcp_handle_t *handle,
                                       dcp_work_packet_t *dcpPacket,
                                       const uint8_t *ciphertext,
                                       uint8_t *plaintext,
                                       size_t size)
{
    /* Size must be 16-byte multiple */
    if ((size < 16u) || (size % 16u))
    {
        return kStatus_InvalidArgument;
    }

    dcpPacket->control0                 = 0x22u | (handle->swapConfig & 0xFC0000u); /* ENABLE_CIPHER | DECR_SEMAPHORE */
    dcpPacket->sourceBufferAddress      = (uint32_t)ciphertext;
    dcpPacket->destinationBufferAddress = (uint32_t)plaintext;
    dcpPacket->bufferSize               = (uint32_t)size;

    if (handle->keySlot == kDCP_OtpKey)
    {
        dcpPacket->control0 |= (1u << 10);  /* OTP_KEY */
        dcpPacket->control1 = (0xFFu << 8); /* KEY_SELECT = OTP_KEY */
    }
    else if (handle->keySlot == kDCP_OtpUniqueKey)
    {
        dcpPacket->control0 |= (1u << 10);  /* OTP_KEY */
        dcpPacket->control1 = (0xFEu << 8); /* KEY_SELECT = UNIQUE_KEY */
    }
    else if (handle->keySlot == kDCP_PayloadKey)
    {
        /* ECB does not have IV, so we can point payload directly to keyWord[] stored in handle. */
        dcpPacket->payloadPointer = (uint32_t)&handle->keyWord[0];
        dcpPacket->control0 |= (1u << 11); /* PAYLOAD_KEY */
    }
    else
    {
        dcpPacket->control1 = (handle->keySlot << 8); /* KEY_SELECT = keySlot */
    }

    return dcp_schedule_work(base, handle, dcpPacket);
}

/*!
 * brief Encrypts AES using CBC block mode.
 *
 * Encrypts AES using CBC block mode.
 * The source plaintext and destination ciphertext can overlap in system memory.
 *
 * param base DCP peripheral base address
 * param handle Handle used for this request.
 * param plaintext Input plain text to encrypt
 * param[out] ciphertext Output cipher text
 * param size Size of input and output data in bytes. Must be multiple of 16 bytes.
 * param iv Input initial vector to combine with the first input block.
 * return Status from encrypt operation
 */
status_t DCP_AES_EncryptCbc(DCP_Type *base,
                            dcp_handle_t *handle,
                            const uint8_t *plaintext,
                            uint8_t *ciphertext,
                            size_t size,
                            const uint8_t iv[16])
{
    status_t completionStatus = kStatus_Fail;
    dcp_work_packet_t dcpWork = {0};

    do
    {
        completionStatus = DCP_AES_EncryptCbcNonBlocking(base, handle, &dcpWork, plaintext, ciphertext, size, iv);
    } while (completionStatus == kStatus_DCP_Again);

    if (completionStatus != kStatus_Success)
    {
        return completionStatus;
    }

    return DCP_WaitForChannelComplete(base, handle);
}

/*!
 * brief Encrypts AES using CBC block mode.
 *
 * Puts AES CBC encrypt dcpPacket to DCP input job ring.
 *
 * param base DCP peripheral base address
 * param handle Handle used for this request. Specifies jobRing.
 * param[out] dcpPacket Memory for the DCP work packet.
 * param plaintext Input plain text to encrypt
 * param[out] ciphertext Output cipher text
 * param size Size of input and output data in bytes. Must be multiple of 16 bytes.
 * param iv Input initial vector to combine with the first input block.
 * return kStatus_Success The work packet has been scheduled at DCP channel.
 * return kStatus_DCP_Again The DCP channel is busy processing previous request.
 */
status_t DCP_AES_EncryptCbcNonBlocking(DCP_Type *base,
                                       dcp_handle_t *handle,
                                       dcp_work_packet_t *dcpPacket,
                                       const uint8_t *plaintext,
                                       uint8_t *ciphertext,
                                       size_t size,
                                       const uint8_t *iv)
{
    /* Size must be 16-byte multiple */
    if ((size < 16u) || (size % 16u))
    {
        return kStatus_InvalidArgument;
    }

    dcpPacket->control0 =
        0x322u | (handle->swapConfig & 0xFC0000u); /* CIPHER_INIT | CIPHER_ENCRYPT | ENABLE_CIPHER | DECR_SEMAPHORE */
    dcpPacket->control1                 = 0x10u;   /* CBC */
    dcpPacket->sourceBufferAddress      = (uint32_t)plaintext;
    dcpPacket->destinationBufferAddress = (uint32_t)ciphertext;
    dcpPacket->bufferSize               = (uint32_t)size;

    if (handle->keySlot == kDCP_OtpKey)
    {
        dcpPacket->payloadPointer = (uint32_t)iv;
        dcpPacket->control0 |= (1u << 10);   /* OTP_KEY */
        dcpPacket->control1 |= (0xFFu << 8); /* KEY_SELECT = OTP_KEY */
    }
    else if (handle->keySlot == kDCP_OtpUniqueKey)
    {
        dcpPacket->payloadPointer = (uint32_t)iv;
        dcpPacket->control0 |= (1u << 10);   /* OTP_KEY */
        dcpPacket->control1 |= (0xFEu << 8); /* KEY_SELECT = UNIQUE_KEY */
    }
    else if (handle->keySlot == kDCP_PayloadKey)
    {
        /* In this case payload must contain key & iv in one array. */
        /* Copy iv into handle right behind the keyWord[] so we can point payload to keyWord[]. */
        dcp_memcpy(handle->iv, iv, 16);
        dcpPacket->payloadPointer = (uint32_t)&handle->keyWord[0];
        dcpPacket->control0 |= (1u << 11); /* PAYLOAD_KEY */
    }
    else
    {
        dcpPacket->payloadPointer = (uint32_t)iv;
        dcpPacket->control1 |= ((uint32_t)handle->keySlot << 8); /* KEY_SELECT = keySlot */
    }

    return dcp_schedule_work(base, handle, dcpPacket);
}

/*!
 * brief Decrypts AES using CBC block mode.
 *
 * Decrypts AES using CBC block mode.
 * The source ciphertext and destination plaintext can overlap in system memory.
 *
 * param base DCP peripheral base address
 * param handle Handle used for this request.
 * param ciphertext Input cipher text to decrypt
 * param[out] plaintext Output plain text
 * param size Size of input and output data in bytes. Must be multiple of 16 bytes.
 * param iv Input initial vector to combine with the first input block.
 * return Status from decrypt operation
 */
status_t DCP_AES_DecryptCbc(DCP_Type *base,
                            dcp_handle_t *handle,
                            const uint8_t *ciphertext,
                            uint8_t *plaintext,
                            size_t size,
                            const uint8_t iv[16])
{
    status_t completionStatus = kStatus_Fail;
    dcp_work_packet_t dcpWork = {0};

    do
    {
        completionStatus = DCP_AES_DecryptCbcNonBlocking(base, handle, &dcpWork, ciphertext, plaintext, size, iv);
    } while (completionStatus == kStatus_DCP_Again);

    if (completionStatus != kStatus_Success)
    {
        return completionStatus;
    }

    return DCP_WaitForChannelComplete(base, handle);
}

/*!
 * brief Decrypts AES using CBC block mode.
 *
 * Puts AES CBC decrypt dcpPacket to DCP input job ring.
 *
 * param base DCP peripheral base address
 * param handle Handle used for this request. Specifies jobRing.
 * param[out] dcpPacket Memory for the DCP work packet.
 * param ciphertext Input cipher text to decrypt
 * param[out] plaintext Output plain text
 * param size Size of input and output data in bytes. Must be multiple of 16 bytes.
 * param iv Input initial vector to combine with the first input block.
 * return kStatus_Success The work packet has been scheduled at DCP channel.
 * return kStatus_DCP_Again The DCP channel is busy processing previous request.
 */
status_t DCP_AES_DecryptCbcNonBlocking(DCP_Type *base,
                                       dcp_handle_t *handle,
                                       dcp_work_packet_t *dcpPacket,
                                       const uint8_t *ciphertext,
                                       uint8_t *plaintext,
                                       size_t size,
                                       const uint8_t *iv)
{
    /* Size must be 16-byte multiple */
    if ((size < 16u) || (size % 16u))
    {
        return kStatus_InvalidArgument;
    }

    dcpPacket->control0 = 0x222u | (handle->swapConfig & 0xFC0000u); /* CIPHER_INIT | ENABLE_CIPHER | DECR_SEMAPHORE */
    dcpPacket->control1 = 0x10u;                                     /* CBC */
    dcpPacket->sourceBufferAddress      = (uint32_t)ciphertext;
    dcpPacket->destinationBufferAddress = (uint32_t)plaintext;
    dcpPacket->bufferSize               = (uint32_t)size;

    if (handle->keySlot == kDCP_OtpKey)
    {
        dcpPacket->payloadPointer = (uint32_t)iv;
        dcpPacket->control0 |= (1u << 10);   /* OTP_KEY */
        dcpPacket->control1 |= (0xFFu << 8); /* OTP_KEY */
    }
    else if (handle->keySlot == kDCP_OtpUniqueKey)
    {
        dcpPacket->payloadPointer = (uint32_t)iv;
        dcpPacket->control0 |= (1u << 10);   /* OTP_KEY */
        dcpPacket->control1 |= (0xFEu << 8); /* UNIQUE_KEY */
    }
    else if (handle->keySlot == kDCP_PayloadKey)
    {
        /* in this case payload must contain KEY + IV together */
        /* copy iv into handle struct so we can point payload directly to keyWord[]. */
        dcp_memcpy(handle->iv, iv, 16);
        dcpPacket->payloadPointer = (uint32_t)&handle->keyWord[0];
        dcpPacket->control0 |= (1u << 11); /* PAYLOAD_KEY */
    }
    else
    {
        dcpPacket->payloadPointer = (uint32_t)iv;
        dcpPacket->control1 |= ((uint32_t)handle->keySlot << 8); /* KEY_SELECT */
    }

    return dcp_schedule_work(base, handle, dcpPacket);
}

/*!
 * brief Gets the default configuration structure.
 *
 * This function initializes the DCP configuration structure to a default value. The default
 * values are as follows.
 *   dcpConfig->gatherResidualWrites = true;
 *   dcpConfig->enableContextCaching = true;
 *   dcpConfig->enableContextSwitching = true;
 *   dcpConfig->enableChannnel = kDCP_chEnableAll;
 *   dcpConfig->enableChannelInterrupt = kDCP_chIntDisable;
 *
 * param[out] config Pointer to configuration structure.
 */
void DCP_GetDefaultConfig(dcp_config_t *config)
{
    /* ENABLE_CONTEXT_CACHING is disabled by default as the DCP Hash driver uses
     * dcp_hash_save_running_hash() and dcp_hash_restore_running_hash() to support
     * Hash context switch (different messages interleaved) on the same channel.
     */

    /* Initializes the configure structure to zero. */
    memset(config, 0, sizeof(*config));

    dcp_config_t userConfig = {
        true, false, true, kDCP_chEnableAll, kDCP_chIntDisable,
    };

    *config = userConfig;
}

/*!
 * brief   Enables clock to and enables DCP
 *
 * Enable DCP clock and configure DCP.
 *
 * param base DCP base address
 * param config Pointer to configuration structure.
 */
void DCP_Init(DCP_Type *base, const dcp_config_t *config)
{
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    CLOCK_EnableClock(kCLOCK_Dcp);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

    base->CTRL = 0xF0800000u; /* reset value */
    base->CTRL = 0x30800000u; /* default value */

    dcp_clear_status(base);
    dcp_clear_channel_status(base, kDCP_Channel0 | kDCP_Channel1 | kDCP_Channel2 | kDCP_Channel3);

    base->CTRL = DCP_CTRL_GATHER_RESIDUAL_WRITES(config->gatherResidualWrites) |
                 DCP_CTRL_ENABLE_CONTEXT_CACHING(config->enableContextCaching) |
                 DCP_CTRL_ENABLE_CONTEXT_SWITCHING(config->enableContextSwitching) |
                 DCP_CTRL_CHANNEL_INTERRUPT_ENABLE(config->enableChannelInterrupt);

    /* enable DCP channels */
    base->CHANNELCTRL = DCP_CHANNELCTRL_ENABLE_CHANNEL(config->enableChannel);

    /* use context switching buffer */
    base->CONTEXT = (uint32_t)&s_dcpContextSwitchingBuffer;
}

/*!
 * brief   Disable DCP clock
 *
 * Reset DCP and Disable DCP clock.
 *
 * param base DCP base address
 */
void DCP_Deinit(DCP_Type *base)
{
    base->CTRL = 0xF0800000u; /* reset value */
    memset(&s_dcpContextSwitchingBuffer, 0, sizeof(s_dcpContextSwitchingBuffer));

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    CLOCK_DisableClock(kCLOCK_Dcp);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

/*!
 * brief Poll and wait on DCP channel.
 *
 * Polls the specified DCP channel until current it completes activity.
 *
 * param   base DCP peripheral base address.
 * param   handle Specifies DCP channel.
 * return  kStatus_Success When data processing completes without error.
 * return  kStatus_Fail When error occurs.
 */
status_t DCP_WaitForChannelComplete(DCP_Type *base, dcp_handle_t *handle)
{
    /* wait if our channel is still active */
    while ((base->STAT & (uint32_t)handle->channel) == handle->channel)
    {
    }

    if (dcp_get_channel_status(base, handle->channel) != kStatus_Success)
    {
        dcp_clear_status(base);
        dcp_clear_channel_status(base, handle->channel);
        return kStatus_Fail;
    }

    dcp_clear_status(base);
    return kStatus_Success;
}

/*!
 * @brief Check validity of algoritm.
 *
 * This function checks the validity of input argument.
 *
 * @param algo Tested algorithm value.
 * @return kStatus_Success if valid, kStatus_InvalidArgument otherwise.
 */
static status_t dcp_hash_check_input_alg(dcp_hash_algo_t algo)
{
    if ((algo != kDCP_Sha256) && (algo != kDCP_Sha1) && (algo != kDCP_Crc32))
    {
        return kStatus_InvalidArgument;
    }
    return kStatus_Success;
}

/*!
 * @brief Check validity of input arguments.
 *
 * This function checks the validity of input arguments.
 *
 * @param base DCP peripheral base address.
 * @param ctx Memory buffer given by user application where the DCP_HASH_Init/DCP_HASH_Update/DCP_HASH_Finish store
 * context.
 * @param algo Tested algorithm value.
 * @return kStatus_Success if valid, kStatus_InvalidArgument otherwise.
 */
static status_t dcp_hash_check_input_args(DCP_Type *base, dcp_hash_ctx_t *ctx, dcp_hash_algo_t algo)
{
    /* Check validity of input algorithm */
    if (kStatus_Success != dcp_hash_check_input_alg(algo))
    {
        return kStatus_InvalidArgument;
    }

    if ((NULL == ctx) || (NULL == base))
    {
        return kStatus_InvalidArgument;
    }

    return kStatus_Success;
}

/*!
 * @brief Check validity of internal software context.
 *
 * This function checks if the internal context structure looks correct.
 *
 * @param ctxInternal Internal context.
 * @param message Input message address.
 * @return kStatus_Success if valid, kStatus_InvalidArgument otherwise.
 */
static status_t dcp_hash_check_context(dcp_hash_ctx_internal_t *ctxInternal, const uint8_t *message)
{
    if ((NULL == message) || (NULL == ctxInternal) || (kStatus_Success != dcp_hash_check_input_alg(ctxInternal->algo)))
    {
        return kStatus_InvalidArgument;
    }
    return kStatus_Success;
}

/*!
 * @brief Initialize the SHA engine for new hash.
 *
 * This function sets kDCP_CONTROL0_HASH_INIT for control0 in work packet to start a new hash.
 *
 * @param base SHA peripheral base address.
 * @param ctxInternal Internal context.
 */
static status_t dcp_hash_engine_init(DCP_Type *base, dcp_hash_ctx_internal_t *ctxInternal)
{
    status_t status;

    status = kStatus_InvalidArgument;

    if ((kDCP_Sha256 == ctxInternal->algo) || (kDCP_Sha1 == ctxInternal->algo) || (kDCP_Crc32 == ctxInternal->algo))
    {
        ctxInternal->ctrl0 = kDCP_CONTROL0_HASH_INIT;
        status             = kStatus_Success;
    }

    return status;
}

static status_t dcp_hash_update_non_blocking(
    DCP_Type *base, dcp_hash_ctx_internal_t *ctxInternal, dcp_work_packet_t *dcpPacket, const uint8_t *msg, size_t size)
{
    dcpPacket->control0 = ctxInternal->ctrl0 | (ctxInternal->handle->swapConfig & 0xFC0000u) |
                          kDCP_CONTROL0_ENABLE_HASH | kDCP_CONTROL0_DECR_SEMAPHOR;
    if (ctxInternal->algo == kDCP_Sha256)
    {
        dcpPacket->control1 = kDCP_CONTROL1_HASH_SELECT_SHA256;
    }
    else if (ctxInternal->algo == kDCP_Sha1)
    {
        dcpPacket->control1 = kDCP_CONTROL1_HASH_SELECT_SHA1;
    }
    else if (ctxInternal->algo == kDCP_Crc32)
    {
        dcpPacket->control1 = kDCP_CONTROL1_HASH_SELECT_CRC32;
    }
    else
    {
        return kStatus_Fail;
    }
    dcpPacket->sourceBufferAddress      = (uint32_t)msg;
    dcpPacket->destinationBufferAddress = 0;
    dcpPacket->bufferSize               = size;
    dcpPacket->payloadPointer           = (uint32_t)ctxInternal->runningHash;

    return dcp_schedule_work(base, ctxInternal->handle, dcpPacket);
}

static status_t dcp_hash_update(DCP_Type *base, dcp_hash_ctx_internal_t *ctxInternal, const uint8_t *msg, size_t size)
{
    status_t completionStatus = kStatus_Fail;
    dcp_work_packet_t dcpWork = {0};

    do
    {
        completionStatus = dcp_hash_update_non_blocking(base, ctxInternal, &dcpWork, msg, size);
    } while (completionStatus == kStatus_DCP_Again);

    completionStatus = DCP_WaitForChannelComplete(base, ctxInternal->handle);

    ctxInternal->ctrl0 = 0; /* clear kDCP_CONTROL0_HASH_INIT and kDCP_CONTROL0_HASH_TERM flags */
    return (completionStatus);
}

/*!
 * @brief Adds message to current hash.
 *
 * This function merges the message to fill the internal buffer, empties the internal buffer if
 * it becomes full, then process all remaining message data.
 *
 *
 * @param base DCP peripheral base address.
 * @param ctxInternal Internal context.
 * @param message Input message.
 * @param messageSize Size of input message in bytes.
 * @return kStatus_Success.
 */
static status_t dcp_hash_process_message_data(DCP_Type *base,
                                              dcp_hash_ctx_internal_t *ctxInternal,
                                              const uint8_t *message,
                                              size_t messageSize)
{
    status_t status = kStatus_Fail;

    /* if there is partially filled internal buffer, fill it to full block */
    if (ctxInternal->blksz > 0)
    {
        size_t toCopy = DCP_HASH_BLOCK_SIZE - ctxInternal->blksz;
        dcp_memcpy(&ctxInternal->blk.b[ctxInternal->blksz], message, toCopy);
        message += toCopy;
        messageSize -= toCopy;

        /* process full internal block */
        status = dcp_hash_update(base, ctxInternal, &ctxInternal->blk.b[0], DCP_HASH_BLOCK_SIZE);
        if (kStatus_Success != status)
        {
            return status;
        }
    }

    /* process all full blocks in message[] */
    uint32_t fullBlocksSize = ((messageSize >> 6) << 6); /* (X / 64) * 64 */
    if (fullBlocksSize > 0)
    {
        status = dcp_hash_update(base, ctxInternal, message, fullBlocksSize);
        if (kStatus_Success != status)
        {
            return status;
        }
        message += fullBlocksSize;
        messageSize -= fullBlocksSize;
    }

    /* copy last incomplete message bytes into internal block */
    dcp_memcpy(&ctxInternal->blk.b[0], message, messageSize);
    ctxInternal->blksz = messageSize;

    return status;
}

/*!
 * @brief Finalize the running hash to make digest.
 *
 * This function empties the internal buffer, adds padding bits, and generates final digest.
 *
 * @param base SHA peripheral base address.
 * @param ctxInternal Internal context.
 * @return kStatus_Success.
 */
static status_t dcp_hash_finalize(DCP_Type *base, dcp_hash_ctx_internal_t *ctxInternal)
{
    status_t status;

    ctxInternal->ctrl0 |= kDCP_CONTROL0_HASH_TERM;
    status = dcp_hash_update(base, ctxInternal, &ctxInternal->blk.b[0], ctxInternal->blksz);

    return status;
}

static void dcp_hash_save_running_hash(dcp_hash_ctx_internal_t *ctxInternal)
{
    uint32_t *srcAddr = NULL;

    switch (ctxInternal->handle->channel)
    {
        case kDCP_Channel0:
            srcAddr = &s_dcpContextSwitchingBuffer.x[43];
            break;

        case kDCP_Channel1:
            srcAddr = &s_dcpContextSwitchingBuffer.x[30];
            break;

        case kDCP_Channel2:
            srcAddr = &s_dcpContextSwitchingBuffer.x[17];
            break;

        case kDCP_Channel3:
            srcAddr = &s_dcpContextSwitchingBuffer.x[4];
            break;

        default:
            break;
    }
    if (srcAddr)
    {
        dcp_memcpy(ctxInternal->runningHash, srcAddr, sizeof(ctxInternal->runningHash));
    }
}

static void dcp_hash_restore_running_hash(dcp_hash_ctx_internal_t *ctxInternal)
{
    uint32_t *destAddr = NULL;

    switch (ctxInternal->handle->channel)
    {
        case kDCP_Channel0:
            destAddr = &s_dcpContextSwitchingBuffer.x[43];
            break;

        case kDCP_Channel1:
            destAddr = &s_dcpContextSwitchingBuffer.x[30];
            break;

        case kDCP_Channel2:
            destAddr = &s_dcpContextSwitchingBuffer.x[17];
            break;

        case kDCP_Channel3:
            destAddr = &s_dcpContextSwitchingBuffer.x[4];
            break;

        default:
            break;
    }
    if (destAddr)
    {
        dcp_memcpy(destAddr, ctxInternal->runningHash, sizeof(ctxInternal->runningHash));
    }
}

/*!
 * brief Initialize HASH context
 *
 * This function initializes the HASH.
 *
 * param base DCP peripheral base address
 * param handle Specifies the DCP channel used for hashing.
 * param[out] ctx Output hash context
 * param algo Underlaying algorithm to use for hash computation.
 * return Status of initialization
 */
status_t DCP_HASH_Init(DCP_Type *base, dcp_handle_t *handle, dcp_hash_ctx_t *ctx, dcp_hash_algo_t algo)
{
    status_t status;

    dcp_hash_ctx_internal_t *ctxInternal;
    /* compile time check for the correct structure size */
    BUILD_ASSURE(sizeof(dcp_hash_ctx_t) >= sizeof(dcp_hash_ctx_internal_t), dcp_hash_ctx_t_size);
    uint32_t i;

    status = dcp_hash_check_input_args(base, ctx, algo);
    if (status != kStatus_Success)
    {
        return status;
    }

    /* set algorithm in context struct for later use */
    ctxInternal        = (dcp_hash_ctx_internal_t *)ctx;
    ctxInternal->algo  = algo;
    ctxInternal->blksz = 0u;
    for (i = 0; i < sizeof(ctxInternal->blk.w) / sizeof(ctxInternal->blk.w[0]); i++)
    {
        ctxInternal->blk.w[0] = 0u;
    }
    ctxInternal->state           = kDCP_StateHashInit;
    ctxInternal->fullMessageSize = 0;
    ctxInternal->handle          = handle;
    return status;
}

/*!
 * brief Add data to current HASH
 *
 * Add data to current HASH. This can be called repeatedly with an arbitrary amount of data to be
 * hashed. The functions blocks. If it returns kStatus_Success, the running hash
 * has been updated (DCP has processed the input data), so the memory at ref input pointer
 * can be released back to system. The DCP context buffer is updated with the running hash
 * and with all necessary information to support possible context switch.
 *
 * param base DCP peripheral base address
 * param[in,out] ctx HASH context
 * param input Input data
 * param inputSize Size of input data in bytes
 * return Status of the hash update operation
 */
status_t DCP_HASH_Update(DCP_Type *base, dcp_hash_ctx_t *ctx, const uint8_t *input, size_t inputSize)
{
    bool isUpdateState;
    status_t status;
    dcp_hash_ctx_internal_t *ctxInternal;
    size_t blockSize;

    if (inputSize == 0)
    {
        return kStatus_Success;
    }

    ctxInternal = (dcp_hash_ctx_internal_t *)ctx;
    status      = dcp_hash_check_context(ctxInternal, input);
    if (kStatus_Success != status)
    {
        return status;
    }

    ctxInternal->fullMessageSize += inputSize;
    blockSize = DCP_HASH_BLOCK_SIZE;
    /* if we are still less than DCP_HASH_BLOCK_SIZE bytes, keep only in context */
    if ((ctxInternal->blksz + inputSize) <= blockSize)
    {
        dcp_memcpy((&ctxInternal->blk.b[0]) + ctxInternal->blksz, input, inputSize);
        ctxInternal->blksz += inputSize;
        return status;
    }
    else
    {
        isUpdateState = ctxInternal->state == kDCP_StateHashUpdate;
        if (!isUpdateState)
        {
            /* start NEW hash */
            status = dcp_hash_engine_init(base, ctxInternal);
            if (status != kStatus_Success)
            {
                return status;
            }
            ctxInternal->state = kDCP_StateHashUpdate;
        }
        else
        {
            dcp_hash_restore_running_hash(ctxInternal);
        }
    }

    /* process input data */
    status = dcp_hash_process_message_data(base, ctxInternal, input, inputSize);
    dcp_hash_save_running_hash(ctxInternal);
    return status;
}

/*!
 * brief Finalize hashing
 *
 * Outputs the final hash (computed by DCP_HASH_Update()) and erases the context.
 *
 * param[in,out] ctx Input hash context
 * param[out] output Output hash data
 * param[in,out] outputSize Optional parameter (can be passed as NULL). On function entry, it specifies the size of
 * output[] buffer. On function return, it stores the number of updated output bytes.
 * return Status of the hash finish operation
 */
status_t DCP_HASH_Finish(DCP_Type *base, dcp_hash_ctx_t *ctx, uint8_t *output, size_t *outputSize)
{
    size_t algOutSize = 0;
    status_t status;
    dcp_hash_ctx_internal_t *ctxInternal;

    ctxInternal = (dcp_hash_ctx_internal_t *)ctx;
    status      = dcp_hash_check_context(ctxInternal, output);
    if (kStatus_Success != status)
    {
        return status;
    }

    if (ctxInternal->state == kDCP_StateHashInit)
    {
        status = dcp_hash_engine_init(base, ctxInternal);
        if (status != kStatus_Success)
        {
            return status;
        }
    }
    else
    {
        dcp_hash_restore_running_hash(ctxInternal);
    }

    size_t outSize = 0u;

    /* compute algorithm output length */
    switch (ctxInternal->algo)
    {
        case kDCP_Sha256:
            outSize = kDCP_OutLenSha256;
            break;
        case kDCP_Sha1:
            outSize = kDCP_OutLenSha1;
            break;
        case kDCP_Crc32:
            outSize = kDCP_OutLenCrc32;
            break;
        default:
            break;
    }
    algOutSize = outSize;

#if defined(DCP_HASH_CAVP_COMPATIBLE)
    if (ctxInternal->fullMessageSize == 0)
    {
        switch (ctxInternal->algo)
        {
            case kDCP_Sha256:
                dcp_memcpy(&output[0], &s_nullSha256, 32);
                break;
            case kDCP_Sha1:
                dcp_memcpy(&output[0], &s_nullSha1, 20);
                break;
            default:
                break;
        }

        return kStatus_Success;
    }
#endif /* DCP_HASH_CAVP_COMPATIBLE */

    /* flush message last incomplete block, if there is any, and add padding bits */
    status = dcp_hash_finalize(base, ctxInternal);

    if (outputSize)
    {
        if (algOutSize < *outputSize)
        {
            *outputSize = algOutSize;
        }
        else
        {
            algOutSize = *outputSize;
        }
    }

    /* Reverse and copy result to output[] */
    dcp_reverse_and_copy((uint8_t *)ctxInternal->runningHash, &output[0], algOutSize);

    memset(ctx, 0, sizeof(dcp_hash_ctx_t));
    return status;
}

/*!
 * brief Create HASH on given data
 *
 * Perform the full SHA or CRC32 in one function call. The function is blocking.
 *
 * param base DCP peripheral base address
 * param handle Handle used for the request.
 * param algo Underlaying algorithm to use for hash computation.
 * param input Input data
 * param inputSize Size of input data in bytes
 * param[out] output Output hash data
 * param[out] outputSize Output parameter storing the size of the output hash in bytes
 * return Status of the one call hash operation.
 */
status_t DCP_HASH(DCP_Type *base,
                  dcp_handle_t *handle,
                  dcp_hash_algo_t algo,
                  const uint8_t *input,
                  size_t inputSize,
                  uint8_t *output,
                  size_t *outputSize)
{
    dcp_hash_ctx_t hashCtx;
    status_t status;

    status = DCP_HASH_Init(base, handle, &hashCtx, algo);
    if (status != kStatus_Success)
    {
        return status;
    }

    status = DCP_HASH_Update(base, &hashCtx, input, inputSize);
    if (status != kStatus_Success)
    {
        return status;
    }

    status = DCP_HASH_Finish(base, &hashCtx, output, outputSize);

    return status;
}
