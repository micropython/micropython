#ifndef HASH_H_
#define HASH_H_

extern void HASH_Init (void);
extern void HASH_SHAMD5Start (uint32_t algo, uint32_t blocklen);
extern void HASH_SHAMD5Update (uint8_t *data, uint32_t datalen);
extern void HASH_SHAMD5Read (uint8_t *hash);


#endif /* HASH_H_ */
