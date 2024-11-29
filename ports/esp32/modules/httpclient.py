import usocket, os
class Response:

    def __init__(self, socket, saveToFile=None):
        self._socket = socket
        self._saveToFile = saveToFile
        self._encoding = 'utf-8'
        if saveToFile is not None:
            CHUNK_SIZE = 512 # bytes
            with open(saveToFile, 'w') as outfile:
                data = self._socket.read(CHUNK_SIZE)
                while data:
                    outfile.write(data)
                    data = self._socket.read(CHUNK_SIZE)
                outfile.close()

            self.close()

    def close(self):
        if self._socket:
            self._socket.close()
            self._socket = None

    @property
    def content(self):
        if self._saveToFile is not None:
            raise SystemError('You cannot get the content from the response as you decided to save it in {}'.format(self._saveToFile))

        try:
            result = self._socket.read()
            return result
        finally:
            self.close()

    @property
    def text(self):
        return str(self.content, self._encoding)

    def json(self):
        try:
            import ujson
            result = ujson.load(self._socket)
            return result
        finally:
            self.close()


class HttpClient:

    def __init__(self, headers={}):
        self._headers = headers

    def request(self, method, url, data=None, json=None, file=None, custom=None, saveToFile=None, headers={}, stream=None):
        def _write_headers(sock, _headers):
            for k in _headers:
                sock.write(b'{}: {}\r\n'.format(k, _headers[k]))

        try:
            proto, dummy, host, path = url.split('/', 3)
        except ValueError:
            proto, dummy, host = url.split('/', 2)
            path = ''
        if proto == 'http:':
            port = 80
        elif proto == 'https:':
            import ssl as ussl
            port = 443
        else:
            raise ValueError('Unsupported protocol: ' + proto)

        if ':' in host:
            host, port = host.split(':', 1)
            port = int(port)

        ai = usocket.getaddrinfo(host, port, 0, usocket.SOCK_STREAM)
        if len(ai) < 1:
            raise ValueError('You are not connected to the internet...')
        ai = ai[0]

        s = usocket.socket(ai[0], ai[1], ai[2])
        try:
            s.connect(ai[-1])
            if proto == 'https:':
                s = ussl.wrap_socket(s, server_hostname=host)
            s.write(b'%s /%s HTTP/1.0\r\n' % (method, path))
            if not 'Host' in headers:
                s.write(b'Host: %s\r\n' % host)
            # Iterate over keys to avoid tuple alloc
            _write_headers(s, self._headers)
            _write_headers(s, headers)

            # add user agent
            s.write(b'User-Agent: MicroPython Client\r\n')
            if json is not None:
                assert data is None
                import ujson
                data = ujson.dumps(json)
                s.write(b'Content-Type: application/json\r\n')

            if data:
                s.write(b'Content-Length: %d\r\n' % len(data))
                s.write(b'\r\n')
                s.write(data)
            elif file:
                s.write(b'Content-Length: %d\r\n' % os.stat(file)[6])
                s.write(b'\r\n')
                with open(file, 'r') as file_object:
                    for line in file_object:
                        s.write(line + '\n')
            elif custom:
                custom(s)
            else:
                s.write(b'\r\n')

            l = s.readline()
            # print(l)
            l = l.split(None, 2)
            status = int(l[1])
            reason = ''
            if len(l) > 2:
                reason = l[2].rstrip()
            while True:
                l = s.readline()
                if not l or l == b'\r\n':
                    break
                # print(l)
                if l.startswith(b'Transfer-Encoding:'):
                    if b'chunked' in l:
                        raise ValueError('Unsupported ' + l)
                elif l.startswith(b'Location:') and not 200 <= status <= 299:
                    raise NotImplementedError('Redirects not yet supported')
        except OSError:
            s.close()
            raise

        resp = Response(s, saveToFile)
        resp.status_code = status
        resp.reason = reason
        return resp

    def head(self, url, **kw):
        return self.request('HEAD', url, **kw)

    def get(self, url, **kw):
        return self.request('GET', url, **kw)

    def post(self, url, **kw):
        return self.request('POST', url, **kw)

    def put(self, url, **kw):
        return self.request('PUT', url, **kw)

    def patch(self, url, **kw):
        return self.request('PATCH', url, **kw)

    def delete(self, url, **kw):
        return self.request('DELETE', url, **kw)