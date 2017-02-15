#ifndef ROM_SSL_RAM_MAP_H
#define ROM_SSL_RAM_MAP_H

#include "basic_types.h"

struct _rom_ssl_ram_map {
	/* OS interface */
	void *(*ssl_malloc)(unsigned int sz);
	void (*ssl_free)(void *);
	int (*ssl_printf)(const char *, ...);

	//AES HW CRYPTO
	int (*hw_crypto_aes_ecb_init)(const u8* key, const u32 keylen);
	int (*hw_crypto_aes_ecb_decrypt)(
		const u8* message, 	const u32 msglen, 
		const u8* iv, 		const u32 ivlen, 
		u8* pResult);
	int (*hw_crypto_aes_ecb_encrypt)(
		const u8* message, 	const u32 msglen, 
		const u8* iv, 		const u32 ivlen, 
		u8* pResult);
	int (*hw_crypto_aes_cbc_init)(const u8* key, 	const u32 keylen);
	int (*hw_crypto_aes_cbc_decrypt)(
		const u8* message, 	const u32 msglen, 
		const u8* iv, 		const u32 ivlen, 
		u8* pResult);
	int (*hw_crypto_aes_cbc_encrypt)(
		const u8* message, 	const u32 msglen, 
		const u8* iv, 		const u32 ivlen, 
		u8* pResult);

	//DES HW CRYPTO
	int (*hw_crypto_des_cbc_init)(const u8* key, const u32 keylen);
	int (*hw_crypto_des_cbc_decrypt)(
		const u8* message, 	const u32 msglen, 
		const u8* iv, 		const u32 ivlen, 
		u8* pResult);
	int (*hw_crypto_des_cbc_encrypt)(
		const u8* message, 	const u32 msglen, 
		const u8* iv, 		const u32 ivlen, 
		u8* pResult);
	int (*hw_crypto_3des_cbc_init)(const u8* key, const u32 keylen);
	int (*hw_crypto_3des_cbc_decrypt)(
		const u8* message, 	const u32 msglen, 
		const u8* iv, 		const u32 ivlen, 
		u8* pResult);
	int (*hw_crypto_3des_cbc_encrypt)(
		const u8* message, 	const u32 msglen, 
		const u8* iv, 		const u32 ivlen, 
		u8* pResult);

	/* Variables */
	u32 use_hw_crypto_func;
};

extern struct _rom_ssl_ram_map rom_ssl_ram_map;

#endif	/* ROM_SSL_RAM_MAP_H */
