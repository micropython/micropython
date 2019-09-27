/*********************************************************************
* Source:     https://github.com/B-Con/crypto-algorithms
* Filename:   sha256.h
* Author:     Brad Conte (brad AT bradconte.com)
* Copyright:  This code is released into the public domain.
* Disclaimer: This code is presented "as is" without any guarantees.
* Details:    Defines the API for the corresponding SHA1 implementation.
*********************************************************************/

#ifndef SHA256_H
#define SHA256_H

/*************************** HEADER FILES ***************************/
#include <stddef.h>

/****************************** MACROS ******************************/
#define SHA256_BLOCK_SIZE 32            // SHA256 outputs a 32 byte digest

/**************************** DATA TYPES ****************************/
typedef unsigned char BYTE;             // 8-bit byte
typedef unsigned int  WORD;             // 32-bit word, change to "long" for 16-bit machines

typedef struct {
	BYTE data[64];
	WORD datalen;
	unsigned long long bitlen;
	WORD state[8];
} CRYAL_SHA256_CTX;

/*********************** FUNCTION DECLARATIONS **********************/
void sha256_init(CRYAL_SHA256_CTX *ctx);
void sha256_update(CRYAL_SHA256_CTX *ctx, const BYTE data[], size_t len);
void sha256_final(CRYAL_SHA256_CTX *ctx, BYTE hash[]);

#endif   // SHA256_H
