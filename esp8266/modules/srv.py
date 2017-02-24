# This module should be imported from REPL, not run from command line.
import socket
import uos
import network
import time
import esp,machine

class myserver:
    def __init__(self,port=8999,name='myserver'):
        self.__name__=name
        self.port=port
        self.listen_s = None
        self.client_s = None
        self.setup_conn(port)
        print("Started server {} in normal mode".format(self.__name__))

    def setup_conn(self,port):
        self.listen_s = socket.socket()
        self.listen_s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        ai = socket.getaddrinfo("0.0.0.0", self.port)
        addr = ai[0][4]
        self.listen_s.bind(addr)
        self.listen_s.listen(1)
        self.listen_s.setsockopt(socket.SOL_SOCKET, 20, self.accept_conn)
        for i in (network.AP_IF, network.STA_IF):
            iface = network.WLAN(i)
            if iface.active():
                print("Server {} started on {}:{}".format(self.__name__,iface.ifconfig()[0], port))

    def read_from_sock(self,sock):
        print('myhandler',sock.read())

    def write_to_socket_loop(self,sock):
        pass
    def check_ok(self):
        return True
    def wrap_socket(self,cl):
        return cl
    def accept_conn(self,listen_sock):
        cl, remote_addr = listen_sock.accept()
        if not self.check_ok():
            cl.close()
            return
        print("\n{} connection from:".format(self.__name__, remote_addr))
        self.client_s = cl
        ws=self.wrap_socket(cl)
        cl.setblocking(False)
        cl.setsockopt(socket.SOL_SOCKET, 20, self.read_from_sock)
        self.write_to_socket_loop(ws)

    def close(self):
        if self.client_s:
            self.client_s.close()
        if self.listen_s:
            self.listen_s.close()
        self.client_s=None
        self.listen_s=None            


class repl(myserver):
    def __init__(self,port=23):
        super().__init__(port=port)
    def read_from_sock(self,sock):
        uos.dupterm_notify(sock)
    def write_to_socket_loop(self,sock):
        print('socket now connected to repl')
        uos.dupterm(sock)
    def close(self):
        uos.dupterm(None)
        super().close()

class webrepl(myserver):
    def __init__(self,port=8266,password=None):
        import _webrepl
        if password is None:
            try:
                import webrepl_cfg
                _webrepl.password(webrepl_cfg.PASS)
                super().__init__(port=port)
                print("Started webrepl in normal mode")
            except:
                print("WebREPL is not configured, run 'import webrepl_setup'")
        else:
            _webrepl.password(password)
            super().__init__(port=port)
            print("Started webrepl in manual override mode")
    def check_ok(self):
        return True
    def wrap_socket(self,cl):
        import websocket, websocket_helper, _webrepl
        websocket_helper.server_handshake(cl)
        ws = websocket.websocket(cl, True)
        ws = _webrepl._webrepl(ws)
        return ws
    def read_from_sock(self,sock):
        uos.dupterm_notify(sock)
    def write_to_socket_loop(self,sock):
        print('socket now connected to repl')
        uos.dupterm(sock)
    def close(self):
        uos.dupterm(None)
        super().close()
