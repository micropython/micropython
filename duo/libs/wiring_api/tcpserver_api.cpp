#include "spark_wiring_tcpclient.h"
#include "spark_wiring_tcpserver.h"

#ifdef __cplusplus
extern "C" {
#endif


void* TCPServer_newTCPServer(uint16_t port)
{
    TCPServer *_tcp_server = new TCPServer(port);
    return ((void *)_tcp_server);
}

void TCPServer_deleteTCPServer(void *tcp_server)
{
    delete ((TCPServer *)tcp_server);
}

bool TCPServer_begin(void *tcp_server)
{
    return ((TCPServer *)tcp_server)->begin();
}

void TCPServer_stop(void *tcp_server)
{
    ((TCPServer *)tcp_server)->stop();
}

void* TCPServer_available(void *tcp_server)
{
    TCPClient *_tcp_client = new TCPClient();

    if(_tcp_client != NULL) {
        *_tcp_client = ((TCPServer *)tcp_server)->available();
        if(*_tcp_client)
        {
            return (void *)_tcp_client;
        }
        else
        {
    	    delete _tcp_client;
            return NULL;
        }
    }
    return NULL;
}

size_t TCPServer_write(void *tcp_server, uint8_t *buf, size_t size)
{
    return ((TCPServer *)tcp_server)->write(buf, size);
}


#ifdef __cplusplus
}
#endif
