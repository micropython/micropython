import asyncio
import os
import signal
from microdot import *  # noqa: F401, F403
from microdot.microdot import Microdot as BaseMicrodot, Request, Response, \
    NoCaseDict, abort
from microdot.websocket import WebSocket as BaseWebSocket, websocket_wrapper


class _BodyStream:  # pragma: no cover
    def __init__(self, receive):
        self.receive = receive
        self.data = b''
        self.more = True

    async def read_more(self):
        if self.more:
            packet = await self.receive()
            self.data += packet.get('body', b'')
            self.more = packet.get('more_body', False)

    async def read(self, n=-1):
        while self.more and len(self.data) < n:
            await self.read_more()
        if len(self.data) < n:
            data = self.data
            self.data = b''
            return data

        data = self.data[:n]
        self.data = self.data[n:]
        return data

    async def readline(self):
        return await self.readuntil()

    async def readexactly(self, n):
        return await self.read(n)

    async def readuntil(self, separator=b'\n'):
        if self.more and separator not in self.data:
            await self.read_more()
        data, self.data = self.data.split(separator, 1)
        return data


class Microdot(BaseMicrodot):
    """A subclass of the core :class:`Microdot <microdot.Microdot>` class that
    implements the ASGI protocol.

    This class must be used as the application instance when running under an
    ASGI web server.
    """
    def __init__(self):
        super().__init__()
        self.embedded_server = False

    async def asgi_app(self, scope, receive, send):
        """An ASGI application."""
        if scope['type'] not in ['http', 'websocket']:  # pragma: no cover
            return
        path = scope['path']
        if 'query_string' in scope and scope['query_string']:
            path += '?' + scope['query_string'].decode()
        headers = NoCaseDict()
        content_length = 0
        for key, value in scope.get('headers', []):
            key = key.decode().title()
            headers[key] = value.decode()
            if key == 'Content-Length':
                content_length = int(value)

        if content_length and content_length <= Request.max_body_length:
            body = b''
            more = True
            while more:
                packet = await receive()
                body += packet.get('body', b'')
                more = packet.get('more_body', False)
            stream = None
        else:
            body = b''
            stream = _BodyStream(receive)

        req = Request(
            self,
            (scope['client'][0], scope['client'][1]),
            scope.get('method', 'GET'),
            path,
            'HTTP/' + scope['http_version'],
            headers,
            body=body,
            stream=stream,
            sock=(receive, send))
        req.asgi_scope = scope

        res = await self.dispatch_request(req)
        res.complete()

        header_list = []
        for name, value in res.headers.items():
            if not isinstance(value, list):
                header_list.append((name.lower().encode(), value.encode()))
            else:
                for v in value:
                    header_list.append((name.lower().encode(), v.encode()))

        if scope['type'] != 'http':  # pragma: no cover
            return

        await send({'type': 'http.response.start',
                    'status': res.status_code,
                    'headers': header_list})

        cancelled = False

        async def cancel_monitor():
            nonlocal cancelled

            while True:
                event = await receive()
                if event is None or \
                        event['type'] == 'http.disconnect':  # pragma: no cover
                    cancelled = True
                    break

        monitor_task = asyncio.ensure_future(cancel_monitor())

        body_iter = res.body_iter().__aiter__()
        res_body = b''
        try:
            res_body = await body_iter.__anext__()
            while not cancelled:  # pragma: no branch
                next_body = await body_iter.__anext__()
                await send({'type': 'http.response.body',
                            'body': res_body,
                            'more_body': True})
                res_body = next_body
        except StopAsyncIteration:
            await send({'type': 'http.response.body',
                        'body': res_body,
                        'more_body': False})
        if hasattr(body_iter, 'aclose'):  # pragma: no branch
            await body_iter.aclose()
        cancelled = True
        await monitor_task

    async def __call__(self, scope, receive, send):
        return await self.asgi_app(scope, receive, send)

    def shutdown(self):
        if self.embedded_server:  # pragma: no cover
            super().shutdown()
        else:
            pid = os.getpgrp() if hasattr(os, 'getpgrp') else os.getpid()
            os.kill(pid, signal.SIGTERM)

    def run(self, host='0.0.0.0', port=5000, debug=False,
            **options):  # pragma: no cover
        """Normally you would not start the server by invoking this method.
        Instead, start your chosen ASGI web server and pass the ``Microdot``
        instance as the ASGI application.
        """
        self.embedded_server = True
        super().run(host=host, port=port, debug=debug, **options)


class WebSocket(BaseWebSocket):  # pragma: no cover
    async def handshake(self):
        connect = await self.request.sock[0]()
        if connect['type'] != 'websocket.connect':
            abort(400)
        await self.request.sock[1]({'type': 'websocket.accept'})

    async def receive(self):
        message = await self.request.sock[0]()
        if message['type'] == 'websocket.disconnect':
            raise OSError(32, 'Websocket connection closed')
        elif message['type'] != 'websocket.receive':
            raise OSError(32, 'Websocket message type not supported')
        return message.get('bytes', message.get('text'))

    async def send(self, data):
        if isinstance(data, str):
            await self.request.sock[1](
                {'type': 'websocket.send', 'text': data})
        else:
            await self.request.sock[1](
                {'type': 'websocket.send', 'bytes': data})

    async def close(self):
        if not self.closed:
            self.closed = True
            try:
                await self.request.sock[1]({'type': 'websocket.close'})
            except:  # noqa E722
                pass


async def websocket_upgrade(request):  # pragma: no cover
    """Upgrade a request handler to a websocket connection.

    This function can be called directly inside a route function to process a
    WebSocket upgrade handshake, for example after the user's credentials are
    verified. The function returns the websocket object::

        @app.route('/echo')
        async def echo(request):
            if not (await authenticate_user(request)):
                abort(401)
            ws = await websocket_upgrade(request)
            while True:
                message = await ws.receive()
                await ws.send(message)
    """
    ws = WebSocket(request) if not request.app.embedded_server else \
        BaseWebSocket(request)
    await ws.handshake()

    @request.after_request
    async def after_request(request, response):
        return Response.already_handled

    return ws


def with_websocket(f):  # pragma: no cover
    """Decorator to make a route a WebSocket endpoint.

    This decorator is used to define a route that accepts websocket
    connections. The route then receives a websocket object as a second
    argument that it can use to send and receive messages::

        @app.route('/echo')
        @with_websocket
        async def echo(request, ws):
            while True:
                message = await ws.receive()
                await ws.send(message)
    """
    return websocket_wrapper(f, websocket_upgrade)
