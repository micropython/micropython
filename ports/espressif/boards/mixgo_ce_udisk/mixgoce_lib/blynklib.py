# Copyright (c) 2019-2020 Anton Morozenko
# Copyright (c) 2015-2019 Volodymyr Shymanskyy.
# See the file LICENSE for copying permission.
# 宋义深 2021/4/25 Modified
__version__ = '0.2.6'

import socketpool as socket
import ssl
import struct
import time
import wifi

LOGO = """
        ___  __          __
       / _ )/ /_ _____  / /__
      / _  / / // / _ \\/  '_/
     /____/_/\\_, /_//_/_/\\_\\
            /___/ for Python v{}\n""".format(__version__)


def stub_log(*args):
    pass


def ticks_ms():
    return int(time.time() * 1000)


def sleep_ms(ms):
    time.sleep(ms // 1000)


class BlynkError(Exception):
    pass


class RedirectError(Exception):
    def __init__(self, server, port):
        self.server = server
        self.port = port


class Protocol(object):
    MSG_RSP = 0
    MSG_LOGIN = 2
    MSG_PING = 6
    MSG_TWEET = 12
    MSG_EMAIL = 13
    MSG_NOTIFY = 14
    MSG_BRIDGE = 15
    MSG_HW_SYNC = 16
    MSG_INTERNAL = 17
    MSG_PROPERTY = 19
    MSG_HW = 20
    MSG_REDIRECT = 41
    MSG_HEAD_LEN = 5

    STATUS_INVALID_TOKEN = 9
    STATUS_NO_DATA = 17
    STATUS_OK = 200
    VPIN_MAX_NUM = 32

    _msg_id = 0

    def _get_msg_id(self, **kwargs):
        if 'msg_id' in kwargs:
            return kwargs['msg_id']
        self._msg_id += 1
        return self._msg_id if self._msg_id <= 0xFFFF else 1

    def _pack_msg(self, msg_type, *args, **kwargs):
        data = ('\0'.join([str(curr_arg) for curr_arg in args])).encode('utf-8')
        return struct.pack('!BHH', msg_type, self._get_msg_id(**kwargs), len(data)) + data

    def parse_response(self, rsp_data, msg_buffer):
        msg_args = []
        try:
            msg_type, msg_id, h_data = struct.unpack('!BHH', rsp_data[:self.MSG_HEAD_LEN])
        except Exception as p_err:
            print(p_err)
            raise BlynkError('Message parse error: {}'.format(p_err))
        if msg_id == 0:
            raise BlynkError('invalid msg_id == 0')
        elif h_data >= msg_buffer:
            raise BlynkError('Command too long. Length = {}'.format(h_data))
        elif msg_type in (self.MSG_RSP, self.MSG_PING):
            pass
        elif msg_type in (self.MSG_HW, self.MSG_BRIDGE, self.MSG_INTERNAL, self.MSG_REDIRECT):
            msg_body = rsp_data[self.MSG_HEAD_LEN: self.MSG_HEAD_LEN + h_data]
            msg_args = [itm.decode('utf-8') for itm in msg_body.split(b'\0')]
        else:
            print('unknown')
            raise BlynkError("Unknown message type: '{}'".format(msg_type))
        return msg_type, msg_id, h_data, msg_args

    def heartbeat_msg(self, heartbeat, rcv_buffer):
        return self._pack_msg(self.MSG_INTERNAL, 'ver', __version__, 'buff-in', rcv_buffer, 'h-beat', heartbeat,
                              'dev', 'python')

    def login_msg(self, token):
        return self._pack_msg(self.MSG_LOGIN, token)

    def ping_msg(self):
        return self._pack_msg(self.MSG_PING)

    def response_msg(self, *args, **kwargs):
        return self._pack_msg(self.MSG_RSP, *args, **kwargs)

    def virtual_write_msg(self, v_pin, *val):
        return self._pack_msg(self.MSG_HW, 'vw', v_pin, *val)

    def virtual_sync_msg(self, *pins):
        return self._pack_msg(self.MSG_HW_SYNC, 'vr', *pins)

    def email_msg(self, to, subject, body):
        return self._pack_msg(self.MSG_EMAIL, to, subject, body)

    def tweet_msg(self, msg):
        return self._pack_msg(self.MSG_TWEET, msg)

    def notify_msg(self, msg):
        return self._pack_msg(self.MSG_NOTIFY, msg)

    def set_property_msg(self, pin, prop, *val):
        return self._pack_msg(self.MSG_PROPERTY, pin, prop, *val)

    def internal_msg(self, *args):
        return self._pack_msg(self.MSG_INTERNAL, *args)


class Connection(Protocol):
    SOCK_MAX_TIMEOUT = 5
    SOCK_TIMEOUT = 0
    SOCK_SSL_TIMEOUT = 1
    EAGAIN = 11
    ETIMEDOUT = 60
    RETRIES_TX_DELAY = 2
    RETRIES_TX_MAX_NUM = 3
    RECONNECT_SLEEP = 1
    TASK_PERIOD_RES = 50
    DISCONNECTED = 0
    CONNECTING = 1
    AUTHENTICATING = 2
    AUTHENTICATED = 3

    _state = None
    _socket = None
    _socketPool = None
    _last_rcv_time = 0
    _last_ping_time = 0
    _last_send_time = 0

    def __init__(self, token, server='blynk-cloud.com', port=80, ssl_cert=None, heartbeat=10, rcv_buffer=1024,
                 log=stub_log):
        self.token = token
        self.server = server
        self.port = port
        self.heartbeat = heartbeat
        self.rcv_buffer = rcv_buffer
        self.log = log
        self.ssl_cert = ssl_cert

    def send(self, data):
        retries = self.RETRIES_TX_MAX_NUM
        while retries > 0:
            try:
                retries -= 1
                self._last_send_time = ticks_ms()
                return self._socket.send(data)
            except (Exception):
                sleep_ms(self.RETRIES_TX_DELAY)

    def receive(self, length, timeout):
        d_buff = bytearray(length)
        try:
            self._socket.settimeout(timeout)
            recv_length = self._socket.recv_into(d_buff)
            d_buff = d_buff[:recv_length]
            #print(self.parse_response(bytes(d_buff), self.rcv_buffer))
            return bytes(d_buff)
        except (Exception) as err:
            if 'timed out' in str(err):
                return b''
            if str(self.EAGAIN) in str(err) or str(self.ETIMEDOUT) in str(err):
                return b''
            raise

    def is_server_alive(self):
        now = ticks_ms()
        h_beat_ms = self.heartbeat * 1000
        rcv_delta = now - self._last_rcv_time
        ping_delta = now - self._last_ping_time
        send_delta = now - self._last_send_time
        if rcv_delta > h_beat_ms + (h_beat_ms // 2):
            return False
        if (ping_delta > h_beat_ms // 10) and (send_delta > h_beat_ms or rcv_delta > h_beat_ms):
            self.send(self.ping_msg())
            self.log('Heartbeat time: {}'.format(now))
            self._last_ping_time = now
        return True

    def _get_socket(self):
        try:
            self._state = self.CONNECTING
            self._socket = socket.SocketPool(wifi.radio).socket()
            self._socketPool = socket.SocketPool(wifi.radio)
            self._socket.connect(self._socketPool.getaddrinfo(self.server, self.port)[0][4])
            self._socket.settimeout(self.SOCK_TIMEOUT)
            if self.ssl_cert:
                # system default CA certificates case
                if self.ssl_cert == "default":
                    self.ssl_cert = None
                self.log('Using SSL socket...')
                ssl_context = ssl.create_default_context(cafile=self.ssl_cert)
                ssl_context.verify_mode = ssl.CERT_REQUIRED
                self._socket.settimeout(self.SOCK_SSL_TIMEOUT)
                self._socket = ssl_context.wrap_socket(sock=self._socket, server_hostname=self.server)
            self.log('Connected to blynk server')
        except Exception as g_exc:
            print(g_exc)
            raise BlynkError('Connection with the Blynk server failed: {}'.format(g_exc))

    def _authenticate(self):
        self._state = self.AUTHENTICATING
        self.send(self.login_msg(self.token))
        rsp_data = self.receive(self.rcv_buffer, self.SOCK_MAX_TIMEOUT)
        if not rsp_data:
            raise BlynkError('Auth stage timeout')
        msg_type, _, status, args = self.parse_response(rsp_data, self.rcv_buffer)
        if status != self.STATUS_OK:
            if status == self.STATUS_INVALID_TOKEN:
                raise BlynkError('Invalid Auth Token')
            if msg_type == self.MSG_REDIRECT:
                raise RedirectError(*args)
            raise BlynkError('Auth stage failed. Status={}'.format(status))
        self._state = self.AUTHENTICATED

    def _set_heartbeat(self):
        self.send(self.heartbeat_msg(self.heartbeat, self.rcv_buffer))
        rcv_data = self.receive(self.rcv_buffer, self.SOCK_MAX_TIMEOUT)
        if not rcv_data:
            raise BlynkError('Heartbeat stage timeout')
        _, _, status, _ = self.parse_response(rcv_data, self.rcv_buffer)
        if status != self.STATUS_OK:
            raise BlynkError('Set heartbeat returned code={}'.format(status))
        self.log('Heartbeat = {} sec. MaxCmdBuffer = {} bytes'.format(self.heartbeat, self.rcv_buffer))

    def connected(self):
        return True if self._state == self.AUTHENTICATED else False


class Blynk(Connection):
    _CONNECT_TIMEOUT = 30  # 30sec
    _VPIN_WILDCARD = '*'
    _VPIN_READ = 'read v'
    _VPIN_WRITE = 'write v'
    _INTERNAL = 'internal_'
    _CONNECT = 'connect'
    _DISCONNECT = 'disconnect'
    _VPIN_READ_ALL = '{}{}'.format(_VPIN_READ, _VPIN_WILDCARD)
    _VPIN_WRITE_ALL = '{}{}'.format(_VPIN_WRITE, _VPIN_WILDCARD)
    _events = {}

    def __init__(self, token, **kwargs):
        Connection.__init__(self, token, **kwargs)
        self._start_time = ticks_ms()
        self._last_rcv_time = ticks_ms()
        self._last_send_time = ticks_ms()
        self._last_ping_time = ticks_ms()
        self._state = self.DISCONNECTED
        print(LOGO)

    def connect(self, timeout=_CONNECT_TIMEOUT):
        end_time = time.time() + timeout
        while not self.connected():
            if self._state == self.DISCONNECTED:
                try:
                    self._get_socket()
                    print('[Connecting 1/5] Socket got.')
                    self._authenticate()
                    print('[Connecting 2/5] Authenticated.')
                    self._set_heartbeat()
                    print('[Connecting 3/5] Heartbeat sent.')
                    self._last_rcv_time = ticks_ms()
                    print('[Connecting 4/5] Last receive time set.')
                    self.log('Registered events: {}\n'.format(list(self._events.keys())))
                    print('[Connecting 5/5] Events registered.')
                    self.call_handler(self._CONNECT)
                    return True
                except BlynkError as b_err:
                    self.disconnect(b_err)
                    sleep_ms(self.TASK_PERIOD_RES)
                except RedirectError as r_err:
                    self.disconnect()
                    self.server = r_err.server
                    self.port = r_err.port
                    sleep_ms(self.TASK_PERIOD_RES)
            if time.time() >= end_time:
                return False

    def disconnect(self, err_msg=None):
        self.call_handler(self._DISCONNECT)
        if self._socket:
            self._socket.close()
        self._state = self.DISCONNECTED
        if err_msg:
            self.log('[ERROR]: {}\nConnection closed'.format(err_msg))
        self._msg_id = 0
        time.sleep(self.RECONNECT_SLEEP)

    def virtual_write(self, v_pin, *val):
        return self.send(self.virtual_write_msg(v_pin, *val))

    def virtual_sync(self, *v_pin):
        return self.send(self.virtual_sync_msg(*v_pin))

    def email(self, to, subject, body):
        return self.send(self.email_msg(to, subject, body))

    def tweet(self, msg):
        return self.send(self.tweet_msg(msg))

    def notify(self, msg):
        return self.send(self.notify_msg(msg))

    def set_property(self, v_pin, property_name, *val):
        return self.send(self.set_property_msg(v_pin, property_name, *val))

    def internal(self, *args):
        return self.send(self.internal_msg(*args))

    def handle_event(blynk, event_name):
        class Deco(object):
            def __init__(self, func):
                self.func = func
                # wildcard 'read V*' and 'write V*' events handling
                if str(event_name).lower() in (blynk._VPIN_READ_ALL, blynk._VPIN_WRITE_ALL):
                    event_base_name = str(event_name).split(blynk._VPIN_WILDCARD)[0]
                    for i in range(blynk.VPIN_MAX_NUM + 1):
                        blynk._events['{}{}'.format(event_base_name.lower(), i)] = func
                else:
                    blynk._events[str(event_name).lower()] = func

            def __call__(self):
                return self.func()

        return Deco

    def call_handler(self, event, *args, **kwargs):
        if event in self._events.keys():
            self.log("Event: ['{}'] -> {}".format(event, args))
            self._events[event](*args, **kwargs)

    def process(self, msg_type, msg_id, msg_len, msg_args):
        if msg_type == self.MSG_RSP:
            self.log('Response status: {}'.format(msg_len))
        elif msg_type == self.MSG_PING:
            self.send(self.response_msg(self.STATUS_OK, msg_id=msg_id))
        elif msg_type in (self.MSG_HW, self.MSG_BRIDGE, self.MSG_INTERNAL):
            if msg_type == self.MSG_INTERNAL:
                self.call_handler("{}{}".format(self._INTERNAL, msg_args[0]), msg_args[1:])
            elif len(msg_args) >= 3 and msg_args[0] == 'vw':
                self.call_handler("{}{}".format(self._VPIN_WRITE, msg_args[1]), int(msg_args[1]), msg_args[2:])
            elif len(msg_args) == 2 and msg_args[0] == 'vr':
                self.call_handler("{}{}".format(self._VPIN_READ, msg_args[1]), int(msg_args[1]))

    def read_response(self, timeout=0.5):
        end_time = time.time() + timeout
        while time.time() <= end_time:
            rsp_data = self.receive(self.rcv_buffer, self.SOCK_TIMEOUT)
            if rsp_data:
                self._last_rcv_time = ticks_ms()
                msg_type, msg_id, h_data, msg_args = self.parse_response(rsp_data, self.rcv_buffer)
                self.process(msg_type, msg_id, h_data, msg_args)

    def run(self):
        if not self.connected():
            self.connect()
        else:
            try:
                self.read_response(timeout=self.SOCK_TIMEOUT)
                if not self.is_server_alive():
                    self.disconnect('Blynk server is offline')
            except KeyboardInterrupt:
                raise
            except BlynkError as b_err:
                self.log(b_err)
                self.disconnect()
            except Exception as g_exc:
                self.log(g_exc)
