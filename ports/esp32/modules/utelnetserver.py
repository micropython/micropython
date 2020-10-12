import socket
import network
import uos
import errno
from uio import IOBase 

last_client_socket = None
server_socket = None

# Provide necessary functions for dupterm and replace telnet control characters that come in.
class TelnetWrapper(IOBase):
    def __init__(self, socket):
        self.socket = socket
        self.discard_count = 0
        
    def readinto(self, b):
        readbytes = 0
        for i in range(len(b)):
            try:
                byte = 0
                # discard telnet control characters and
                # null bytes 
                while(byte == 0):
                    byte = self.socket.recv(1)[0]
                    if byte == 0xFF:
                        self.discard_count = 2
                        byte = 0
                    elif self.discard_count > 0:
                        self.discard_count -= 1
                        byte = 0
                    
                b[i] = byte
                
                readbytes += 1
            except (IndexError, OSError) as e:
                if type(e) == IndexError or len(e.args) > 0 and e.args[0] == errno.EAGAIN:
                    if readbytes == 0:
                        return None
                    else:
                        return readbytes
                else:
                    raise
        return readbytes
    
    def write(self, data):
        # we need to write all the data but it's a non-blocking socket
        # so loop until it's all written eating EAGAIN exceptions
        while len(data) > 0:
            try:
                written_bytes = self.socket.write(data)
                data = data[written_bytes:]
            except OSError as e:
                if len(e.args) > 0 and e.args[0] == errno.EAGAIN:
                    # can't write yet, try again
                    pass
                else:
                    # something else...propagate the exception
                    raise
    
    def close(self):
        self.socket.close()

# Attach new clients to dupterm and 
# send telnet control characters to disable line mode
# and stop local echoing
def accept_telnet_connect(telnet_server):
    global last_client_socket
    
    if last_client_socket:
        # close any previous clients
        uos.dupterm(None)
        last_client_socket.close()
    
    last_client_socket, remote_addr = telnet_server.accept()
    print("Telnet connection from:", remote_addr)
    last_client_socket.setblocking(False)
    last_client_socket.setsockopt(socket.SOL_SOCKET, 20, uos.dupterm_notify)
    
    last_client_socket.sendall(bytes([255, 252, 34])) # dont allow line mode
    last_client_socket.sendall(bytes([255, 251, 1])) # turn off local echo
    
    uos.dupterm(TelnetWrapper(last_client_socket))

def stop():
    global server_socket, last_client_socket
    uos.dupterm(None)
    if server_socket:
        server_socket.close()
    if last_client_socket:
        last_client_socket.close()

# start listening for telnet connections on port 23
def start(port=23):
    stop()
    global server_socket
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    
    ai = socket.getaddrinfo("0.0.0.0", port)
    addr = ai[0][4]
    
    server_socket.bind(addr)
    server_socket.listen(1)
    server_socket.setsockopt(socket.SOL_SOCKET, 20, accept_telnet_connect)
    
    for i in (network.AP_IF, network.STA_IF):
        wlan = network.WLAN(i)
        if wlan.active():
            print("Telnet server started on {}:{}".format(wlan.ifconfig()[0], port))
