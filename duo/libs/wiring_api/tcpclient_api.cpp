#include "spark_wiring_tcpclient.h"

#ifdef __cplusplus
extern "C" {
#endif


void* TCPClient_newTCPClient(void)
{
    TCPClient *_tcp_client = new TCPClient();
    return ((void *)_tcp_client);
}

void TCPClient_deleteTCPClient(void *tcp_client)
{
    delete ((TCPClient *)tcp_client);
}

int TCPClient_connectByIP(void *tcp_client, uint32_t ip, uint16_t port)
{
    return ((TCPClient *)tcp_client)->connect(ip, port);
}

int TCPClient_connectByHostName(void *tcp_client, const char *host, uint16_t port)
{
    return ((TCPClient *)tcp_client)->connect(host, port);
}

size_t TCPClient_write(void *tcp_client, uint8_t *buf, size_t size)
{
    return ((TCPClient *)tcp_client)->write(buf, size);
}

int TCPClient_available(void *tcp_client)
{
    return ((TCPClient *)tcp_client)->available();
}

int TCPClient_read(void *tcp_client, uint8_t *buffer, size_t size)
{
    return ((TCPClient *)tcp_client)->read(buffer, size);
}

int TCPClient_peek(void *tcp_client)
{
    return ((TCPClient *)tcp_client)->peek();
}

void TCPClient_flush(void *tcp_client)
{
    ((TCPClient *)tcp_client)->flush();
}

void TCPClient_flushBuffer(void *tcp_client)
{
    ((TCPClient *)tcp_client)->flush_buffer();
}

void TCPClient_stop(void *tcp_client)
{
    ((TCPClient *)tcp_client)->stop();
}

uint8_t TCPClient_status(void *tcp_client)
{
    return ((TCPClient *)tcp_client)->status();
}

uint8_t TCPClient_connected(void *tcp_client)
{
    return ((TCPClient *)tcp_client)->connected();
}


#ifdef __cplusplus
}
#endif
