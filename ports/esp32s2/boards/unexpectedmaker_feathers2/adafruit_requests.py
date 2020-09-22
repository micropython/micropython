# The MIT License (MIT)
#
# Copyright (c) 2019 ladyada for Adafruit Industries
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
"""
`adafruit_requests`
================================================================================

A requests-like library for web interfacing


* Author(s): ladyada, Paul Sokolovsky

Implementation Notes
--------------------

Adapted from https://github.com/micropython/micropython-lib/tree/master/urequests

micropython-lib consists of multiple modules from different sources and
authors. Each module comes under its own licensing terms. Short name of
a license can be found in a file within a module directory (usually
metadata.txt or setup.py). Complete text of each license used is provided
at https://github.com/micropython/micropython-lib/blob/master/LICENSE

author='Paul Sokolovsky'
license='MIT'

**Software and Dependencies:**

* Adafruit CircuitPython firmware for the supported boards:
  https://github.com/adafruit/circuitpython/releases

"""

import gc

__version__ = "0.0.0-auto.0"
__repo__ = "https://github.com/adafruit/Adafruit_CircuitPython_Requests.git"

_the_interface = None  # pylint: disable=invalid-name
_the_sock = None  # pylint: disable=invalid-name


def set_socket(sock, iface=None):
    """Helper to set the global socket and optionally set the global network interface.
    :param sock: socket object.
    :param iface: internet interface object

    """
    global _the_sock  # pylint: disable=invalid-name, global-statement
    _the_sock = sock
    if iface:
        global _the_interface  # pylint: disable=invalid-name, global-statement
        _the_interface = iface
        _the_sock.set_interface(iface)


class Response:
    """The response from a request, contains all the headers/content"""

    encoding = None

    def __init__(self, sock):
        self.socket = sock
        self.encoding = "utf-8"
        self._cached = None
        self.status_code = None
        self.reason = None
        self._read_so_far = 0
        self.headers = {}

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def close(self):
        """Close, delete and collect the response data"""
        if self.socket:
            self.socket.close()
            del self.socket
        del self._cached
        gc.collect()

    @property
    def content(self):
        """The HTTP content direct from the socket, as bytes"""
        # print(self.headers)
        try:
            content_length = int(self.headers["content-length"])
        except KeyError:
            content_length = 0
        # print("Content length:", content_length)
        if self._cached is None:
            try:
                self._cached = self.socket.recv(content_length)
            finally:
                self.socket.close()
                self.socket = None
        # print("Buffer length:", len(self._cached))
        return self._cached

    @property
    def text(self):
        """The HTTP content, encoded into a string according to the HTTP
        header encoding"""
        return str(self.content, self.encoding)

    def json(self):
        """The HTTP content, parsed into a json dictionary"""
        # pylint: disable=import-outside-toplevel
        try:
            import json as json_module
        except ImportError:
            import ujson as json_module
        return json_module.loads(self.content)

    def iter_content(self, chunk_size=1, decode_unicode=False):
        """An iterator that will stream data by only reading 'chunk_size'
        bytes and yielding them, when we can't buffer the whole datastream"""
        if decode_unicode:
            raise NotImplementedError("Unicode not supported")

        while True:
            chunk = self.socket.recv(chunk_size)
            if chunk:
                yield chunk
            else:
                return


# pylint: disable=too-many-branches, too-many-statements, unused-argument, too-many-arguments, too-many-locals
def request(method, url, data=None, json=None, headers=None, stream=False, timeout=1):
    """Perform an HTTP request to the given url which we will parse to determine
    whether to use SSL ('https://') or not. We can also send some provided 'data'
    or a json dictionary which we will stringify. 'headers' is optional HTTP headers
    sent along. 'stream' will determine if we buffer everything, or whether to only
    read only when requested
    """
    global _the_interface  # pylint: disable=global-statement, invalid-name
    global _the_sock  # pylint: disable=global-statement, invalid-name

    if not headers:
        headers = {}

    try:
        proto, dummy, host, path = url.split("/", 3)
        # replace spaces in path
        path = path.replace(" ", "%20")
    except ValueError:
        proto, dummy, host = url.split("/", 2)
        path = ""
    if proto == "http:":
        port = 80
    elif proto == "https:":
        port = 443
    else:
        raise ValueError("Unsupported protocol: " + proto)

    if ":" in host:
        host, port = host.split(":", 1)
        port = int(port)

    addr_info = _the_sock.getaddrinfo(host, port, 0, _the_sock.SOCK_STREAM)[0]
    sock = _the_sock.socket(addr_info[0], addr_info[1], addr_info[2])
    resp = Response(sock)  # our response

    sock.settimeout(timeout)  # socket read timeout

    try:
        if proto == "https:":
            conntype = _the_interface.TLS_MODE
            sock.connect(
                (host, port), conntype
            )  # for SSL we need to know the host name
        else:
            conntype = _the_interface.TCP_MODE
            sock.connect(addr_info[-1], conntype)
        sock.send(
            b"%s /%s HTTP/1.0\r\n" % (bytes(method, "utf-8"), bytes(path, "utf-8"))
        )
        if "Host" not in headers:
            sock.send(b"Host: %s\r\n" % bytes(host, "utf-8"))
        if "User-Agent" not in headers:
            sock.send(b"User-Agent: Adafruit CircuitPython\r\n")
        # Iterate over keys to avoid tuple alloc
        for k in headers:
            sock.send(k.encode())
            sock.send(b": ")
            sock.send(headers[k].encode())
            sock.send(b"\r\n")
        if json is not None:
            assert data is None
            # pylint: disable=import-outside-toplevel
            try:
                import json as json_module
            except ImportError:
                import ujson as json_module
            # pylint: enable=import-outside-toplevel
            data = json_module.dumps(json)
            sock.send(b"Content-Type: application/json\r\n")
        if data:
            if isinstance(data, dict):
                sock.send(b"Content-Type: application/x-www-form-urlencoded\r\n")
                _post_data = ""
                for k in data:
                    _post_data = "{}&{}={}".format(_post_data, k, data[k])
                data = _post_data[1:]
            sock.send(b"Content-Length: %d\r\n" % len(data))
        sock.send(b"\r\n")
        if data:
            if isinstance(data, bytearray):
                sock.send(bytes(data))
            else:
                sock.send(bytes(data, "utf-8"))

        line = sock.readline()
        # print(line)
        line = line.split(None, 2)
        status = int(line[1])
        reason = ""
        if len(line) > 2:
            reason = line[2].rstrip()
        resp.headers = parse_headers(sock)
        if resp.headers.get("transfer-encoding"):
            if "chunked" in resp.headers.get("transfer-encoding"):
                raise ValueError("Unsupported " + resp.headers.get("transfer-encoding"))
        elif resp.headers.get("location") and not 200 <= status <= 299:
            raise NotImplementedError("Redirects not yet supported")

    except:
        sock.close()
        raise

    resp.status_code = status
    resp.reason = reason
    return resp


def parse_headers(sock):
    """
    Parses the header portion of an HTTP request/response from the socket.
    Expects first line of HTTP request/response to have been read already
    return: header dictionary
    rtype: Dict
    """
    headers = {}
    while True:
        line = sock.readline()
        if not line or line == b"\r\n":
            break

        # print("**line: ", line)
        splits = line.split(b": ", 1)
        title = splits[0]
        content = ""
        if len(splits) > 1:
            content = splits[1]
        if title and content:
            title = str(title.lower(), "utf-8")
            content = str(content, "utf-8")
            headers[title] = content
    return headers


def head(url, **kw):
    """Send HTTP HEAD request"""
    return request("HEAD", url, **kw)


def get(url, **kw):
    """Send HTTP GET request"""
    return request("GET", url, **kw)


def post(url, **kw):
    """Send HTTP POST request"""
    return request("POST", url, **kw)


def put(url, **kw):
    """Send HTTP PUT request"""
    return request("PUT", url, **kw)


def patch(url, **kw):
    """Send HTTP PATCH request"""
    return request("PATCH", url, **kw)


def delete(url, **kw):
    """Send HTTP DELETE request"""
    return request("DELETE", url, **kw)
