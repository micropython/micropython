import asyncio
import socket

# Helper to run a TCP stream server.
# Callbacks (i.e. connection handlers) may run in a different taskgroup.
async def run_server(cb, host, port, backlog=5, taskgroup=None):

    # Create and bind server socket.
    host = socket.getaddrinfo(host, port)[0]  # TODO this is blocking!
    s = socket.socket()
    s.setblocking(False)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(host[-1])
    s.listen(backlog)

    if taskgroup is None:
        from asyncio import TaskGroup

        async with TaskGroup() as tg:
            await _run_server(tg, s, cb)
    else:
        await _run_server(taskgroup, s, cb)


async def _run_server(tg, s, cb):
    try:
        while True:
            yield asyncio._io_queue.queue_read(s)
            try:
                s2, addr = s.accept()
            except Exception:
                # Ignore a failed accept
                continue

            s2.setblocking(False)
            s2s = Stream(s2, {"peername": addr})
            tg.create_task(cb(s2s, s2s))
    finally:
        s.close()


