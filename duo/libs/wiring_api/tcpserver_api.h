#ifndef _TCP_SERVER_API_H
#define _TCP_SERVER_API_H

typedef struct TCPServer tcp_server;

void* TCPServer_newTCPServer(uint16_t port);
void TCPServer_deleteTCPServer(void *tcp_server);
bool TCPServer_begin(void *tcp_server);
void TCPServer_stop(void *tcp_server);

/*
 * User who invoke this function has the responsibility to destruct
 * the client returned by this function using TCPClient_deleteTCPClient().
 */
void* TCPServer_available(void *tcp_server);

size_t TCPServer_write(void *tcp_server, uint8_t *buf, size_t size);


#endif
