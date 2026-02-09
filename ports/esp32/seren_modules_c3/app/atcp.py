import network, asyncio, time

class ATCP:
    def __init__(self, nvs, wlan=None, flag=None, ap_ssid="SerenSpecs", ap_password="starstar"):
        self.network_dict = nvs._load_networks()
        self.ID = nvs.settings["device_id"]
        self.ap_ssid = ap_ssid
        self.ap_password = ap_password
        self.wlan = wlan
        self.server = None
        self.clients = set()
        self.ssid = ''
        self.password = ''
        self.ap_mode = False
        self.message = b''
        self.flag = flag

    async def initialize(self):
        await self.check_wifis()
        if not await self.connect_wifi():
            return False
        return True

    async def check_wifis(self):
        if len(self.network_dict) > 0:
            print("Scanning available networks.....")
            scan_results = self.wlan.scan()
            wifi_ssids = {ap[0].decode('utf-8'): ap for ap in scan_results}
            for ssid, password in self.network_dict.items():
                if ssid in wifi_ssids:
                    self.ssid = ssid
                    self.password = password
        else:
            for ssid, password in self.network_dict.items():
                print("Only 1 SSID: ", ssid)
                self.ssid = ssid
                self.password = password

    async def connect_wifi(self):
        try:
            self.wlan.connect(self.ssid, self.password)
        except Exception as e:
            print(f"OSError while connecting to network: {e}")
            print(f"Deactivating Wifi....")
            self.wlan.active(False)
            time.sleep(1)
            return False

        start_time = time.time()
        while not self.wlan.isconnected():
            if time.time() - start_time > 10:
                print(f"Timeout while connecting to {self.ssid}")
                self.wlan.active(False)
                break
            await asyncio.sleep(1)

        if self.wlan.isconnected():
            self.ap_mode = False
            print(f"Connected to {self.ssid}")
            print(f"IP address: {self.wlan.ifconfig()[0]}")
            return True
        else:
            print(f"Failed to connect to {self.ssid}")
            print("Could not connect to any known network")
            self.wlan.active(False)
            return False

    async def start_ap(self):
        self.wlan.active(False)
        await asyncio.sleep(0.5)
        self.wlan = network.WLAN(network.AP_IF)
        self.wlan.active(True)
        self.wlan.config(essid=self.ap_ssid, password=self.ap_password, channel=11)
        while not self.wlan.active():
            await asyncio.sleep(0.1)
        print(f"Access Point started. SSID: {self.ap_ssid}, Password: {self.ap_password}")
        print(f"AP IP address: {self.wlan.ifconfig()[0]}")
        self.ap_mode = True
        self.ssid = self.ap_ssid

        # portal = CaptivePortal(self.wlan.ifconfig()[0])
        # asyncio.create_task(portal.start())

    async def start_atcp_server(self):
        self.server = await asyncio.start_server(self.handle_atcp_client, "0.0.0.0", 1099)
        print(f"ATCP server started on port, 1099")

    async def handle_atcp_client(self, reader, writer):
        try:
            self.clients.add(writer)
            print(f"New ATCP client connected: {writer.get_extra_info('peername')}")
            while True:
                data = await reader.readline()
                # print(data)
                if not data:
                    break
                self.message = data
                self.flag.set()
                exec(data)
        except Exception as e:
            print(f"Error handling ATCP client: {e}")
        finally:
            self.clients.remove(writer)
            writer.close()
            await writer.wait_closed()

    async def send_message_client(self, message):
        try:
            if self.server:
                message = f"{self.ID}; {message + "\n"}"
                for client in self.clients:
                    client.write(message.encode())
                    await client.drain()
        except Exception as e:
            print(f"Error sending message: {e}")


class CaptivePortal:
    def __init__(self, ip):
        self.ip = ip
        self.running = True
    
    async def start(self):
        import socket
        
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.setblocking(False)
        s.bind(('0.0.0.0', 53))
        
        print(f"DNS server started on {self.ip}")
        
        while self.running:
            try:
                data, addr = s.recvfrom(512)
                p = DNSQuery(data)
                s.sendto(p.response(self.ip), addr)
                print(f"DNS request for: {p.domain}")
            except OSError as e:
                if e.args[0] != 11:  # EAGAIN is expected, only print other errors
                    print(f"DNS Error: {e}")
            await asyncio.sleep(1)

class DNSQuery:
    def __init__(self, data):
        self.data = data
        self.domain = ''
        tipo = (data[2] >> 3) & 15
        if tipo == 0:
            ini = 12
            lon = data[ini]
            while lon != 0:
                self.domain += data[ini+1:ini+lon+1].decode('utf-8') + '.'
                ini += lon + 1
                lon = data[ini]

    def response(self, ip):
        packet = self.data[:2] + b'\x81\x80'
        packet += self.data[4:6] + self.data[4:6] + b'\x00\x00\x00\x00'
        packet += self.data[12:]
        packet += b'\xc0\x0c'
        packet += b'\x00\x01\x00\x01\x00\x00\x00\x3c\x00\x04'
        packet += bytes(map(int, ip.split('.')))
        return packet