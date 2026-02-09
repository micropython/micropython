import json
from microdot.microdot import Request, Response, AsyncBytesIO

try:
    from microdot.websocket import WebSocket
except:  # pragma: no cover  # noqa: E722
    WebSocket = None

__all__ = ['TestClient', 'TestResponse']


class TestResponse:
    """A response object issued by the Microdot test client."""
    def __init__(self):
        #: The numeric status code returned by the server.
        self.status_code = None
        #: The text reason associated with the status response, such as
        #: ``'OK'`` or ``'NOT FOUND'``. Set to ``None`` unless the application
        #: explicitly sets it on the response object.
        self.reason = None
        #: A dictionary with the response headers.
        self.headers = None
        #: The body of the response, as a bytes object.
        self.body = None
        #: The body of the response, decoded to a UTF-8 string. Set to
        #: ``None`` if the response cannot be represented as UTF-8 text.
        self.text = None
        #: The body of the JSON response, decoded to a dictionary or list. Set
        #: ``Note`` if the response does not have a JSON payload.
        self.json = None

    def _initialize_response(self, res):
        self.status_code = res.status_code
        self.reason = res.reason
        self.headers = res.headers

    async def _initialize_body(self, res):
        self.body = b''
        iter = res.body_iter()
        async for body in iter:  # pragma: no branch
            if isinstance(body, str):
                body = body.encode()
            self.body += body
        if hasattr(iter, 'aclose'):  # pragma: no branch
            await iter.aclose()

    def _process_text_body(self):
        try:
            self.text = self.body.decode()
        except ValueError:
            pass

    def _process_json_body(self):
        if 'Content-Type' in self.headers:  # pragma: no branch
            content_type = self.headers['Content-Type']
            if content_type.split(';')[0] == 'application/json':
                self.json = json.loads(self.text)

    @classmethod
    async def create(cls, res):
        test_res = cls()
        test_res._initialize_response(res)
        if not res.is_head:
            await test_res._initialize_body(res)
            test_res._process_text_body()
            test_res._process_json_body()
        return test_res


class TestClient:
    """A test client for Microdot.

    :param app: The Microdot application instance.
    :param cookies: A dictionary of cookies to use when sending requests to the
                    application.

    The following example shows how to create a test client for an application
    and send a test request::

        from microdot import Microdot

        app = Microdot()

        @app.get('/')
        async def index():
            return 'Hello, World!'

        async def test_hello_world(self):
            client = TestClient(app)
            res = await client.get('/')
            assert res.status_code == 200
            assert res.text == 'Hello, World!'
    """
    __test__ = False  # remove this class from pytest's test collection

    def __init__(self, app, cookies=None):
        self.app = app
        self.cookies = cookies or {}

    def _process_body(self, body, headers):
        if body is None:
            body = b''
        elif isinstance(body, (dict, list)):
            body = json.dumps(body).encode()
            if 'Content-Type' not in headers:  # pragma: no cover
                headers['Content-Type'] = 'application/json'
        elif isinstance(body, str):
            body = body.encode()
        if body and 'Content-Length' not in headers:
            headers['Content-Length'] = str(len(body))
        if 'Host' not in headers:  # pragma: no branch
            headers['Host'] = 'example.com:1234'
        return body, headers

    def _process_cookies(self, path, headers):
        cookies = ''
        for name, value in self.cookies.items():
            if isinstance(value, tuple):
                value, cookie_path = value
                if not path.startswith(cookie_path):
                    continue
            if cookies:
                cookies += '; '
            cookies += name + '=' + value
        if cookies:
            if 'Cookie' in headers:
                headers['Cookie'] += '; ' + cookies
            else:
                headers['Cookie'] = cookies
        return headers

    def _render_request(self, method, path, headers, body):
        request_bytes = '{method} {path} HTTP/1.0\n'.format(
            method=method, path=path)
        for header, value in headers.items():
            request_bytes += '{header}: {value}\n'.format(
                header=header, value=value)
        request_bytes = request_bytes.encode() + b'\n' + body
        return request_bytes

    def _update_cookies(self, res):
        cookies = res.headers.get('Set-Cookie', [])
        for cookie in cookies:
            cookie_name, cookie_value = cookie.split('=', 1)
            cookie_options = cookie_value.split(';')
            path = '/'
            delete = False
            for option in cookie_options[1:]:
                option = option.strip().lower()
                if option.startswith(
                        'max-age='):  # pragma: no cover
                    _, age = option.split('=', 1)
                    try:
                        age = int(age)
                    except ValueError:  # pragma: no cover
                        age = 0
                    if age <= 0:
                        delete = True
                elif option.startswith('expires='):
                    _, e = option.split('=', 1)
                    # this is a very limited parser for cookie expiry
                    # that only detects a cookie deletion request when
                    # the date is 1/1/1970
                    if '1 jan 1970' in e.lower():  # pragma: no branch
                        delete = True
                elif option.startswith('path='):
                    _, path = option.split('=', 1)
            if delete:
                if cookie_name in self.cookies:  # pragma: no branch
                    cookie_path = self.cookies[cookie_name][1] \
                        if isinstance(self.cookies[cookie_name], tuple) \
                        else '/'
                    if path == cookie_path:
                        del self.cookies[cookie_name]
            else:
                if path == '/':
                    self.cookies[cookie_name] = cookie_options[0]
                else:
                    self.cookies[cookie_name] = (cookie_options[0], path)

    async def request(self, method, path, headers=None, body=None, sock=None):
        headers = headers or {}
        body, headers = self._process_body(body, headers)
        headers = self._process_cookies(path, headers)
        request_bytes = self._render_request(method, path, headers, body)
        if sock:
            reader = sock[0]
            reader.buffer = request_bytes
            writer = sock[1]
        else:
            reader = AsyncBytesIO(request_bytes)
            writer = AsyncBytesIO(b'')

        req = await Request.create(self.app, reader, writer,
                                   ('127.0.0.1', 1234))
        res = await self.app.dispatch_request(req)
        if res == Response.already_handled:
            return None
        res.complete()

        self._update_cookies(res)
        return await TestResponse.create(res)

    async def get(self, path, headers=None):
        """Send a GET request to the application.

        :param path: The request URL.
        :param headers: A dictionary of headers to send with the request.

        This method returns a
        :class:`TestResponse <microdot_test_client.TestResponse>` object.
        """
        return await self.request('GET', path, headers=headers)

    async def post(self, path, headers=None, body=None):
        """Send a POST request to the application.

        :param path: The request URL.
        :param headers: A dictionary of headers to send with the request.
        :param body: The request body. If a dictionary or list is provided,
                     a JSON-encoded body will be sent. A string body is encoded
                     to bytes as UTF-8. A bytes body is sent as-is.

        This method returns a
        :class:`TestResponse <microdot_test_client.TestResponse>` object.
        """
        return await self.request('POST', path, headers=headers, body=body)

    async def put(self, path, headers=None, body=None):
        """Send a PUT request to the application.

        :param path: The request URL.
        :param headers: A dictionary of headers to send with the request.
        :param body: The request body. If a dictionary or list is provided,
                     a JSON-encoded body will be sent. A string body is encoded
                     to bytes as UTF-8. A bytes body is sent as-is.

        This method returns a
        :class:`TestResponse <microdot_test_client.TestResponse>` object.
        """
        return await self.request('PUT', path, headers=headers, body=body)

    async def patch(self, path, headers=None, body=None):
        """Send a PATCH request to the application.

        :param path: The request URL.
        :param headers: A dictionary of headers to send with the request.
        :param body: The request body. If a dictionary or list is provided,
                     a JSON-encoded body will be sent. A string body is encoded
                     to bytes as UTF-8. A bytes body is sent as-is.

        This method returns a
        :class:`TestResponse <microdot_test_client.TestResponse>` object.
        """
        return await self.request('PATCH', path, headers=headers, body=body)

    async def delete(self, path, headers=None):
        """Send a DELETE request to the application.

        :param path: The request URL.
        :param headers: A dictionary of headers to send with the request.

        This method returns a
        :class:`TestResponse <microdot_test_client.TestResponse>` object.
        """
        return await self.request('DELETE', path, headers=headers)

    async def websocket(self, path, client, headers=None):
        """Send a websocket connection request to the application.

        :param path: The request URL.
        :param client: A generator function that yields client messages.
        :param headers: A dictionary of headers to send with the request.
        """
        gen = client()

        class FakeWebSocket:
            def __init__(self):
                self.started = False
                self.closed = False
                self.buffer = b''

            async def _next(self, data=None):
                try:
                    data = (await gen.asend(data)) if hasattr(gen, 'asend') \
                        else gen.send(data)
                except (StopIteration, StopAsyncIteration):
                    if not self.closed:
                        self.closed = True
                        raise OSError(32, 'Websocket connection closed')
                    return  # pragma: no cover
                opcode = WebSocket.TEXT if isinstance(data, str) \
                    else WebSocket.BINARY
                return WebSocket._encode_websocket_frame(opcode, data)

            async def read(self, n):
                if not self.buffer:
                    self.started = True
                    self.buffer = await self._next()
                data = self.buffer[:n]
                self.buffer = self.buffer[n:]
                return data

            async def readexactly(self, n):  # pragma: no cover
                return await self.read(n)

            async def readline(self):
                line = b''
                while True:
                    line += await self.read(1)
                    if line[-1] in [b'\n', 10]:
                        break
                return line

            async def awrite(self, data):
                if self.started:
                    h = WebSocket._parse_frame_header(data[0:2])
                    if h[1] not in [WebSocket.TEXT, WebSocket.BINARY]:
                        return
                    if h[3] < 0:
                        data = data[2 - h[3]:]
                    else:
                        data = data[2:]
                    if h[1] == WebSocket.TEXT:
                        data = data.decode()
                    self.buffer = await self._next(data)

        ws_headers = {
            'Upgrade': 'websocket',
            'Connection': 'Upgrade',
            'Sec-WebSocket-Version': '13',
            'Sec-WebSocket-Key': 'dGhlIHNhbXBsZSBub25jZQ==',
        }
        ws_headers.update(headers or {})
        sock = FakeWebSocket()
        return await self.request('GET', path, headers=ws_headers,
                                  sock=(sock, sock))
