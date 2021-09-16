#
# Small ftp server for ESP8266 Micropython
# Based on the work of chrisgp - Christopher Popp and pfalcon - Paul Sokolovsky
#
# The server accepts passive mode only. It runs in background.
# Start the server with:
#
# import uftpd
# uftpd.start([port = 21][, verbose = level])
#
# port is the port number (default 21)
# verbose controls the level of printed activity messages, values 0, 1, 2
#
# Copyright (c) 2016 Christopher Popp (initial ftp server framework)
# Copyright (c) 2016 Paul Sokolovsky (background execution control structure)
# Copyright (c) 2016 Robert Hammelrath (putting the pieces together and a
# Copyright (c) 2020 Jan Wieck Use separate FTP servers per socket for STA + AP mode
# few extensions)
# Distributed under MIT License
#
import socket
import network
import uos
import gc
import sys
from time import sleep_ms, localtime
from micropython import alloc_emergency_exception_buf

# constant definitions
_CHUNK_SIZE = const(1024)
_SO_REGISTER_HANDLER = const(20)
_COMMAND_TIMEOUT = const(300)
_DATA_TIMEOUT = const(100)
_DATA_PORT = const(13333)

# Global variables
ftpsockets = []
datasocket = None
client_list = []
verbose_l = 0
client_busy = False
# Interfaces: (IP-Address (string), IP-Address (integer), Netmask (integer))

_month_name = ("", "Jan", "Feb", "Mar", "Apr", "May", "Jun",
               "Jul", "Aug", "Sep", "Oct", "Nov", "Dec")


class FTP_client:

    def __init__(self, ftpsocket, local_addr):
        self.command_client, self.remote_addr = ftpsocket.accept()
        self.remote_addr = self.remote_addr[0]
        self.command_client.settimeout(_COMMAND_TIMEOUT)
        log_msg(1, "FTP Command connection from:", self.remote_addr)
        self.command_client.setsockopt(socket.SOL_SOCKET,
                                       _SO_REGISTER_HANDLER,
                                       self.exec_ftp_command)
        self.command_client.sendall("220 Hello, this is the {}.\r\n".format(sys.platform))
        self.cwd = '/'
        self.fromname = None
#        self.logged_in = False
        self.act_data_addr = self.remote_addr
        self.DATA_PORT = 20
        self.active = True
        self.pasv_data_addr = local_addr

    def send_list_data(self, path, data_client, full):
        try:
            for fname in uos.listdir(path):
                data_client.sendall(self.make_description(path, fname, full))
        except Exception as e:  # path may be a file name or pattern
            path, pattern = self.split_path(path)
            try:
                for fname in uos.listdir(path):
                    if self.fncmp(fname, pattern):
                        data_client.sendall(
                            self.make_description(path, fname, full))
            except:
                pass

    def make_description(self, path, fname, full):
        global _month_name
        if full:
            stat = uos.stat(self.get_absolute_path(path, fname))
            file_permissions = ("drwxr-xr-x"
                                if (stat[0] & 0o170000 == 0o040000)
                                else "-rw-r--r--")
            file_size = stat[6]
            tm = stat[7] & 0xffffffff
            tm = localtime(tm if tm < 0x80000000 else tm - 0x100000000)
            if tm[0] != localtime()[0]:
                description = "{} 1 owner group {:>10} {} {:2} {:>5} {}\r\n".\
                    format(file_permissions, file_size,
                        _month_name[tm[1]], tm[2], tm[0], fname)
            else:
                description = "{} 1 owner group {:>10} {} {:2} {:02}:{:02} {}\r\n".\
                    format(file_permissions, file_size,
                        _month_name[tm[1]], tm[2], tm[3], tm[4], fname)
        else:
            description = fname + "\r\n"
        return description

    def send_file_data(self, path, data_client):
        with open(path, "rb") as file:
            chunk = file.read(_CHUNK_SIZE)
            while len(chunk) > 0:
                data_client.sendall(chunk)
                chunk = file.read(_CHUNK_SIZE)
            data_client.close()

    def save_file_data(self, path, data_client, mode):
        with open(path, mode) as file:
            chunk = data_client.recv(_CHUNK_SIZE)
            while len(chunk) > 0:
                file.write(chunk)
                chunk = data_client.recv(_CHUNK_SIZE)
            data_client.close()

    def get_absolute_path(self, cwd, payload):
        # Just a few special cases "..", "." and ""
        # If payload start's with /, set cwd to /
        # and consider the remainder a relative path
        if payload.startswith('/'):
            cwd = "/"
        for token in payload.split("/"):
            if token == '..':
                cwd = self.split_path(cwd)[0]
            elif token != '.' and token != '':
                if cwd == '/':
                    cwd += token
                else:
                    cwd = cwd + '/' + token
        return cwd

    def split_path(self, path):  # instead of path.rpartition('/')
        tail = path.split('/')[-1]
        head = path[:-(len(tail) + 1)]
        return ('/' if head == '' else head, tail)

    # compare fname against pattern. Pattern may contain
    # the wildcards ? and *.
    def fncmp(self, fname, pattern):
        pi = 0
        si = 0
        while pi < len(pattern) and si < len(fname):
            if (fname[si] == pattern[pi]) or (pattern[pi] == '?'):
                si += 1
                pi += 1
            else:
                if pattern[pi] == '*':  # recurse
                    if pi == len(pattern.rstrip("*?")):  # only wildcards left
                        return True
                    while si < len(fname):
                        if self.fncmp(fname[si:], pattern[pi + 1:]):
                            return True
                        else:
                            si += 1
                    return False
                else:
                    return False
        if pi == len(pattern.rstrip("*")) and si == len(fname):
            return True
        else:
            return False

    def open_dataclient(self):
        if self.active:  # active mode
            data_client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            data_client.settimeout(_DATA_TIMEOUT)
            data_client.connect((self.act_data_addr, self.DATA_PORT))
            log_msg(1, "FTP Data connection with:", self.act_data_addr)
        else:  # passive mode
            data_client, data_addr = datasocket.accept()
            log_msg(1, "FTP Data connection with:", data_addr[0])
        return data_client

    def exec_ftp_command(self, cl):
        global datasocket
        global client_busy
        global my_ip_addr

        try:
            gc.collect()

            data = cl.readline().decode("utf-8").rstrip("\r\n")

            if len(data) <= 0:
                # No data, close
                # This part is NOT CLEAN; there is still a chance that a
                # closing data connection will be signalled as closing
                # command connection
                log_msg(1, "*** No data, assume QUIT")
                close_client(cl)
                return

            if client_busy:  # check if another client is busy
                cl.sendall("400 Device busy.\r\n")  # tell so the remote client
                return  # and quit
            client_busy = True  # now it's my turn

            # check for log-in state may done here, like
            # if self.logged_in == False and not command in\
            #    ("USER", "PASS", "QUIT"):
            #    cl.sendall("530 Not logged in.\r\n")
            #    return

            command = data.split()[0].upper()
            payload = data[len(command):].lstrip()  # partition is missing
            path = self.get_absolute_path(self.cwd, payload)
            log_msg(1, "Command={}, Payload={}".format(command, payload))

            if command == "USER":
                # self.logged_in = True
                cl.sendall("230 Logged in.\r\n")
                # If you want to see a password,return
                #   "331 Need password.\r\n" instead
                # If you want to reject an user, return
                #   "530 Not logged in.\r\n"
            elif command == "PASS":
                # you may check here for a valid password and return
                # "530 Not logged in.\r\n" in case it's wrong
                # self.logged_in = True
                cl.sendall("230 Logged in.\r\n")
            elif command == "SYST":
                cl.sendall("215 UNIX Type: L8\r\n")
            elif command in ("TYPE", "NOOP", "ABOR"):  # just accept & ignore
                cl.sendall('200 OK\r\n')
            elif command == "QUIT":
                cl.sendall('221 Bye.\r\n')
                close_client(cl)
            elif command == "PWD" or command == "XPWD":
                cl.sendall('257 "{}"\r\n'.format(self.cwd))
            elif command == "CWD" or command == "XCWD":
                try:
                    if (uos.stat(path)[0] & 0o170000) == 0o040000:
                        self.cwd = path
                        cl.sendall('250 OK\r\n')
                    else:
                        cl.sendall('550 Fail\r\n')
                except:
                    cl.sendall('550 Fail\r\n')
            elif command == "PASV":
                cl.sendall('227 Entering Passive Mode ({},{},{}).\r\n'.format(
                    self.pasv_data_addr.replace('.', ','),
                    _DATA_PORT >> 8, _DATA_PORT % 256))
                self.active = False
            elif command == "PORT":
                items = payload.split(",")
                if len(items) >= 6:
                    self.act_data_addr = '.'.join(items[:4])
                    if self.act_data_addr == "127.0.1.1":
                        # replace by command session addr
                        self.act_data_addr = self.remote_addr
                    self.DATA_PORT = int(items[4]) * 256 + int(items[5])
                    cl.sendall('200 OK\r\n')
                    self.active = True
                else:
                    cl.sendall('504 Fail\r\n')
            elif command == "LIST" or command == "NLST":
                if payload.startswith("-"):
                    option = payload.split()[0].lower()
                    path = self.get_absolute_path(
                            self.cwd, payload[len(option):].lstrip())
                else:
                    option = ""
                try:
                    data_client = self.open_dataclient()
                    cl.sendall("150 Directory listing:\r\n")
                    self.send_list_data(path, data_client,
                                        command == "LIST" or 'l' in option)
                    cl.sendall("226 Done.\r\n")
                    data_client.close()
                except:
                    cl.sendall('550 Fail\r\n')
                    if data_client is not None:
                        data_client.close()
            elif command == "RETR":
                try:
                    data_client = self.open_dataclient()
                    cl.sendall("150 Opened data connection.\r\n")
                    self.send_file_data(path, data_client)
                    # if the next statement is reached,
                    # the data_client was closed.
                    data_client = None
                    cl.sendall("226 Done.\r\n")
                except:
                    cl.sendall('550 Fail\r\n')
                    if data_client is not None:
                        data_client.close()
            elif command == "STOR" or command == "APPE":
                try:
                    data_client = self.open_dataclient()
                    cl.sendall("150 Opened data connection.\r\n")
                    self.save_file_data(path, data_client,
                                        "wb" if command == "STOR" else "ab")
                    # if the next statement is reached,
                    # the data_client was closed.
                    data_client = None
                    cl.sendall("226 Done.\r\n")
                except:
                    cl.sendall('550 Fail\r\n')
                    if data_client is not None:
                        data_client.close()
            elif command == "SIZE":
                try:
                    cl.sendall('213 {}\r\n'.format(uos.stat(path)[6]))
                except:
                    cl.sendall('550 Fail\r\n')
            elif command == "MDTM":
                try:
                    tm=localtime(uos.stat(path)[8])
                    cl.sendall('213 {:04d}{:02d}{:02d}{:02d}{:02d}{:02d}\r\n'.format(*tm[0:6]))
                except:
                    cl.sendall('550 Fail\r\n')
            elif command == "STAT":
                if payload == "":
                    cl.sendall("211-Connected to ({})\r\n"
                               "    Data address ({})\r\n"
                               "    TYPE: Binary STRU: File MODE: Stream\r\n"
                               "    Session timeout {}\r\n"
                               "211 Client count is {}\r\n".format(
                                self.remote_addr, self.pasv_data_addr,
                                _COMMAND_TIMEOUT, len(client_list)))
                else:
                    cl.sendall("213-Directory listing:\r\n")
                    self.send_list_data(path, cl, True)
                    cl.sendall("213 Done.\r\n")
            elif command == "DELE":
                try:
                    uos.remove(path)
                    cl.sendall('250 OK\r\n')
                except:
                    cl.sendall('550 Fail\r\n')
            elif command == "RNFR":
                try:
                    # just test if the name exists, exception if not
                    uos.stat(path)
                    self.fromname = path
                    cl.sendall("350 Rename from\r\n")
                except:
                    cl.sendall('550 Fail\r\n')
            elif command == "RNTO":
                    try:
                        uos.rename(self.fromname, path)
                        cl.sendall('250 OK\r\n')
                    except:
                        cl.sendall('550 Fail\r\n')
                    self.fromname = None
            elif command == "CDUP" or command == "XCUP":
                self.cwd = self.get_absolute_path(self.cwd, "..")
                cl.sendall('250 OK\r\n')
            elif command == "RMD" or command == "XRMD":
                try:
                    uos.rmdir(path)
                    cl.sendall('250 OK\r\n')
                except:
                    cl.sendall('550 Fail\r\n')
            elif command == "MKD" or command == "XMKD":
                try:
                    uos.mkdir(path)
                    cl.sendall('250 OK\r\n')
                except:
                    cl.sendall('550 Fail\r\n')
            elif command == "SITE":
                try:
                    exec(payload.replace('\0','\n'))
                    cl.sendall('250 OK\r\n')
                except:
                    cl.sendall('550 Fail\r\n')
            else:
                cl.sendall("502 Unsupported command.\r\n")
                # log_msg(2,
                #  "Unsupported command {} with payload {}".format(command,
                #  payload))
        # handle unexpected errors
        except Exception as err:
            log_msg(1, "Exception in exec_ftp_command: {}".format(err))
        # tidy up before leaving
        client_busy = False


def log_msg(level, *args):
    global verbose_l
    if verbose_l >= level:
        print(*args)


# close client and remove it from the list
def close_client(cl):
    cl.setsockopt(socket.SOL_SOCKET, _SO_REGISTER_HANDLER, None)
    cl.close()
    for i, client in enumerate(client_list):
        if client.command_client == cl:
            del client_list[i]
            break


def accept_ftp_connect(ftpsocket, local_addr):
    # Accept new calls for the server
    try:
        client_list.append(FTP_client(ftpsocket, local_addr))
    except:
        log_msg(1, "Attempt to connect failed")
        # try at least to reject
        try:
            temp_client, temp_addr = ftpsocket.accept()
            temp_client.close()
        except:
            pass


def num_ip(ip):
    items = ip.split(".")
    return (int(items[0]) << 24 | int(items[1]) << 16 |
            int(items[2]) << 8 | int(items[3]))


def stop():
    global ftpsockets, datasocket
    global client_list
    global client_busy

    for client in client_list:
        client.command_client.setsockopt(socket.SOL_SOCKET,
                                         _SO_REGISTER_HANDLER, None)
        client.command_client.close()
    del client_list
    client_list = []
    client_busy = False
    for sock in ftpsockets:
        sock.setsockopt(socket.SOL_SOCKET, _SO_REGISTER_HANDLER, None)
        sock.close()
    ftpsockets = []
    if datasocket is not None:
        datasocket.close()
        datasocket = None


# start listening for ftp connections on port 21
def start(port=21, verbose=0, splash=True):
    global ftpsockets, datasocket
    global verbose_l
    global client_list
    global client_busy

    alloc_emergency_exception_buf(100)
    verbose_l = verbose
    client_list = []
    client_busy = False

    for interface in [network.AP_IF, network.STA_IF]:
        wlan = network.WLAN(interface)
        if not wlan.active():
            continue

        ifconfig = wlan.ifconfig()
        addr = socket.getaddrinfo(ifconfig[0], port)
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock.bind(addr[0][4])
        sock.listen(1)
        sock.setsockopt(socket.SOL_SOCKET,
                        _SO_REGISTER_HANDLER,
                        lambda s : accept_ftp_connect(s, ifconfig[0]))
        ftpsockets.append(sock)
        if splash:
            print("FTP server started on {}:{}".format(ifconfig[0], port))

    datasocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    datasocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    datasocket.bind(('0.0.0.0', _DATA_PORT))
    datasocket.listen(1)
    datasocket.settimeout(10)

def restart(port=21, verbose=0, splash=True):
    stop()
    sleep_ms(200)
    start(port, verbose, splash)


start(splash=True)
