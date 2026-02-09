import binascii
import hashlib
from microdot import Request, Response
from microdot.microdot import MUTED_SOCKET_ERRORS, print_exception
from microdot.helpers import wraps


class WebSocketError(Exception):
    """Exception raised when an error occurs in a WebSocket connection."""
    pass


class WebSocket:
    """A WebSocket connection object.

    An instance of this class is sent to handler functions to manage the
    WebSocket connection.
    """
    CONT = 0
    TEXT = 1
    BINARY = 2
    CLOSE = 8
    PING = 9
    PONG = 10

    #: Specify the maximum message size that can be received when calling the
    #: ``receive()`` method. Messages with payloads that are larger than this
    #: size will be rejected and the connection closed. Set to 0 to disable
    #: the size check (be aware of potential security issues if you do this),
    #: or to -1 to use the value set in
    #: ``Request.max_body_length``. The default is -1.
    #:
    #: Example::
    #:
    #:    WebSocket.max_message_length = 4 * 1024  # up to 4KB messages
    max_message_length = -1

    def __init__(self, request):
        self.request = request
        self.closed = False

    async def handshake(self):
        response = self._handshake_response()
        await self.request.sock[1].awrite(
            b'HTTP/1.1 101 Switching Protocols\r\n')
        await self.request.sock[1].awrite(b'Upgrade: websocket\r\n')
        await self.request.sock[1].awrite(b'Connection: Upgrade\r\n')
        await self.request.sock[1].awrite(
            b'Sec-WebSocket-Accept: ' + response + b'\r\n\r\n')

    async def receive(self):
        """Receive a message from the client."""
        while True:
            opcode, payload = await self._read_frame()
            send_opcode, data = self._process_websocket_frame(opcode, payload)
            if send_opcode:  # pragma: no cover
                await self.send(data, send_opcode)
            elif data:  # pragma: no branch
                return data

    async def send(self, data, opcode=None):
        """Send a message to the client.

        :param data: the data to send, given as a string or bytes.
        :param opcode: a custom frame opcode to use. If not given, the opcode
                       is ``TEXT`` or ``BINARY`` depending on the type of the
                       data.
        """
        frame = self._encode_websocket_frame(
            opcode or (self.TEXT if isinstance(data, str) else self.BINARY),
            data)
        await self.request.sock[1].awrite(frame)

    async def close(self):
        """Close the websocket connection."""
        if not self.closed:  # pragma: no cover
            self.closed = True
            await self.send(b'', self.CLOSE)

    def _handshake_response(self):
        connection = False
        upgrade = False
        websocket_key = None
        for header, value in self.request.headers.items():
            h = header.lower()
            if h == 'connection':
                connection = True
                if 'upgrade' not in value.lower():
                    return self.request.app.abort(400)
            elif h == 'upgrade':
                upgrade = True
                if not value.lower() == 'websocket':
                    return self.request.app.abort(400)
            elif h == 'sec-websocket-key':
                websocket_key = value
        if not connection or not upgrade or not websocket_key:
            return self.request.app.abort(400)
        d = hashlib.sha1(websocket_key.encode())
        d.update(b'258EAFA5-E914-47DA-95CA-C5AB0DC85B11')
        return binascii.b2a_base64(d.digest())[:-1]

    @classmethod
    def _parse_frame_header(cls, header):
        fin = header[0] & 0x80
        opcode = header[0] & 0x0f
        if fin == 0 or opcode == cls.CONT:  # pragma: no cover
            raise WebSocketError('Continuation frames not supported')
        has_mask = header[1] & 0x80
        length = header[1] & 0x7f
        if length == 126:
            length = -2
        elif length == 127:
            length = -8
        return fin, opcode, has_mask, length

    def _process_websocket_frame(self, opcode, payload):
        if opcode == self.TEXT:
            payload = payload.decode()
        elif opcode == self.BINARY:
            pass
        elif opcode == self.CLOSE:
            raise WebSocketError('Websocket connection closed')
        elif opcode == self.PING:
            return self.PONG, payload
        elif opcode == self.PONG:  # pragma: no branch
            return None, None
        return None, payload

    @classmethod
    def _encode_websocket_frame(cls, opcode, payload):
        frame = bytearray()
        frame.append(0x80 | opcode)
        if opcode == cls.TEXT:
            payload = payload.encode()
        if len(payload) < 126:
            frame.append(len(payload))
        elif len(payload) < (1 << 16):
            frame.append(126)
            frame.extend(len(payload).to_bytes(2, 'big'))
        else:
            frame.append(127)
            frame.extend(len(payload).to_bytes(8, 'big'))
        frame.extend(payload)
        return frame

    async def _read_frame(self):
        header = await self.request.sock[0].read(2)
        if len(header) != 2:  # pragma: no cover
            raise WebSocketError('Websocket connection closed')
        fin, opcode, has_mask, length = self._parse_frame_header(header)
        if length == -2:
            length = await self.request.sock[0].read(2)
            length = int.from_bytes(length, 'big')
        elif length == -8:
            length = await self.request.sock[0].read(8)
            length = int.from_bytes(length, 'big')
        max_allowed_length = Request.max_body_length \
            if self.max_message_length == -1 else self.max_message_length
        if length > max_allowed_length:
            raise WebSocketError('Message too large')
        if has_mask:  # pragma: no cover
            mask = await self.request.sock[0].read(4)
        payload = await self.request.sock[0].read(length)
        if has_mask:  # pragma: no cover
            payload = bytes(x ^ mask[i % 4] for i, x in enumerate(payload))
        return opcode, payload


async def websocket_upgrade(request):
    """Upgrade a request handler to a websocket connection.

    This function can be called directly inside a route function to process a
    WebSocket upgrade handshake, for example after the user's credentials are
    verified. The function returns the websocket object::

        @app.route('/echo')
        async def echo(request):
            if not authenticate_user(request):
                abort(401)
            ws = await websocket_upgrade(request)
            while True:
                message = await ws.receive()
                await ws.send(message)
    """
    ws = WebSocket(request)
    await ws.handshake()

    @request.after_request
    async def after_request(request, response):
        return Response.already_handled

    return ws


def websocket_wrapper(f, upgrade_function):
    @wraps(f)
    async def wrapper(request, *args, **kwargs):
        ws = await upgrade_function(request)
        try:
            await f(request, ws, *args, **kwargs)
        except OSError as exc:
            if exc.errno not in MUTED_SOCKET_ERRORS:  # pragma: no cover
                raise
        except WebSocketError:
            pass
        except Exception as exc:
            print_exception(exc)
        finally:  # pragma: no cover
            try:
                await ws.close()
            except Exception:
                pass
        return Response.already_handled
    return wrapper


def with_websocket(f):
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
