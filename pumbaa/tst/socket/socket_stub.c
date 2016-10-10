/**
 * @file socket_stub.c
 *
 * @section License
 * Copyright (C) 2016, Erik Moqvist
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERSOCKTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * This file is part of the Simba project.
 */

#include "simba.h"

int socket_module_init()
{
    return (0);
}

int socket_open_tcp(struct socket_t *self_p)
{
    /* Channel functions. */
    self_p->base.read = (chan_read_fn_t)socket_read;
    self_p->base.write = (chan_write_fn_t)socket_write;
    self_p->base.size = (chan_size_fn_t)socket_size;

    return (0);
}

int socket_open_udp(struct socket_t *self_p)
{
    return (0);
}

int socket_open_raw(struct socket_t *self_p)
{
    return (0);
}

int socket_close(struct socket_t *self_p)
{
    return (0);
}

int socket_bind(struct socket_t *self_p,
                const struct inet_addr_t *local_addr_p)
{
    return (0);
}

int socket_listen(struct socket_t *self_p, int backlog)
{
    if (backlog != 5) {
        return (-1);
    }
    
    return (0);
}

int socket_connect(struct socket_t *self_p,
                   const struct inet_addr_t *addr_p)
{
    char ip[16];
    
    if (strcmp("192.168.1.101", inet_ntoa(&addr_p->ip, &ip[0])) != 0) {
        return (-1);
    }

    if (addr_p->port != 80) {
        return (-1);
    }
    
    return (0);
}

int socket_accept(struct socket_t *self_p,
                  struct socket_t *accepted_p,
                  struct inet_addr_t *addr_p)
{
    return (0);
}

ssize_t socket_sendto(struct socket_t *self_p,
                      const void *buf_p,
                      size_t size,
                      int flags,
                      const struct inet_addr_t *remote_addr_p)
{
    return (0);
}

ssize_t socket_recvfrom(struct socket_t *self_p,
                        void *buf_p,
                        size_t size,
                        int flags,
                        struct inet_addr_t *remote_addr)
{
    return (0);
}

ssize_t socket_write(struct socket_t *self_p,
                     const void *buf_p,
                     size_t size)
{
    if (memcmp(buf_p, "foo", size) != 0) {
        return (-1);
    }

    return (size);
}

ssize_t socket_read(struct socket_t *self_p,
                    void *buf_p,
                    size_t size)
{
    memcpy(buf_p, "bar", size);

    return (size);
}

ssize_t socket_size(struct socket_t *self_p)
{
    return (0);
}
