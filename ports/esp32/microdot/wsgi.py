import asyncio
import os
import signal
from microdot import *  # noqa: F401, F403
from microdot.microdot import Microdot as BaseMicrodot, Request, NoCaseDict, \
    MUTED_SOCKET_ERRORS
from microdot.websocket import WebSocket, websocket_upgrade, \
    with_websocket  # noqa: F401


class Microdot(BaseMicrodot):
    """A subclass of the core :class:`Microdot <microdot.Microdot>` class that
    implements the WSGI protocol.

    This class must be used as the application instance when running under a
    WSGI web server.
    """
    def __init__(self):
        super().__init__()
        self.loop = asyncio.new_event_loop()
        self.embedded_server = False

    def wsgi_app(self, environ, start_response):
        """A WSGI application callable."""
        path = environ.get('SCRIPT_NAME', '') + environ.get('PATH_INFO', '')
        if 'QUERY_STRING' in environ and environ['QUERY_STRING']:
            path += '?' + environ['QUERY_STRING']
        headers = NoCaseDict()
        content_length = 0
        for k, value in environ.items():
            if k.startswith('HTTP_'):
                key = '-'.join([p.title() for p in k[5:].split('_')])
                headers[key] = value
            elif k == 'CONTENT_TYPE':
                headers['Content-Type'] = value
            elif k == 'CONTENT_LENGTH':
                headers['Content-Length'] = value
                content_length = int(value)

        class sync_to_async_body_stream():  # pragma: no cover
            def __init__(self, wsgi_input=None):
                self.wsgi_input = wsgi_input

            async def read(self, n=-1):
                return self.wsgi_input.read(n)

            async def readline(self):
                return self.wsgi_input.readline()

            async def readexactly(self, n):
                return self.wsgi_input.read(n)

        wsgi_input = environ.get('wsgi.input')
        if content_length and content_length <= Request.max_body_length:
            # the request came with a body that is within the allowed size
            body = wsgi_input.read(content_length)
            stream = None
            sock = (None, None)
        else:
            body = b''
            if content_length:
                # the request came with a body that is too large to fit in
                # memory, so we stream it
                stream = sync_to_async_body_stream(wsgi_input)
                sock = (None, None)
            else:
                # the request did not declare a body size, so we connect the
                # raw socket if available
                stream = None
                if 'gunicorn.socket' in environ:  # pragma: no cover
                    reader, writer = self.loop.run_until_complete(
                        asyncio.open_connection(
                            sock=environ['gunicorn.socket'].dup()))
                    if not hasattr(writer, 'awrite'):  # pragma: no cover
                        async def awrite(self, data):
                            self.write(data)
                            await self.drain()

                        async def aclose(self):
                            self.close()
                            await self.wait_closed()

                        from types import MethodType
                        writer.awrite = MethodType(awrite, writer)
                        writer.aclose = MethodType(aclose, writer)
                    sock = (reader, writer)
                else:
                    sock = (None, None)

        req = Request(
            self,
            (environ['REMOTE_ADDR'], int(environ.get('REMOTE_PORT', '0'))),
            environ['REQUEST_METHOD'],
            path,
            environ['SERVER_PROTOCOL'],
            headers,
            body=body,
            stream=stream,
            sock=sock)
        req.environ = environ

        res = self.loop.run_until_complete(self.dispatch_request(req))
        res.complete()
        if sock[1]:  # pragma: no cover
            try:
                self.loop.run_until_complete(sock[1].aclose())
            except OSError as exc:  # pragma: no cover
                if exc.errno in MUTED_SOCKET_ERRORS:
                    pass
                else:
                    raise

        reason = res.reason or ('OK' if res.status_code == 200 else 'N/A')
        header_list = []
        for name, value in res.headers.items():
            if not isinstance(value, list):
                header_list.append((name, value))
            else:
                for v in value:
                    header_list.append((name, v))
        start_response(str(res.status_code) + ' ' + reason, header_list)

        class async_to_sync_iter():
            def __init__(self, iter, loop):
                self.iter = iter.__aiter__()
                self.loop = loop

            def __iter__(self):
                return self

            def __next__(self):
                try:
                    return self.loop.run_until_complete(self.iter.__anext__())
                except StopAsyncIteration:
                    raise StopIteration

            def close(self):  # pragma: no cover
                if hasattr(self.iter, 'aclose'):
                    self.loop.run_until_complete(self.iter.aclose())

        return async_to_sync_iter(res.body_iter(), self.loop)

    def __call__(self, environ, start_response):
        return self.wsgi_app(environ, start_response)

    def shutdown(self):
        if self.embedded_server:  # pragma: no cover
            super().shutdown()
        else:
            pid = os.getpgrp() if hasattr(os, 'getpgrp') else os.getpid()
            os.kill(pid, signal.SIGTERM)

    def run(self, host='0.0.0.0', port=5000, debug=False,
            **options):  # pragma: no cover
        """Normally you would not start the server by invoking this method.
        Instead, start your chosen WSGI web server and pass the ``Microdot``
        instance as the WSGI callable.
        """
        self.embedded_server = True
        super().run(host=host, port=port, debug=debug, **options)
