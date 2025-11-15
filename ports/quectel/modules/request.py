# Copyright (c) Quectel Wireless Solution, Co., Ltd.All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import usocket
import ujson
import utime
import uio

import uos
name, platform = uos.uname()[1].split("=",1)

if platform != "FCM360W" and platform != "FCM362K":
    import net
    import dataCall


def parse_chunked_data(sock):
    data = b''
    while True:
        chunk_size_str = b''
        while True:
            char = sock.read(1)
            if char == b'\r':
                continue
            if char == b'\n':
                break
            chunk_size_str += char
        chunk_size = int(chunk_size_str, 16)
        if chunk_size == 0:
            break
        chunk_data = sock.read(chunk_size)
        data += chunk_data
        sock.read(2)  # discard \r\n
    return data

class Response:
    def __init__(self, f, decode=True, sizeof=4096, flag=False):
        self.raw = f
        self.encoding = "utf-8"
        self.decode = decode
        self.sizeof = sizeof
        self.read_len = 0
        self.read_data = sizeof
        self.flag = flag

    def close(self):
        if self.raw:
            if s_isopen:
                self.raw.close()
                self.raw = None
        if s_isopen:
            if self.raw:
                self.raw.close()

    @property
    def content(self):
        global s_isopen
        try:
            while True:
                if self.read_data > 2048:
                    block = self.raw.read(2048)
                    self.read_data -= 2048
                else:
                    block = self.raw.read(self.read_data)
                self.read_len += len(block)
                if block:
                    yield block.decode() if self.decode else block
                else:
                    self.raw.close()
                    s_isopen = False
                    break
                if (self.flag == True) and (self.sizeof == self.read_len):
                    self.raw.close()
                    s_isopen = False
                    break
        except Exception as e:
            self.raw.close() # 2021-05-27
            s_isopen = False
            if "ETIMEDOUT" in str(e):
                raise ValueError(str(e))
            else:
                pass
        return ""

    @property
    def text(self):
        for i in self.content:
            yield str(i)
        return ""

    def json(self):
        try:
            json_str = ""
            for i in self.content:
                json_str += i
            if json_str:
                return ujson.loads(json_str)
            else:
                return None
        except Exception as e:
            raise ValueError(
                "The data for the response cannot be converted to JSON-type data,please try use response.content method")


def request(method, url, data=None, json=None, files=None, stream=None, decode=True, sizeof=2048, timeout=20, headers=None,
            ssl_params=None, version=0, files_data=None,files_data_len=None,files_name=None, ipvtype=0,username=None,password=None):
    global port
    global s_isopen
    s_isopen = True
    port_exist = False
    addr = None
    URL = url
    if ipvtype == 1 and (url.split("[")[0] == "http://" or url.split("[")[0] == "https://"):
        try:
            proto, dummy, hosts, path = url.split("/", 3)
        except ValueError:
            proto, dummy, hosts = url.split("/", 2)
            path = ""
        
        left = hosts.find('[')
        if left < 0:
            raise ValueError("http missing left delimiter:" + url)

        right = hosts.find(']', left + 1)
        if right < 0:
            raise ValueError("http missing right delimiter:" + url)

        host = hosts[left + 1:right]

        try:
            if hosts[right + 1] == ':':
                port = int(hosts[right + 2:])
                port_exist = True
        except Exception as e:
            pass
            
        if proto == "http:":
            if not port_exist:
                port = 80
        elif proto == "https:":
            if not port_exist:
                port = 443
        else:
            raise ValueError("Unsupported protocol: " + proto)

        try:
            ai = usocket.getaddrinfo(host, port, 0, usocket.SOCK_STREAM)
        except Exception as e:
            raise IndexError("Domain name resolution error, please check network connection:" + host)

        import uos
        name, platform = uos.uname()[1].split("=",1)
        if platform == "FCM360W" or platform == "FCM362K":
            raise ValueError("platform Not Support IPV6!")
        for i in ai:
            if i[0] == 10:
                addr = i[-1]
                break
        host = hosts

        if addr is None:
            raise ValueError("Request DNS Parsing IPV6 Fail")
    elif not url.split(".")[0].isdigit():
        if not url.startswith("http"):
            url = "http://" + url
        try:
            proto, dummy, host, path = url.split("/", 3)
        except ValueError:
            proto, dummy, host = url.split("/", 2)
            path = ""
        if ":" in host:
            url_info = host.split(":")
            host = url_info[0]
            port = int(url_info[1])
            port_exist = True
        # jian.yao 2020-12-09
        if proto == "http:":
            if not port_exist:
                port = 80
        # jian.yao 2020-12-09
        elif proto == "https:":
            if not port_exist:
                port = 443
        else:
            raise ValueError("Unsupported protocol: " + proto)

        try:
            ai = usocket.getaddrinfo(host, port, 0, usocket.SOCK_STREAM)
        except Exception as e:
            raise IndexError("Domain name resolution error, please check network connection")

        if ipvtype == 1:
            import uos
            name, platform = uos.uname()[1].split("=",1)
            if platform == "FCM360W" or platform == "FCM362K":
                raise ValueError("platform Not Support IPV6!")
            for i in ai:
                if i[0] == 10:
                    addr = i[-1]
                    break
            if addr is None:
                raise ValueError("Request DNS Parsing IPV_6 Fail")
        else:
            for i in ai:
                if i[0] == 2:
                    addr = i[-1]
                    break
            if addr is None:
                raise ValueError("Request DNS Parsing IPV_4 Fail")
    elif url.split(".")[0].isdigit() and ":" not in url:
        raise ValueError(
            "MissingSchema: Invalid URL '{}': No schema supplied. Perhaps you meant http://{}? ".format(url, url))
    else:
        path = ""
        proto = ""
        if ":" not in url:
            raise ValueError("URL address error: !" + url)
        try:
            if "/" in url:
                ip_info = url.split('/', 1)
                path = ip_info[1]
                host, port = ip_info[0].split(":")
            else:
                host, port = url.split(":")
        except:
            raise ValueError("URL address error: " + url)
        try:
            ai = usocket.getaddrinfo(host, port, 0, usocket.SOCK_STREAM)
        except Exception as e:
            raise IndexError("Domain name resolution error, please check network connection")

        if ipvtype == 1:
            import uos
            name, platform = uos.uname()[1].split("=",1)
            if platform == "FCM360W" or platform == "FCM362K":
                raise ValueError("platform Not Support IPV6!")
            for i in ai:
                if i[0] == 10:
                    addr = i[-1]
                    break
            if addr is None:
                raise ValueError("Request DNS Parsing IPV6 Fail")
        else:
            for i in ai:
                if i[0] == 2:
                    addr = i[-1]
                    break
            if addr is None:
                raise ValueError("Request DNS Parsing IPV4 Fail")
    #global s

    if ipvtype == 1:
        import uos
        name, platform = uos.uname()[1].split("=",1)
        if platform == "FCM360W" or platform == "FCM362K":
            raise ValueError("platform Not Support IPV6!")
        try:
            s = usocket.socket(usocket.AF_INET6, usocket.SOCK_STREAM, usocket.TCP_CUSTOMIZE_PORT)
        except Exception as e:
            raise RuntimeError("socket resource malloc FAIL='{}'".format(str(e)))
        call_state = dataCall.getInfo(1,1)
        if call_state == -1:
            raise ValueError("request Connect dataCall get Info FAIL")
        elif call_state[2][0] != 1:
            raise ValueError("request Connect dataCall get IPV6 Info FAIL")
        try:
            s.bind((call_state[2][2],0))
        except Exception as e:
            raise ValueError("request sock bind IPV6 IP FAIL")
    else:
        try:
            s = usocket.socket(usocket.AF_INET, usocket.SOCK_STREAM)
        except Exception as e:
            raise RuntimeError("socket resource malloc FAIL='{}'".format(str(e)))

    s.settimeout(timeout)
    try:
        try:
            s.connect(addr)
        except Exception as e:
            s.close()
            raise RuntimeError("HTTP Connection '{}' FAIL='{}'".format(str(e), URL))
        if proto == "https:":
            import ussl
            try:
                if ssl_params:
                    s = ussl.wrap_socket(s, **ssl_params, server_hostname=host)
                else:
                    s = ussl.wrap_socket(s, server_hostname=host)
            except Exception as e:
                s.close()
                raise RuntimeError("HTTPS USSL '{}' FAIL='{}'".format(str(e), URL))
        if version == 1:
            s.write(b"%s /%s HTTP/1.1\r\n" % (method, path))
        else:
            s.write(b"%s /%s HTTP/1.0\r\n" % (method, path))
        s.write(b"Host: %s\r\n" % host)
        if version == 1:
            s.write(b"Connection: close\r\n")
        if headers:
            if files:
                boundary = str(utime.time())
                if not (files.get("filepath") and files.get("filename")):
                    raise ValueError("Missing key parameters 'filepath' and 'filename'")
                if headers.get('Content-Type') == "multipart/form-data":
                    headers['Content-Type'] = headers['Content-Type'] + '; boundary={}'.format(boundary)
                    headers['charset'] = 'UTF-8'
            if files_data:
                boundary = str(utime.time())
                if headers.get('Content-Type') == "multipart/form-data":
                    headers['Content-Type'] = headers['Content-Type'] + '; boundary={}'.format(boundary)
                    headers['charset'] = 'UTF-8'
        else:
            headers = dict()
            if files:
                boundary = str(utime.time())
                headers['Content-Type'] = "multipart/form-data; boundary={}".format(boundary)
                headers['charset'] = 'UTF-8'
            if json:
                headers['Content-Type'] = "application/json"
        for k in headers:
            s.write(k)
            s.write(b": ")
            s.write(headers[k])
            s.write(b"\r\n")
        if (username is not None) and (password is not None):
            try:
                import ubinascii
                key  = "{}:{}".format(username, password)
                key_64 = ubinascii.b2a_base64(key)[:-1].decode('utf-8')
                s.write(b"Authorization: Basic {}\r\n".format(key_64))
            except Exception as e:
                s.close()
                raise RuntimeError("HTTP base64 FAIL='{}'".format(str(e)))
        if json is not None:
            assert data is None
            data = ujson.dumps(json)
        if data:
            s.write(b"Content-Length: %d\r\n" % len(data))
            s.write(b"\r\n")
            s.write(data)
        if files_data:
            if files_name == None:
                raise ValueError("files_name must have")
            if files_data_len == None:
                raise ValueError("files_data_len must have")
            import uos
            name,platform = uos .uname()[1].split("=",1)
            datas = '--{0}{1}Content-Disposition: form-data; name="file"; filename="{2}"{1}Content-Type: application/octet-stream{1}{1}'.format(
                boundary, '\r\n', files_name)
            suffix = '{1}--{0}--{1}'.format(boundary, '\r\n')
            len_d = files_data_len + len(datas) + len(suffix)
            s.write(b"Content-Length: %d\r\n" % len_d)
            s.write(b"\r\n")
            s.write(datas)
            s.write(files_data[0:files_data_len])
            s.write(suffix)
        if files:
            import uos
            counter = 0
            name,platform = uos .uname()[1].split("=",1)
            file_path = files.get("filepath")
            with open(file_path, 'rb') as f:
                content = f.read(4096)
                if files.get("name") is not None:
                    datas = '--{0}{1}Content-Disposition: form-data; {2}{1}{1}{3}{1}--{0}'. \
                        format(boundary, '\r\n', files.get("name"), content)
                else:
                    datas = '--{0}{1}Content-Disposition: form-data; name="file"; filename="{2}"{1}Content-Type: application/octet-stream{1}{1}'.format(
                        boundary, '\r\n', files.get("filename"))
                if files.get("filepath1") is not None:
                    with open('{}'.format(files.get("filepath1")), 'r') as f1:
                        content1 = f1.read()
                        if files.get("name1") is not None:
                            datas += '{1}Content-Disposition: form-data; {2}{1}{1}{3}{1}--{0}'. \
                                    format(boundary, '\r\n', files.get("name1"), content1)
                        else:
                            if files.get("filename1") is None:
                                raise ValueError("Missing key parameters 'filename1' ")
                            datas += '{1}Content-Disposition: form-data; name="file"; filename="{2}"{1}Content-Type: application/octet-stream{1}{1}{3}{1}--{0}'. \
                                      format(boundary, '\r\n', files.get("filename1"), content1)
                suffix = '{1}--{0}--{1}'.format(boundary, '\r\n')
                len_d = uos.stat(file_path)[-4] + len(datas) + len(suffix)
                s.write(b"Content-Length: %d\r\n" % len_d)
                s.write(b"\r\n")
                s.write(datas)
                while content:
                    s.write(content)
                    content = f.read(4096)                 
                s.write(suffix)
        if not (files and data and files_data):
            s.write(b"\r\n")
        l = s.readline()
        uheaders = {}
        chunked_encoding = False
        try:
            # jian.yao 2020-12-09 Abnormal response handle
            l = l.split(None, 2)
            status = int(l[1])
        except:
            raise ValueError("InvalidSchema: No connection adapters were found for '{}'".format(URL))
        reason = ""
        if len(l) > 2:
            reason = l[2].rstrip()
        con_len = sizeof
        con_flag = False
        while True:
            l = s.readline()
            j = l.decode().split(":")
            try:
                uheaders[j[0]] = j[1].replace("\n", "").replace("\r", "")
            except Exception as e:
                pass
            if not l or l == b"\r\n":
                break

            if l.lower().startswith(b"transfer-encoding:"):
                if b"chunked" in l.lower():
                    chunked_encoding = True
            if l.startswith(b'Content-Length:'):
                con_len = int(l.split(b' ')[1])
                con_flag = True
            #if l.startswith(b"Location:") and not 200 <= status <= 299:
                #raise NotImplementedError("Redirects not yet supported")
    except OSError:
        s.close()
        raise
    if chunked_encoding:
        resp = Response(uio.BytesIO(parse_chunked_data(s)), sizeof=con_len)
    else:
        resp = Response(s,sizeof=con_len, flag=con_flag)
    resp.status_code = status
    resp.reason = reason
    resp.headers = uheaders
    return resp


def head(url, **kw):
    return request("HEAD", url, **kw)


def get(url, **kw):
    return request("GET", url, **kw)


def post(url, **kw):
    return request("POST", url, **kw)


def put(url, **kw):
    return request("PUT", url, **kw)


def patch(url, **kw):
    return request("PATCH", url, **kw)


def delete(url, **kw):
    return request("DELETE", url, **kw)
