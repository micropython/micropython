/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#ifndef __HAL_CRYPTO_H__
#define __HAL_CRYPTO_H__


#include "hal_api.h"
#include "basic_types.h"


#define CRYPTO_MAX_MSG_LENGTH		16000
#define CRYPTO_MD5_DIGEST_LENGTH 	16
#define CRYPTO_SHA1_DIGEST_LENGTH 	20
#define CRYPTO_SHA2_DIGEST_LENGTH 	32


typedef enum _SHA2_TYPE_ {
		SHA2_NONE 	= 0,
		SHA2_224 	= 224/8,
		SHA2_256 	= 256/8,
		SHA2_384 	= 384/8,
		SHA2_512 	= 512/8
} SHA2_TYPE;


#define _ERRNO_CRYPTO_DESC_NUM_SET_OutRange 	-2
#define _ERRNO_CRYPTO_BURST_NUM_SET_OutRange	-3
#define _ERRNO_CRYPTO_NULL_POINTER				-4
#define _ERRNO_CRYPTO_ENGINE_NOT_INIT			-5
#define _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned	-6
#define _ERRNO_CRYPTO_KEY_OutRange				-7
#define _ERRNO_CRYPTO_MSG_OutRange				-8
#define _ERRNO_CRYPTO_IV_OutRange				-9
#define _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH		-10
#define _ERRNO_CRYPTO_CIPHER_TYPE_NOT_MATCH 	-11
#define _ERRNO_CRYPTO_KEY_IV_LEN_DIFF			-12



//
//	External API Functions
//


// Crypto Engine
extern int rtl_cryptoEngine_init(void);
extern void rtl_cryptoEngine_info(void);



//
// Authentication 
//

// md5

extern int rtl_crypto_md5(IN const u8* message, IN const u32 msglen, OUT u8* pDigest);

extern int rtl_crypto_md5_init(void);
extern int rtl_crypto_md5_process(IN const u8* message, const IN u32 msglen, OUT u8* pDigest);


// sha1
extern int rtl_crypto_sha1(IN const u8* message, IN const u32 msglen, OUT u8* pDigest);

extern int rtl_crypto_sha1_init(void);
extern int rtl_crypto_sha1_process(IN const u8* message, IN const u32 msglen, OUT u8* pDigest);

// sha2

extern int rtl_crypto_sha2(IN const SHA2_TYPE sha2type, 
								IN const u8* message, IN const u32 msglen, OUT u8* pDigest);

extern int rtl_crypto_sha2_init(IN const SHA2_TYPE sha2type);
extern int rtl_crypto_sha2_process(IN const u8* message, IN const u32 msglen, OUT u8* pDigest);


// HMAC-md5
extern int rtl_crypto_hmac_md5(IN const u8* message, IN const u32 msglen, 
									IN const u8* key, IN const u32 keylen, OUT u8* pDigest);

extern int rtl_crypto_hmac_md5_init(IN const u8* key, IN const u32 keylen);
extern int rtl_crypto_hmac_md5_process(IN const u8* message, IN const u32 msglen, OUT u8* pDigest);


// HMAC-sha1
extern int rtl_crypto_hmac_sha1(IN const u8* message, IN const u32 msglen, 
									IN const u8* key, IN const u32 keylen, OUT u8* pDigest);

extern int rtl_crypto_hmac_sha1_init(IN const u8* key, IN const u32 keylen);
extern int rtl_crypto_hmac_sha1_process(IN const u8* message, IN const u32 msglen, OUT u8* pDigest);


// HMAC-sha2
extern int rtl_crypto_hmac_sha2(IN const SHA2_TYPE sha2type, IN const u8* message, IN const u32 msglen, 
									IN const u8* key, IN const u32 keylen, OUT u8* pDigest);

extern int rtl_crypto_hmac_sha2_init(IN const SHA2_TYPE sha2type, IN const u8* key, IN const u32 keylen);
extern int rtl_crypto_hmac_sha2_process(IN const u8* message, IN const u32 msglen, OUT u8* pDigest);


//
// Cipher Functions
//

// AES - CBC

extern int rtl_crypto_aes_cbc_init(IN const u8* key, IN const u32 keylen);

extern int rtl_crypto_aes_cbc_encrypt(
	IN const u8* message, IN const u32 msglen, 
	IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);

extern int rtl_crypto_aes_cbc_decrypt(
	IN const u8* message, IN const u32 msglen, 
	IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);


// AES - ECB

extern int rtl_crypto_aes_ecb_init(IN const u8* key, IN const u32 keylen);

extern int rtl_crypto_aes_ecb_encrypt(
	IN const u8* message, IN const u32 msglen, 
	IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);

extern int rtl_crypto_aes_ecb_decrypt(
	IN const u8* message, IN const u32 msglen, 
	IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);


// AES - CTR

extern int rtl_crypto_aes_ctr_init(IN const u8* key, IN const u32 keylen);

extern int rtl_crypto_aes_ctr_encrypt(
	IN const u8* message, IN const u32 msglen, 
	IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);

extern int rtl_crypto_aes_ctr_decrypt(
	IN const u8* message, IN const u32 msglen, 
	IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);


// 3DES - CBC

extern int rtl_crypto_3des_cbc_init(IN const u8* key, IN const u32 keylen);

extern int rtl_crypto_3des_cbc_encrypt(
	IN const u8* message, IN const u32 msglen, 
	IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);

extern int rtl_crypto_3des_cbc_decrypt(
	IN const u8* message, IN const u32 msglen, 
	IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);


// 3DES - ECB

extern int rtl_crypto_3des_ecb_init(IN const u8* key, IN const u32 keylen);

extern int rtl_crypto_3des_ecb_encrypt(
	IN const u8* message, IN const u32 msglen, 
	IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);

extern int rtl_crypto_3des_ecb_decrypt(
	IN const u8* message, IN const u32 msglen, 
	IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);


// DES - CBC

extern int rtl_crypto_des_cbc_init(IN const u8* key, IN const u32 keylen);

extern int rtl_crypto_des_cbc_encrypt(
	IN const u8* message, IN const u32 msglen, 
	IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);

extern int rtl_crypto_des_cbc_decrypt(
	IN const u8* message, IN const u32 msglen, 
	IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);


// DES - ECB

extern int rtl_crypto_des_ecb_init(IN const u8* key, IN const u32 keylen);

extern int rtl_crypto_des_ecb_encrypt(
	IN const u8* message, IN const u32 msglen, 
	IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);

extern int rtl_crypto_des_ecb_decrypt(
	IN const u8* message, IN const u32 msglen, 
	IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);


//
// C functions in ROM
//

extern int rtl_memcmpb(const u8 *dst, const u8 *src, int bytes);
extern int rtl_memcpyb(u8 *dst, const u8 *src, int bytes);

#endif /* __HAL_CRYPTO_H__ */

