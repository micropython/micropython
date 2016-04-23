#ifndef _TCP_CLIENT_API_H
#define _TCP_CLIENT_API_H

typedef struct TCPClient tcp_client;

void* TCPClient_newTCPClient(void);
void TCPClient_deleteTCPClient(void *tcp_client);

int TCPClient_connectByIP(void *tcp_client, uint32_t ip, uint16_t port);
int TCPClient_connectByHostName(void *tcp_client, const char *host, uint16_t port);

size_t TCPClient_write(void *tcp_client, uint8_t *buf, size_t size);
int TCPClient_available(void *tcp_client);
int TCPClient_read(void *tcp_client, uint8_t *buffer, size_t size);
int TCPClient_peek(void *tcp_client);
void TCPClient_flush(void *tcp_client);
void TCPClient_flushBuffer(void *tcp_client);

void TCPClient_stop(void *tcp_client);

uint8_t TCPClient_status(void *tcp_client);
uint8_t TCPClient_connected(void *tcp_client);


#endif
