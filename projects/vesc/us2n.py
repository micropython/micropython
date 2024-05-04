# us2n.py

import json
import time
import select
import socket
import network

print_ = print
VERBOSE = 1

lcd = None


def print(*args, **kwargs):
    if lcd:
        lcd.clear()
        for i, arg in enumerate(args):
            lcd.set_cursor(0, i)
            lcd.print(arg)
        time.sleep(3)

    if VERBOSE:
        print_(*args, **kwargs)
        

def read_config(filename='us2n.json', obj=None, default=None):
    with open(filename, 'r') as f:
        config = json.load(f)
        if obj is None:
            return config
        return config.get(obj, default)


def parse_bind_address(addr, default=None):
    if addr is None:
        return default
    args = addr
    if not isinstance(args, (list, tuple)):
        args = addr.rsplit(':', 1)
    host = '' if len(args) == 1 or args[0] == '0' else args[0]
    port = int(args[1])
    return host, port


class Bridge:

    def __init__(self, config, _uart):
        super().__init__()
        self.config = config
        self.uart = _uart
        self.uart_port = config['uart']['port']
        self.tcp = None
        self.address = parse_bind_address(config['tcp']['bind'])
        self.bind_port = self.address[1]
        self.client = None
        self.client_address = None

    def bind(self):
        tcp = socket.socket()
        tcp.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    #    tcp.setblocking(False)
        tcp.bind(self.address)
        tcp.listen(5)
        print('Bridge listening at TCP({0}) for UART({1})'
              .format(self.bind_port, self.uart_port))
        self.tcp = tcp
        return tcp

    def fill(self, fds):
        if self.uart is not None:
            fds.append(self.uart)
        if self.tcp is not None:
            fds.append(self.tcp)
        if self.client is not None:
            fds.append(self.client)
        return fds

    def handle(self, fd):
        if fd == self.tcp:
            self.close_client()
            self.open_client()
        elif fd == self.client:
            data = self.client.recv(4096)
            if data:
                print('TCP({0})->UART({1}) {2}'.format(self.bind_port,
                                                       self.uart_port, data))
                self.uart.write(data)
            else:
                print('Client ', self.client_address, ' disconnected')
                self.close_client()
        elif fd == self.uart:
            data = self.uart.read()
            print('UART({0})->TCP({1}) {2}'.format(self.uart_port,
                                                   self.bind_port, data))
            self.client.sendall(data)

    def close_client(self):
        if self.client is not None:
            print('Closing client ', self.client_address)
            self.client.close()
            self.client = None
            self.client_address = None   

    def open_client(self):
        self.client, self.client_address = self.tcp.accept()
        print('Accepted connection from ', self.client_address)

    def close(self):
        self.close_client()
        if self.tcp is not None:
            print('Closing TCP server {0}...'.format(self.address))
            self.tcp.close()
            self.tcp = None


class S2NServer:

    def __init__(self, config, _uart):
        self.config = config
        self.uart = _uart

    def serve_forever(self):
        try:
            self._serve_forever()
        except KeyboardInterrupt:
            print('Ctrl-C pressed. Bailing out')

    def bind(self):
        bridges = []
        for config in self.config['bridges']:
            bridge = Bridge(config, self.uart)
            bridge.bind()
            bridges.append(bridge)
        return bridges

    def _serve_forever(self):
        bridges = self.bind()

        try:
            while True:
                fds = []
                for bridge in bridges:
                    bridge.fill(fds)
                rlist, _, xlist = select.select(fds, (), fds)
                if xlist:
                    print('Errors. bailing out')
                    continue
                for fd in rlist:
                    for bridge in bridges:
                        bridge.handle(fd)
        finally:
            for bridge in bridges:
                bridge.close()


def config_lan(config, name):
    # For a board which has LAN
    pass


def config_wlan(config, name):
    if config is None:
        return None, None
    return (WLANStation(config.get('sta'), name),
            WLANAccessPoint(config.get('ap'), name))


def WLANStation(config, name):
    if config is None:
        return
    essid = config['essid']
    password = config['password']
    sta = network.WLAN(network.STA_IF)

    if not sta.isconnected():
        sta.active(True)
        sta.connect(essid, password)
        n, ms = 20, 250
        t = n*ms
        while not sta.isconnected() and n > 0:
            time.sleep_ms(ms)
            n -= 1
        if not sta.isconnected():
            print('Failed to connect wifi station after {0}ms. I give up'
                  .format(t))
            return sta
    print('Station at:'. str(sta.ifconfig()[0]))
    return sta


def WLANAccessPoint(config, name):
    if config is None:
        return
    config.setdefault('essid', name)
    config.setdefault('channel', 11)
    config.setdefault('authmode',
                      getattr(network, 'AUTH_' +
                              config.get('authmode', 'OPEN').upper()))
    config.setdefault('hidden', False)
#    config.setdefault('dhcp_hostname', name)
    ap = network.WLAN(network.AP_IF)
    if not ap.isconnected():
        ap.active(True)
        n, ms = 20, 250
        t = n * ms
        while not ap.active() and n > 0:
            time.sleep_ms(ms)
            n -= 1
        if not ap.active():
            print('Failed to activate wifi access point after {0}ms. ' \
                  'I give up'.format(t))
            return ap

#    ap.config(**config)
    print('Wifi {0!r} connected as {1}'.format(ap.config('essid'),
                                               ap.ifconfig()))
    return ap


def config_network(config, name):
    config_lan(config, name)
    config_wlan(config, name)


def config_verbosity(config):
    global VERBOSE
    VERBOSE = config.setdefault('verbose', 1)
    for bridge in config.get('bridges'):
        if bridge.get('uart', {}).get('port', None) == 0:
            VERBOSE = 0


def server(config_filename='us2n.json', uart=None, _lcd=None):
    global lcd
    lcd = _lcd
    config = read_config(config_filename)
    VERBOSE = config.setdefault('verbose', 1)
    name = config.setdefault('name', 'Tiago-ESP32')
    config_verbosity(config)
    #print(50*'=')
    print('ESP32 serial <->', 'tcp bridge')
    config_network(config.get('wlan'), name)
    return S2NServer(config, uart)
