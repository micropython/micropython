"""
microdot
--------

The ``microdot`` module defines a few classes that help implement HTTP-based
servers for MicroPython and standard Python.
"""
import asyncio
import io
import json
import time

try:
    from inspect import iscoroutinefunction, iscoroutine
    from functools import partial

    async def invoke_handler(handler, *args, **kwargs):
        """Invoke a handler and return the result.

        This method runs sync handlers in a thread pool executor.
        """
        if iscoroutinefunction(handler):
            ret = await handler(*args, **kwargs)
        else:
            ret = await asyncio.get_running_loop().run_in_executor(
                None, partial(handler, *args, **kwargs))
        return ret
except ImportError:  # pragma: no cover
    def iscoroutine(coro):
        return hasattr(coro, 'send') and hasattr(coro, 'throw')

    async def invoke_handler(handler, *args, **kwargs):
        """Invoke a handler and return the result.

        This method runs sync handlers in the asyncio thread, which can
        potentially cause blocking and performance issues.
        """
        ret = handler(*args, **kwargs)
        if iscoroutine(ret):
            ret = await ret
        return ret

try:
    from sys import print_exception
except ImportError:  # pragma: no cover
    import traceback

    def print_exception(exc):
        traceback.print_exc()

MUTED_SOCKET_ERRORS = [
    32,  # Broken pipe
    54,  # Connection reset by peer
    104,  # Connection reset by peer
    128,  # Operation on closed socket
]


def urldecode_str(s):
    s = s.replace('+', ' ')
    parts = s.split('%')
    if len(parts) == 1:
        return s
    result = [parts[0]]
    for item in parts[1:]:
        if item == '':
            result.append('%')
        else:
            code = item[:2]
            result.append(chr(int(code, 16)))
            result.append(item[2:])
    return ''.join(result)


def urldecode_bytes(s):
    s = s.replace(b'+', b' ')
    parts = s.split(b'%')
    if len(parts) == 1:
        return s.decode()
    result = [parts[0]]
    for item in parts[1:]:
        if item == b'':
            result.append(b'%')
        else:
            code = item[:2]
            result.append(bytes([int(code, 16)]))
            result.append(item[2:])
    return b''.join(result).decode()


def urlencode(s):
    return s.replace('+', '%2B').replace(' ', '+').replace(
        '%', '%25').replace('?', '%3F').replace('#', '%23').replace(
            '&', '%26').replace('=', '%3D')


class NoCaseDict(dict):
    """A subclass of dictionary that holds case-insensitive keys.

    :param initial_dict: an initial dictionary of key/value pairs to
                         initialize this object with.

    Example::

        >>> d = NoCaseDict()
        >>> d['Content-Type'] = 'text/html'
        >>> print(d['Content-Type'])
        text/html
        >>> print(d['content-type'])
        text/html
        >>> print(d['CONTENT-TYPE'])
        text/html
        >>> del d['cOnTeNt-TyPe']
        >>> print(d)
        {}
    """
    def __init__(self, initial_dict=None):
        super().__init__(initial_dict or {})
        self.keymap = {k.lower(): k for k in self.keys() if k.lower() != k}

    def __setitem__(self, key, value):
        kl = key.lower()
        key = self.keymap.get(kl, key)
        if kl != key:
            self.keymap[kl] = key
        super().__setitem__(key, value)

    def __getitem__(self, key):
        kl = key.lower()
        return super().__getitem__(self.keymap.get(kl, kl))

    def __delitem__(self, key):
        kl = key.lower()
        super().__delitem__(self.keymap.get(kl, kl))

    def __contains__(self, key):
        kl = key.lower()
        return self.keymap.get(kl, kl) in self.keys()

    def get(self, key, default=None):
        kl = key.lower()
        return super().get(self.keymap.get(kl, kl), default)

    def update(self, other_dict):
        for key, value in other_dict.items():
            self[key] = value


def mro(cls):  # pragma: no cover
    """Return the method resolution order of a class.

    This is a helper function that returns the method resolution order of a
    class. It is used by Microdot to find the best error handler to invoke for
    the raised exception.

    In CPython, this function returns the ``__mro__`` attribute of the class.
    In MicroPython, this function implements a recursive depth-first scanning
    of the class hierarchy.
    """
    if hasattr(cls, 'mro'):
        return cls.__mro__

    def _mro(cls):
        m = [cls]
        for base in cls.__bases__:
            m += _mro(base)
        return m

    mro_list = _mro(cls)

    # If a class appears multiple times (due to multiple inheritance) remove
    # all but the last occurence. This matches the method resolution order
    # of MicroPython, but not CPython.
    mro_pruned = []
    for i in range(len(mro_list)):
        base = mro_list.pop(0)
        if base not in mro_list:
            mro_pruned.append(base)
    return mro_pruned


class MultiDict(dict):
    """A subclass of dictionary that can hold multiple values for the same
    key. It is used to hold key/value pairs decoded from query strings and
    form submissions.

    :param initial_dict: an initial dictionary of key/value pairs to
                         initialize this object with.

    Example::

        >>> d = MultiDict()
        >>> d['sort'] = 'name'
        >>> d['sort'] = 'email'
        >>> print(d['sort'])
        'name'
        >>> print(d.getlist('sort'))
        ['name', 'email']
    """
    def __init__(self, initial_dict=None):
        super().__init__()
        if initial_dict:
            for key, value in initial_dict.items():
                self[key] = value

    def __setitem__(self, key, value):
        if key not in self:
            super().__setitem__(key, [])
        super().__getitem__(key).append(value)

    def __getitem__(self, key):
        return super().__getitem__(key)[0]

    def get(self, key, default=None, type=None):
        """Return the value for a given key.

        :param key: The key to retrieve.
        :param default: A default value to use if the key does not exist.
        :param type: A type conversion callable to apply to the value.

        If the multidict contains more than one value for the requested key,
        this method returns the first value only.

        Example::

            >>> d = MultiDict()
            >>> d['age'] = '42'
            >>> d.get('age')
            '42'
            >>> d.get('age', type=int)
            42
            >>> d.get('name', default='noname')
            'noname'
        """
        if key not in self:
            return default
        value = self[key]
        if type is not None:
            value = type(value)
        return value

    def getlist(self, key, type=None):
        """Return all the values for a given key.

        :param key: The key to retrieve.
        :param type: A type conversion callable to apply to the values.

        If the requested key does not exist in the dictionary, this method
        returns an empty list.

        Example::

            >>> d = MultiDict()
            >>> d.getlist('items')
            []
            >>> d['items'] = '3'
            >>> d.getlist('items')
            ['3']
            >>> d['items'] = '56'
            >>> d.getlist('items')
            ['3', '56']
            >>> d.getlist('items', type=int)
            [3, 56]
        """
        if key not in self:
            return []
        values = super().__getitem__(key)
        if type is not None:
            values = [type(value) for value in values]
        return values


class AsyncBytesIO:
    """An async wrapper for BytesIO."""
    def __init__(self, data):
        self.stream = io.BytesIO(data)

    async def read(self, n=-1):
        return self.stream.read(n)

    async def readline(self):  # pragma: no cover
        return self.stream.readline()

    async def readexactly(self, n):  # pragma: no cover
        return self.stream.read(n)

    async def readuntil(self, separator=b'\n'):  # pragma: no cover
        return self.stream.readuntil(separator=separator)

    async def awrite(self, data):  # pragma: no cover
        return self.stream.write(data)

    async def aclose(self):  # pragma: no cover
        pass


class Request:
    """An HTTP request."""
    #: Specify the maximum payload size that is accepted. Requests with larger
    #: payloads will be rejected with a 413 status code. Applications can
    #: change this maximum as necessary.
    #:
    #: Example::
    #:
    #:    Request.max_content_length = 1 * 1024 * 1024  # 1MB requests allowed
    max_content_length = 16 * 1024

    #: Specify the maximum payload size that can be stored in ``body``.
    #: Requests with payloads that are larger than this size and up to
    #: ``max_content_length`` bytes will be accepted, but the application will
    #: only be able to access the body of the request by reading from
    #: ``stream``. Set to 0 if you always access the body as a stream.
    #:
    #: Example::
    #:
    #:    Request.max_body_length = 4 * 1024  # up to 4KB bodies read
    max_body_length = 16 * 1024

    #: Specify the maximum length allowed for a line in the request. Requests
    #: with longer lines will not be correctly interpreted. Applications can
    #: change this maximum as necessary.
    #:
    #: Example::
    #:
    #:    Request.max_readline = 16 * 1024  # 16KB lines allowed
    max_readline = 2 * 1024

    class G:
        pass

    def __init__(self, app, client_addr, method, url, http_version, headers,
                 body=None, stream=None, sock=None):
        #: The application instance to which this request belongs.
        self.app = app
        #: The address of the client, as a tuple (host, port).
        self.client_addr = client_addr
        #: The HTTP method of the request.
        self.method = method
        #: The request URL, including the path and query string.
        self.url = url
        #: The path portion of the URL.
        self.path = url
        #: The query string portion of the URL.
        self.query_string = None
        #: The parsed query string, as a
        #: :class:`MultiDict <microdot.MultiDict>` object.
        self.args = {}
        #: A dictionary with the headers included in the request.
        self.headers = headers
        #: A dictionary with the cookies included in the request.
        self.cookies = {}
        #: The parsed ``Content-Length`` header.
        self.content_length = 0
        #: The parsed ``Content-Type`` header.
        self.content_type = None
        #: A general purpose container for applications to store data during
        #: the life of the request.
        self.g = Request.G()

        self.http_version = http_version
        if '?' in self.path:
            self.path, self.query_string = self.path.split('?', 1)
            self.args = self._parse_urlencoded(self.query_string)

        if 'Content-Length' in self.headers:
            self.content_length = int(self.headers['Content-Length'])
        if 'Content-Type' in self.headers:
            self.content_type = self.headers['Content-Type']
        if 'Cookie' in self.headers:
            for cookie in self.headers['Cookie'].split(';'):
                name, value = cookie.strip().split('=', 1)
                self.cookies[name] = value

        self._body = body
        self.body_used = False
        self._stream = stream
        self.sock = sock
        self._json = None
        self._form = None
        self.after_request_handlers = []

    @staticmethod
    async def create(app, client_reader, client_writer, client_addr):
        """Create a request object.

        :param app: The Microdot application instance.
        :param client_reader: An input stream from where the request data can
                              be read.
        :param client_writer: An output stream where the response data can be
                              written.
        :param client_addr: The address of the client, as a tuple.

        This method is a coroutine. It returns a newly created ``Request``
        object.
        """
        # request line
        line = (await Request._safe_readline(client_reader)).strip().decode()
        if not line:  # pragma: no cover
            return None
        method, url, http_version = line.split()
        http_version = http_version.split('/', 1)[1]

        # headers
        headers = NoCaseDict()
        content_length = 0
        while True:
            line = (await Request._safe_readline(
                client_reader)).strip().decode()
            if line == '':
                break
            header, value = line.split(':', 1)
            value = value.strip()
            headers[header] = value
            if header.lower() == 'content-length':
                content_length = int(value)

        # body
        body = b''
        if content_length and content_length <= Request.max_body_length:
            body = await client_reader.readexactly(content_length)
            stream = None
        else:
            body = b''
            stream = client_reader

        return Request(app, client_addr, method, url, http_version, headers,
                       body=body, stream=stream,
                       sock=(client_reader, client_writer))

    def _parse_urlencoded(self, urlencoded):
        data = MultiDict()
        if len(urlencoded) > 0:  # pragma: no branch
            if isinstance(urlencoded, str):
                for kv in [pair.split('=', 1)
                           for pair in urlencoded.split('&') if pair]:
                    data[urldecode_str(kv[0])] = urldecode_str(kv[1]) \
                        if len(kv) > 1 else ''
            elif isinstance(urlencoded, bytes):  # pragma: no branch
                for kv in [pair.split(b'=', 1)
                           for pair in urlencoded.split(b'&') if pair]:
                    data[urldecode_bytes(kv[0])] = urldecode_bytes(kv[1]) \
                        if len(kv) > 1 else b''
        return data

    @property
    def body(self):
        """The body of the request, as bytes."""
        return self._body

    @property
    def stream(self):
        """The body of the request, as a bytes stream."""
        if self._stream is None:
            self._stream = AsyncBytesIO(self._body)
        return self._stream

    @property
    def json(self):
        """The parsed JSON body, or ``None`` if the request does not have a
        JSON body."""
        if self._json is None:
            if self.content_type is None:
                return None
            mime_type = self.content_type.split(';')[0]
            if mime_type != 'application/json':
                return None
            self._json = json.loads(self.body.decode())
        return self._json

    @property
    def form(self):
        """The parsed form submission body, as a
        :class:`MultiDict <microdot.MultiDict>` object, or ``None`` if the
        request does not have a form submission."""
        if self._form is None:
            if self.content_type is None:
                return None
            mime_type = self.content_type.split(';')[0]
            if mime_type != 'application/x-www-form-urlencoded':
                return None
            self._form = self._parse_urlencoded(self.body)
        return self._form

    def after_request(self, f):
        """Register a request-specific function to run after the request is
        handled. Request-specific after request handlers run at the very end,
        after the application's own after request handlers. The function must
        take two arguments, the request and response objects. The return value
        of the function must be the updated response object.

        Example::

            @app.route('/')
            def index(request):
                # register a request-specific after request handler
                @req.after_request
                def func(request, response):
                    # ...
                    return response

                return 'Hello, World!'

        Note that the function is not called if the request handler raises an
        exception and an error response is returned instead.
        """
        self.after_request_handlers.append(f)
        return f

    @staticmethod
    async def _safe_readline(stream):
        line = (await stream.readline())
        if len(line) > Request.max_readline:
            raise ValueError('line too long')
        return line


class Response:
    """An HTTP response class.

    :param body: The body of the response. If a dictionary or list is given,
                 a JSON formatter is used to generate the body. If a file-like
                 object or an async generator is given, a streaming response is
                 used. If a string is given, it is encoded from UTF-8. Else,
                 the body should be a byte sequence.
    :param status_code: The numeric HTTP status code of the response. The
                        default is 200.
    :param headers: A dictionary of headers to include in the response.
    :param reason: A custom reason phrase to add after the status code. The
                   default is "OK" for responses with a 200 status code and
                   "N/A" for any other status codes.
    """
    types_map = {
        'css': 'text/css',
        'gif': 'image/gif',
        'html': 'text/html',
        'jpg': 'image/jpeg',
        'js': 'application/javascript',
        'json': 'application/json',
        'png': 'image/png',
        'txt': 'text/plain',
    }

    send_file_buffer_size = 1024

    #: The content type to use for responses that do not explicitly define a
    #: ``Content-Type`` header.
    default_content_type = 'text/plain'

    #: The default cache control max age used by :meth:`send_file`. A value
    #: of ``None`` means that no ``Cache-Control`` header is added.
    default_send_file_max_age = None

    #: Special response used to signal that a response does not need to be
    #: written to the client. Used to exit WebSocket connections cleanly.
    already_handled = None

    def __init__(self, body='', status_code=200, headers=None, reason=None):
        if body is None and status_code == 200:
            body = ''
            status_code = 204
        self.status_code = status_code
        self.headers = NoCaseDict(headers or {})
        self.reason = reason
        if isinstance(body, (dict, list)):
            self.body = json.dumps(body).encode()
            self.headers['Content-Type'] = 'application/json; charset=UTF-8'
        elif isinstance(body, str):
            self.body = body.encode()
        else:
            # this applies to bytes, file-like objects or generators
            self.body = body
        self.is_head = False

    def set_cookie(self, cookie, value, path=None, domain=None, expires=None,
                   max_age=None, secure=False, http_only=False,
                   partitioned=False):
        """Add a cookie to the response.

        :param cookie: The cookie's name.
        :param value: The cookie's value.
        :param path: The cookie's path.
        :param domain: The cookie's domain.
        :param expires: The cookie expiration time, as a ``datetime`` object
                        or a correctly formatted string.
        :param max_age: The cookie's ``Max-Age`` value.
        :param secure: The cookie's ``secure`` flag.
        :param http_only: The cookie's ``HttpOnly`` flag.
        :param partitioned: Whether the cookie is partitioned.
        """
        http_cookie = '{cookie}={value}'.format(cookie=cookie, value=value)
        if path:
            http_cookie += '; Path=' + path
        if domain:
            http_cookie += '; Domain=' + domain
        if expires:
            if isinstance(expires, str):
                http_cookie += '; Expires=' + expires
            else:  # pragma: no cover
                http_cookie += '; Expires=' + time.strftime(
                    '%a, %d %b %Y %H:%M:%S GMT', expires.timetuple())
        if max_age is not None:
            http_cookie += '; Max-Age=' + str(max_age)
        if secure:
            http_cookie += '; Secure'
        if http_only:
            http_cookie += '; HttpOnly'
        if partitioned:
            http_cookie += '; Partitioned'
        if 'Set-Cookie' in self.headers:
            self.headers['Set-Cookie'].append(http_cookie)
        else:
            self.headers['Set-Cookie'] = [http_cookie]

    def delete_cookie(self, cookie, **kwargs):
        """Delete a cookie.

        :param cookie: The cookie's name.
        :param kwargs: Any cookie opens and flags supported by
                       ``set_cookie()`` except ``expires`` and ``max_age``.
        """
        self.set_cookie(cookie, '', expires='Thu, 01 Jan 1970 00:00:01 GMT',
                        max_age=0, **kwargs)

    def complete(self):
        if isinstance(self.body, bytes) and \
                'Content-Length' not in self.headers:
            self.headers['Content-Length'] = str(len(self.body))
        if 'Content-Type' not in self.headers:
            self.headers['Content-Type'] = self.default_content_type
            if 'charset=' not in self.headers['Content-Type']:
                self.headers['Content-Type'] += '; charset=UTF-8'

    async def write(self, stream):
        self.complete()

        try:
            # status code
            reason = self.reason if self.reason is not None else \
                ('OK' if self.status_code == 200 else 'N/A')
            await stream.awrite('HTTP/1.0 {status_code} {reason}\r\n'.format(
                status_code=self.status_code, reason=reason).encode())

            # headers
            for header, value in self.headers.items():
                values = value if isinstance(value, list) else [value]
                for value in values:
                    await stream.awrite('{header}: {value}\r\n'.format(
                        header=header, value=value).encode())
            await stream.awrite(b'\r\n')

            # body
            if not self.is_head:
                iter = self.body_iter()
                async for body in iter:
                    if isinstance(body, str):  # pragma: no cover
                        body = body.encode()
                    try:
                        await stream.awrite(body)
                    except OSError as exc:  # pragma: no cover
                        if exc.errno in MUTED_SOCKET_ERRORS or \
                                exc.args[0] == 'Connection lost':
                            if hasattr(iter, 'aclose'):
                                await iter.aclose()
                        raise
                if hasattr(iter, 'aclose'):  # pragma: no branch
                    await iter.aclose()

        except OSError as exc:  # pragma: no cover
            if exc.errno in MUTED_SOCKET_ERRORS or \
                    exc.args[0] == 'Connection lost':
                pass
            else:
                raise

    def body_iter(self):
        if hasattr(self.body, '__anext__'):
            # response body is an async generator
            return self.body

        response = self

        class iter:
            ITER_UNKNOWN = 0
            ITER_SYNC_GEN = 1
            ITER_FILE_OBJ = 2
            ITER_NO_BODY = -1

            def __aiter__(self):
                if response.body:
                    self.i = self.ITER_UNKNOWN  # need to determine type
                else:
                    self.i = self.ITER_NO_BODY
                return self

            async def __anext__(self):
                if self.i == self.ITER_NO_BODY:
                    await self.aclose()
                    raise StopAsyncIteration
                if self.i == self.ITER_UNKNOWN:
                    if hasattr(response.body, 'read'):
                        self.i = self.ITER_FILE_OBJ
                    elif hasattr(response.body, '__next__'):
                        self.i = self.ITER_SYNC_GEN
                        return next(response.body)
                    else:
                        self.i = self.ITER_NO_BODY
                        return response.body
                elif self.i == self.ITER_SYNC_GEN:
                    try:
                        return next(response.body)
                    except StopIteration:
                        await self.aclose()
                        raise StopAsyncIteration
                buf = response.body.read(response.send_file_buffer_size)
                if iscoroutine(buf):  # pragma: no cover
                    buf = await buf
                if len(buf) < response.send_file_buffer_size:
                    self.i = self.ITER_NO_BODY
                return buf

            async def aclose(self):
                if hasattr(response.body, 'close'):
                    result = response.body.close()
                    if iscoroutine(result):  # pragma: no cover
                        await result

        return iter()

    @classmethod
    def redirect(cls, location, status_code=302):
        """Return a redirect response.

        :param location: The URL to redirect to.
        :param status_code: The 3xx status code to use for the redirect. The
                            default is 302.
        """
        if '\x0d' in location or '\x0a' in location:
            raise ValueError('invalid redirect URL')
        return cls(status_code=status_code, headers={'Location': location})

    @classmethod
    def send_file(cls, filename, status_code=200, content_type=None,
                  stream=None, max_age=None, compressed=False,
                  file_extension=''):
        """Send file contents in a response.

        :param filename: The filename of the file.
        :param status_code: The 3xx status code to use for the redirect. The
                            default is 302.
        :param content_type: The ``Content-Type`` header to use in the
                             response. If omitted, it is generated
                             automatically from the file extension of the
                             ``filename`` parameter.
        :param stream: A file-like object to read the file contents from. If
                       a stream is given, the ``filename`` parameter is only
                       used when generating the ``Content-Type`` header.
        :param max_age: The ``Cache-Control`` header's ``max-age`` value in
                        seconds. If omitted, the value of the
                        :attr:`Response.default_send_file_max_age` attribute is
                        used.
        :param compressed: Whether the file is compressed. If ``True``, the
                           ``Content-Encoding`` header is set to ``gzip``. A
                           string with the header value can also be passed.
                           Note that when using this option the file must have
                           been compressed beforehand. This option only sets
                           the header.
        :param file_extension: A file extension to append to the ``filename``
                               parameter when opening the file, including the
                               dot. The extension given here is not considered
                               when generating the ``Content-Type`` header.

        Security note: The filename is assumed to be trusted. Never pass
        filenames provided by the user without validating and sanitizing them
        first.
        """
        if content_type is None:
            ext = filename.split('.')[-1]
            if ext in Response.types_map:
                content_type = Response.types_map[ext]
            else:
                content_type = 'application/octet-stream'
        headers = {'Content-Type': content_type}

        if max_age is None:
            max_age = cls.default_send_file_max_age
        if max_age is not None:
            headers['Cache-Control'] = 'max-age={}'.format(max_age)

        if compressed:
            headers['Content-Encoding'] = compressed \
                if isinstance(compressed, str) else 'gzip'

        f = stream or open(filename + file_extension, 'rb')
        return cls(body=f, status_code=status_code, headers=headers)


class URLPattern():
    def __init__(self, url_pattern):
        self.url_pattern = url_pattern
        self.segments = []
        self.regex = None
        pattern = ''
        use_regex = False
        for segment in url_pattern.lstrip('/').split('/'):
            if segment and segment[0] == '<':
                if segment[-1] != '>':
                    raise ValueError('invalid URL pattern')
                segment = segment[1:-1]
                if ':' in segment:
                    type_, name = segment.rsplit(':', 1)
                else:
                    type_ = 'string'
                    name = segment
                parser = None
                if type_ == 'string':
                    parser = self._string_segment
                    pattern += '/([^/]+)'
                elif type_ == 'int':
                    parser = self._int_segment
                    pattern += '/(-?\\d+)'
                elif type_ == 'path':
                    use_regex = True
                    pattern += '/(.+)'
                elif type_.startswith('re:'):
                    use_regex = True
                    pattern += '/({pattern})'.format(pattern=type_[3:])
                else:
                    raise ValueError('invalid URL segment type')
                self.segments.append({'parser': parser, 'name': name,
                                      'type': type_})
            else:
                pattern += '/' + segment
                self.segments.append({'parser': self._static_segment(segment)})
        if use_regex:
            import re
            self.regex = re.compile('^' + pattern + '$')

    def match(self, path):
        args = {}
        if self.regex:
            g = self.regex.match(path)
            if not g:
                return
            i = 1
            for segment in self.segments:
                if 'name' not in segment:
                    continue
                value = g.group(i)
                if segment['type'] == 'int':
                    value = int(value)
                args[segment['name']] = value
                i += 1
        else:
            if len(path) == 0 or path[0] != '/':
                return
            path = path[1:]
            args = {}
            for segment in self.segments:
                if path is None:
                    return
                arg, path = segment['parser'](path)
                if arg is None:
                    return
                if 'name' in segment:
                    args[segment['name']] = arg
            if path is not None:
                return
        return args

    def _static_segment(self, segment):
        def _static(value):
            s = value.split('/', 1)
            if s[0] == segment:
                return '', s[1] if len(s) > 1 else None
            return None, None
        return _static

    def _string_segment(self, value):
        s = value.split('/', 1)
        if len(s[0]) == 0:
            return None, None
        return s[0], s[1] if len(s) > 1 else None

    def _int_segment(self, value):
        s = value.split('/', 1)
        try:
            return int(s[0]), s[1] if len(s) > 1 else None
        except ValueError:
            return None, None


class HTTPException(Exception):
    def __init__(self, status_code, reason=None):
        self.status_code = status_code
        self.reason = reason or str(status_code) + ' error'

    def __repr__(self):  # pragma: no cover
        return 'HTTPException: {}'.format(self.status_code)


class Microdot:
    """An HTTP application class.

    This class implements an HTTP application instance and is heavily
    influenced by the ``Flask`` class of the Flask framework. It is typically
    declared near the start of the main application script.

    Example::

        from microdot import Microdot

        app = Microdot()
    """

    def __init__(self):
        self.url_map = []
        self.before_request_handlers = []
        self.after_request_handlers = []
        self.after_error_request_handlers = []
        self.error_handlers = {}
        self.shutdown_requested = False
        self.options_handler = self.default_options_handler
        self.debug = False
        self.server = None

    def route(self, url_pattern, methods=None):
        """Decorator that is used to register a function as a request handler
        for a given URL.

        :param url_pattern: The URL pattern that will be compared against
                            incoming requests.
        :param methods: The list of HTTP methods to be handled by the
                        decorated function. If omitted, only ``GET`` requests
                        are handled.

        The URL pattern can be a static path (for example, ``/users`` or
        ``/api/invoices/search``) or a path with dynamic components enclosed
        in ``<`` and ``>`` (for example, ``/users/<id>`` or
        ``/invoices/<number>/products``). Dynamic path components can also
        include a type prefix, separated from the name with a colon (for
        example, ``/users/<int:id>``). The type can be ``string`` (the
        default), ``int``, ``path`` or ``re:[regular-expression]``.

        The first argument of the decorated function must be
        the request object. Any path arguments that are specified in the URL
        pattern are passed as keyword arguments. The return value of the
        function must be a :class:`Response` instance, or the arguments to
        be passed to this class.

        Example::

            @app.route('/')
            def index(request):
                return 'Hello, world!'
        """
        def decorated(f):
            self.url_map.append(
                ([m.upper() for m in (methods or ['GET'])],
                 URLPattern(url_pattern), f))
            return f
        return decorated

    def get(self, url_pattern):
        """Decorator that is used to register a function as a ``GET`` request
        handler for a given URL.

        :param url_pattern: The URL pattern that will be compared against
                            incoming requests.

        This decorator can be used as an alias to the ``route`` decorator with
        ``methods=['GET']``.

        Example::

            @app.get('/users/<int:id>')
            def get_user(request, id):
                # ...
        """
        return self.route(url_pattern, methods=['GET'])

    def post(self, url_pattern):
        """Decorator that is used to register a function as a ``POST`` request
        handler for a given URL.

        :param url_pattern: The URL pattern that will be compared against
                            incoming requests.

        This decorator can be used as an alias to the``route`` decorator with
        ``methods=['POST']``.

        Example::

            @app.post('/users')
            def create_user(request):
                # ...
        """
        return self.route(url_pattern, methods=['POST'])

    def put(self, url_pattern):
        """Decorator that is used to register a function as a ``PUT`` request
        handler for a given URL.

        :param url_pattern: The URL pattern that will be compared against
                            incoming requests.

        This decorator can be used as an alias to the ``route`` decorator with
        ``methods=['PUT']``.

        Example::

            @app.put('/users/<int:id>')
            def edit_user(request, id):
                # ...
        """
        return self.route(url_pattern, methods=['PUT'])

    def patch(self, url_pattern):
        """Decorator that is used to register a function as a ``PATCH`` request
        handler for a given URL.

        :param url_pattern: The URL pattern that will be compared against
                            incoming requests.

        This decorator can be used as an alias to the ``route`` decorator with
        ``methods=['PATCH']``.

        Example::

            @app.patch('/users/<int:id>')
            def edit_user(request, id):
                # ...
        """
        return self.route(url_pattern, methods=['PATCH'])

    def delete(self, url_pattern):
        """Decorator that is used to register a function as a ``DELETE``
        request handler for a given URL.

        :param url_pattern: The URL pattern that will be compared against
                            incoming requests.

        This decorator can be used as an alias to the ``route`` decorator with
        ``methods=['DELETE']``.

        Example::

            @app.delete('/users/<int:id>')
            def delete_user(request, id):
                # ...
        """
        return self.route(url_pattern, methods=['DELETE'])

    def before_request(self, f):
        """Decorator to register a function to run before each request is
        handled. The decorated function must take a single argument, the
        request object.

        Example::

            @app.before_request
            def func(request):
                # ...
        """
        self.before_request_handlers.append(f)
        return f

    def after_request(self, f):
        """Decorator to register a function to run after each request is
        handled. The decorated function must take two arguments, the request
        and response objects. The return value of the function must be an
        updated response object.

        Example::

            @app.after_request
            def func(request, response):
                # ...
                return response
        """
        self.after_request_handlers.append(f)
        return f

    def after_error_request(self, f):
        """Decorator to register a function to run after an error response is
        generated. The decorated function must take two arguments, the request
        and response objects. The return value of the function must be an
        updated response object. The handler is invoked for error responses
        generated by Microdot, as well as those returned by application-defined
        error handlers.

        Example::

            @app.after_error_request
            def func(request, response):
                # ...
                return response
        """
        self.after_error_request_handlers.append(f)
        return f

    def errorhandler(self, status_code_or_exception_class):
        """Decorator to register a function as an error handler. Error handler
        functions for numeric HTTP status codes must accept a single argument,
        the request object. Error handler functions for Python exceptions
        must accept two arguments, the request object and the exception
        object.

        :param status_code_or_exception_class: The numeric HTTP status code or
                                               Python exception class to
                                               handle.

        Examples::

            @app.errorhandler(404)
            def not_found(request):
                return 'Not found'

            @app.errorhandler(RuntimeError)
            def runtime_error(request, exception):
                return 'Runtime error'
        """
        def decorated(f):
            self.error_handlers[status_code_or_exception_class] = f
            return f
        return decorated

    def mount(self, subapp, url_prefix=''):
        """Mount a sub-application, optionally under the given URL prefix.

        :param subapp: The sub-application to mount.
        :param url_prefix: The URL prefix to mount the application under.
        """
        for methods, pattern, handler in subapp.url_map:
            self.url_map.append(
                (methods, URLPattern(url_prefix + pattern.url_pattern),
                 handler))
        for handler in subapp.before_request_handlers:
            self.before_request_handlers.append(handler)
        for handler in subapp.after_request_handlers:
            self.after_request_handlers.append(handler)
        for handler in subapp.after_error_request_handlers:
            self.after_error_request_handlers.append(handler)
        for status_code, handler in subapp.error_handlers.items():
            self.error_handlers[status_code] = handler

    @staticmethod
    def abort(status_code, reason=None):
        """Abort the current request and return an error response with the
        given status code.

        :param status_code: The numeric status code of the response.
        :param reason: The reason for the response, which is included in the
                       response body.

        Example::

            from microdot import abort

            @app.route('/users/<int:id>')
            def get_user(id):
                user = get_user_by_id(id)
                if user is None:
                    abort(404)
                return user.to_dict()
        """
        raise HTTPException(status_code, reason)

    async def start_server(self, host='0.0.0.0', port=5000, debug=False,
                           ssl=None):
        """Start the Microdot web server as a coroutine. This coroutine does
        not normally return, as the server enters an endless listening loop.
        The :func:`shutdown` function provides a method for terminating the
        server gracefully.

        :param host: The hostname or IP address of the network interface that
                     will be listening for requests. A value of ``'0.0.0.0'``
                     (the default) indicates that the server should listen for
                     requests on all the available interfaces, and a value of
                     ``127.0.0.1`` indicates that the server should listen
                     for requests only on the internal networking interface of
                     the host.
        :param port: The port number to listen for requests. The default is
                     port 5000.
        :param debug: If ``True``, the server logs debugging information. The
                      default is ``False``.
        :param ssl: An ``SSLContext`` instance or ``None`` if the server should
                    not use TLS. The default is ``None``.

        This method is a coroutine.

        Example::

            import asyncio
            from microdot import Microdot

            app = Microdot()

            @app.route('/')
            async def index(request):
                return 'Hello, world!'

            async def main():
                await app.start_server(debug=True)

            asyncio.run(main())
        """
        self.debug = debug

        async def serve(reader, writer):
            if not hasattr(writer, 'awrite'):  # pragma: no cover
                # CPython provides the awrite and aclose methods in 3.8+
                async def awrite(self, data):
                    self.write(data)
                    await self.drain()

                async def aclose(self):
                    self.close()
                    await self.wait_closed()

                from types import MethodType
                writer.awrite = MethodType(awrite, writer)
                writer.aclose = MethodType(aclose, writer)

            await self.handle_request(reader, writer)

        if self.debug:  # pragma: no cover
            print('Starting async server on {host}:{port}...'.format(
                host=host, port=port))

        try:
            self.server = await asyncio.start_server(serve, host, port,
                                                     ssl=ssl)
        except TypeError:  # pragma: no cover
            self.server = await asyncio.start_server(serve, host, port)

        while True:
            try:
                if hasattr(self.server, 'serve_forever'):  # pragma: no cover
                    try:
                        await self.server.serve_forever()
                    except asyncio.CancelledError:
                        pass
                await self.server.wait_closed()
                break
            except AttributeError:  # pragma: no cover
                # the task hasn't been initialized in the server object yet
                # wait a bit and try again
                await asyncio.sleep(0.1)

    def run(self, host='0.0.0.0', port=5000, debug=False, ssl=None):
        """Start the web server. This function does not normally return, as
        the server enters an endless listening loop. The :func:`shutdown`
        function provides a method for terminating the server gracefully.

        :param host: The hostname or IP address of the network interface that
                     will be listening for requests. A value of ``'0.0.0.0'``
                     (the default) indicates that the server should listen for
                     requests on all the available interfaces, and a value of
                     ``127.0.0.1`` indicates that the server should listen
                     for requests only on the internal networking interface of
                     the host.
        :param port: The port number to listen for requests. The default is
                     port 5000.
        :param debug: If ``True``, the server logs debugging information. The
                      default is ``False``.
        :param ssl: An ``SSLContext`` instance or ``None`` if the server should
                    not use TLS. The default is ``None``.

        Example::

            from microdot import Microdot

            app = Microdot()

            @app.route('/')
            async def index(request):
                return 'Hello, world!'

            app.run(debug=True)
        """
        asyncio.run(self.start_server(host=host, port=port, debug=debug,
                                      ssl=ssl))  # pragma: no cover

    def shutdown(self):
        """Request a server shutdown. The server will then exit its request
        listening loop and the :func:`run` function will return. This function
        can be safely called from a route handler, as it only schedules the
        server to terminate as soon as the request completes.

        Example::

            @app.route('/shutdown')
            def shutdown(request):
                request.app.shutdown()
                return 'The server is shutting down...'
        """
        self.server.close()

    def find_route(self, req):
        method = req.method.upper()
        if method == 'OPTIONS' and self.options_handler:
            return self.options_handler(req)
        if method == 'HEAD':
            method = 'GET'
        f = 404
        for route_methods, route_pattern, route_handler in self.url_map:
            req.url_args = route_pattern.match(req.path)
            if req.url_args is not None:
                if method in route_methods:
                    f = route_handler
                    break
                else:
                    f = 405
        return f

    def default_options_handler(self, req):
        allow = []
        for route_methods, route_pattern, route_handler in self.url_map:
            if route_pattern.match(req.path) is not None:
                allow.extend(route_methods)
        if 'GET' in allow:
            allow.append('HEAD')
        allow.append('OPTIONS')
        return {'Allow': ', '.join(allow)}

    async def handle_request(self, reader, writer):
        req = None
        try:
            req = await Request.create(self, reader, writer,
                                       writer.get_extra_info('peername'))
        except Exception as exc:  # pragma: no cover
            print_exception(exc)

        res = await self.dispatch_request(req)
        if res != Response.already_handled:  # pragma: no branch
            await res.write(writer)
        try:
            await writer.aclose()
        except OSError as exc:  # pragma: no cover
            if exc.errno in MUTED_SOCKET_ERRORS:
                pass
            else:
                raise
        if self.debug and req:  # pragma: no cover
            print('{method} {path} {status_code}'.format(
                method=req.method, path=req.path,
                status_code=res.status_code))

    async def dispatch_request(self, req):
        after_request_handled = False
        if req:
            if req.content_length > req.max_content_length:
                if 413 in self.error_handlers:
                    res = await invoke_handler(self.error_handlers[413], req)
                else:
                    res = 'Payload too large', 413
            else:
                f = self.find_route(req)
                try:
                    res = None
                    if callable(f):
                        for handler in self.before_request_handlers:
                            res = await invoke_handler(handler, req)
                            if res:
                                break
                        if res is None:
                            res = await invoke_handler(
                                f, req, **req.url_args)
                        if isinstance(res, tuple):
                            body = res[0]
                            if isinstance(res[1], int):
                                status_code = res[1]
                                headers = res[2] if len(res) > 2 else {}
                            else:
                                status_code = 200
                                headers = res[1]
                            res = Response(body, status_code, headers)
                        elif not isinstance(res, Response):
                            res = Response(res)
                        for handler in self.after_request_handlers:
                            res = await invoke_handler(
                                handler, req, res) or res
                        for handler in req.after_request_handlers:
                            res = await invoke_handler(
                                handler, req, res) or res
                        after_request_handled = True
                    elif isinstance(f, dict):
                        res = Response(headers=f)
                    elif f in self.error_handlers:
                        res = await invoke_handler(self.error_handlers[f], req)
                    else:
                        res = 'Not found', f
                except HTTPException as exc:
                    if exc.status_code in self.error_handlers:
                        res = self.error_handlers[exc.status_code](req)
                    else:
                        res = exc.reason, exc.status_code
                except Exception as exc:
                    print_exception(exc)
                    exc_class = None
                    res = None
                    if exc.__class__ in self.error_handlers:
                        exc_class = exc.__class__
                    else:
                        for c in mro(exc.__class__)[1:]:
                            if c in self.error_handlers:
                                exc_class = c
                                break
                    if exc_class:
                        try:
                            res = await invoke_handler(
                                self.error_handlers[exc_class], req, exc)
                        except Exception as exc2:  # pragma: no cover
                            print_exception(exc2)
                    if res is None:
                        if 500 in self.error_handlers:
                            res = await invoke_handler(
                                self.error_handlers[500], req)
                        else:
                            res = 'Internal server error', 500
        else:
            if 400 in self.error_handlers:
                res = await invoke_handler(self.error_handlers[400], req)
            else:
                res = 'Bad request', 400
        if isinstance(res, tuple):
            res = Response(*res)
        elif not isinstance(res, Response):
            res = Response(res)
        if not after_request_handled:
            for handler in self.after_error_request_handlers:
                res = await invoke_handler(
                    handler, req, res) or res
        res.is_head = (req and req.method == 'HEAD')
        return res


Response.already_handled = Response()

abort = Microdot.abort
redirect = Response.redirect
send_file = Response.send_file