#ifndef _MDNS_H
#define _MDNS_H

#include <stdint.h>

/* Text Record */
typedef struct _TXTRecordRef_t {
	char PrivateData[16];
} TXTRecordRef;

extern void TXTRecordCreate(TXTRecordRef *txtRecord, uint16_t bufferLen, void *buffer);
extern int TXTRecordSetValue(TXTRecordRef *txtRecord, const char *key, uint8_t valueSize, const void *value);
extern void TXTRecordDeallocate(TXTRecordRef *txtRecord);

/* mDNS */
typedef void *DNSServiceRef;

extern int mDNSResponderInit(void);
extern void mDNSResponderDeinit(void);
extern DNSServiceRef mDNSRegisterService(char *name, char *service_type, char *domain, unsigned short port, TXTRecordRef *txtRecord);
extern void mDNSDeregisterService(DNSServiceRef serviceRef);
extern void mDNSUpdateService(DNSServiceRef serviceRef, TXTRecordRef *txtRecord, unsigned int ttl);
extern void mDNSRegisterAllInterfaces(void);
extern void mDNSDeregisterAllInterfaces(void);

#endif  /* _MDNS_H */
