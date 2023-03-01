import logging
import network
import socket
import _thread

DEFAULT_SSID = "Live Subway Map"
DEFAULT_PASSWORD = "lsmpassword"

class AccessPoint:
    def __init__(self, ssid=DEFAULT_SSID, password=DEFAULT_PASSWORD):
        self.ssid = ssid
        self.password = password
        self.interface = network.WLAN(network.AP_IF)
        self.interface.active(False)
        while self.interface.active():
            pass
        self.active = False
    
    def start(self):
        self.interface.config(essid=self.ssid, password=self.password)
        self.interface.ifconfig(("192.168.4.1", "255.255.255.0", "192.168.4.0", "8.8.8.8"))
        self.interface.active(True)
        while not self.interface.active():
            pass
        self.active = True

    def stop(self):
        self.interface.active(False)
        while self.interface.active():
            pass
        self.active = False
        
    def get_ip(self):
        return self.interface.ifconfig()[0]
    
    def get_active(self):
        return self.active


class WifiScanner:
    def __init__(self):
        self.interface = network.WLAN(network.STA_IF)
        self.interface.active(False)

    def scan(self):
        logging.logger.info("Starting Wifi Scan...")
        self.interface.active(True)
        ssids = self.interface.scan()
        self.interface.active(False)

        logging.logger.info(ssids)
        new_ssids = []
        for ssid in ssids:
            new_ssids.append(ssid[0].decode())

        return new_ssids
    
class LAN:
    def __init__(self, ssid, password):
        self.ssid = ssid
        self.password = password
        self.interface = network.WLAN(network.STA_IF)
        self.interface.active(False)
    
    def connect(self):
        self.interface.active(True)
        self.interface.connect(self.ssid, self.password)
        logging.logger.info("Connecting to Wifi network %s with password %s", self.ssid, self.password)
        while not self.interface.isconnected():
            pass
        logging.logger.info("Connected. %s", self.interface.ifconfig())
    
class WebServer:
    def __init__(self):
        self.html = "<html>Hello</html>"
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock.bind(('', 80))
        self.sock.listen(5)

    def wait_for_response(self):
        """
        BLOCKING!!!!!!
        """
        resp = None
        while resp is None:
            client, addr = self.sock.accept()
            logging.logger.info("Client connected from '%s'", addr)
            request = client.recv(1024).decode()
            response = self.html
            logging.logger.info("Request '%s'", request)
            if "GET /?" in request:
                resp = request.split("GET /?")[-1]
                resp = resp.split("HTTP")[0]
                resp = resp.split("&")
                response = "<html>Ok</html>"
            client.send('HTTP/1.1 200 OK\n')
            client.send('Content-Type: text/html\n')
            client.send('Connection: close\n\n')
            client.sendall(response)
            client.close()
        return resp

    def set_html(self, html):
        self.html = html
