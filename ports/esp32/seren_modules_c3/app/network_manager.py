import network, asyncio, time, socket, ntptime
from aioespnow import AIOESPNow
import gc
from time import sleep_ms, localtime


class ATCP:
	def __init__(self, nvs, wlan=None):
		self.network_dict = nvs._load_networks()
		self.nvs = nvs
		self.ID = nvs.settings["device_id"]
		self.ap_ssid = self.nvs.settings["hostname"]
		self.ap_password = self.nvs.settings["password"]
		self.wlan = wlan
		self.server = None
		self.clients = set()
		self.ssid = ''
		self.password = ''
		self.message = b''
		self.flag = None
		self.monitor = None

	async def initialize(self):
		await self.check_wifis()
		if not await self.connect_wifi():
			await self.start_ap_mode()
			return True
		else:
			# Run NTP sync in the background after successful WiFi connect
			asyncio.create_task(self.sync_time_with_ntp())
		return False
	

	async def sync_time_with_ntp(self):
		"""Try to sync time with NTP server, up to 5 attempts. Update RTC if successful."""
		ntptime.host = "ptbtime1.ptb.de"
		for attempt in range(5):
			try:
				# print(f"[NTP] Attempt {attempt+1}: Syncing time with NTP server...")
				ntptime.settime()
				now = time.localtime()
				# print(f"[NTP] Time synced: {self.rtc.datetime()}")
				if self.nvs.rtc:
					self.nvs.rtc.datetime((now[0], now[1], now[2], now[6], now[3]+self.nvs.settings['timezone'], now[4], now[5], 0))
					print(f"[NTP] RTC updated: {self.nvs.rtc.datetime()}")
				break
			except Exception as e:
				print(f"[NTP] Failed to sync time: {e}")
				await asyncio.sleep(1)
		else:
			print("[NTP] All attempts to sync time failed.")


	async def check_wifis(self):
		print("Scanning available networks.....")
		scan_results = self.wlan.scan()
		wifi_ssids = {ap[0].decode('utf-8'): ap for ap in scan_results}
		for ssid, password in self.network_dict.items():
			if ssid in wifi_ssids:
				try:
					channel = wifi_ssids[ssid][2]
					self.message = f"network_manager.wlan.config(channel={channel})".encode()
					self.flag.set()
				except Exception as e:
					print(f"Error setting the flag for the sta channel: {e}")
				self.ssid = ssid
				self.password = password

	async def connect_wifi(self):
		try:
			self.wlan.connect(self.ssid, self.password)
		except Exception as e:
			print(f"OSError while connecting to network: {e}")
			print(f"Deactivating Wifi....")
			time.sleep(1)
			return False

		start_time = time.time()
		while not self.wlan.isconnected():
			if time.time() - start_time > 10:
				print(f"Timeout while connecting to {self.ssid}")
				break
			await asyncio.sleep(1)

		if self.wlan.isconnected():
			self.monitor.ap_mode = False
			print(f"Connected to {self.ssid}")
			print(f"IP address: http://{self.wlan.ifconfig()[0]}/")
			return True
		else:
			print(f"Failed to connect to {self.ssid}")
			print("Could not connect to any known network")
			return False

	async def start_ap_mode(self):
		# Activate AP interface in addition to STA (dual mode)
		ap_if = network.WLAN(network.AP_IF)
		ap_if.active(True)
		ap_if.config(essid=self.ap_ssid,
					 key=self.ap_password,
					 authmode=network.AUTH_WPA_WPA2_PSK)
		while not ap_if.active():
			await asyncio.sleep(0.1)
		print(f"Access Point started. SSID: {self.ap_ssid}, Password: {self.ap_password}")
		ap_ip = ap_if.ifconfig()[0]
		self.monitor.ap_mode = True
		asyncio.create_task(self._dns_server(ap_ip))

	async def _dns_server(self, ip):
		"""Minimal DNS server for captive portal"""
		s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		s.setblocking(False)
		s.bind(('0.0.0.0', 53))
		print(f"DNS server started on {ip}")
		while True:
			try:
				data, addr = s.recvfrom(512)
				p = DNSQuery(data)
				s.sendto(p.response(ip), addr)
			except OSError as e:
				if e.args[0] != 11:  # Ignore EAGAIN
					print(f"DNS Error: {e}")
			await asyncio.sleep(0.01)


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


class ESPNOW():
	def __init__(self, nvs, wlan):
		self.ID = nvs.settings["device_id"]
		self.brcast = b'\xff' * 6
		self.wlan = wlan
		#IRQ response time ~700-800ms. Async response ~200-300ms
		# TCP seignal response is sub 100ms.....
	
	def startup(self):
		self.e = AIOESPNow()
		self.e.active(True)
		self.e.add_peer(self.brcast)
		print("ESPNOW started")

	# broadcasts a message to all ESPNOW devices within range. Message must be in bytes
	async def abroadcast(self, message): 
		try:
#                 print("Sending broadcast: ", message)
			await self.e.asend(self.brcast, message)
		except Exception as err:
			print("Broadcast error: ", err)
	
	def broadcast(self, message):
		try:
			self.e.send(self.brcast, message)
			print("Broadcast: ", message)
		except Exception as err:
			print("Broadcast error: ", err)
	
	# broadcasts a message to all REGISTERED devices within range. Message must be in bytes
	def multicast(self, message):
		try:
			idd_message = f"{self.ID};{message}"
			print(f"Sending {message} to peers")
			self.e.send(None, idd_message)
		except Exception as err:
			print("Message not sent: ", err)
		
	
	async def amulticast(self, message): 
		try:
			idd_message = f"{self.ID};{message}"
			print(f"Sending {message} to peers")
			await self.e.asend(None, idd_message)
		except Exception as err:
			print("Peer didn't get sent the message")
				
	async def process_message(self):
		print("Initiating ESPNOW process_messages......")
		while True:
			try:
				async for mac, msg in self.e:
					decoded_msg = msg.decode()
					print("Received e-Message:", msg.decode())
					if decoded_msg.startswith("PING_CHANNEL_"):
						try:
							self.e.add_peer(mac)
						except Exception as e:
							pass
						channel = self.wlan.config('channel')
						self.e.send(mac, f"network_manager.new_channel={channel}".encode())
						self.e.send(mac, b'loop.create_task(led.a_pulse(1,1))')

					else:
						try:
							exec(decoded_msg)
						except OSError as err:
							if len(err.args) > 1 and err.args[1] == 'ESP_ERR_ESPNOW_NOT_FOUND':
								print(f"Adding {mac} to peers")
								self.e.add_peer(mac)
							await self.e.asend(mac, msg)
						except Exception as e:
							print(f"Error executing ESPNOW message: {e}")
							self._handle_command(decoded_msg)
							
			except Exception as e:
				print(f"Error in ESPNOW message processing: {e}")

	def recv_cb(self, e):
		while True:  # Read out all messages waiting in the buffer
			mac, msg = self.e.irecv(0)  # Don't wait if no messages left
			if mac is None:
				return
			exec(msg)

	def _handle_command(self, message):
		print(f"Handling command from ESPNOW: {message}")
		parts = message.strip().split("/")
		if not parts or not parts[0]:
			return
		cmd, *rest = parts
		args = []
		for arg in rest:
			try:
				args.append(float(arg))
			except ValueError:
				args.append(arg)
		func = self.nvs.commands.get(cmd)
		if func:
			func(*args)

class NetworkManager:
	def __init__(self, nvs, led=None, rot=None, loop=None):
		hostname = nvs.settings["hostname"]
		network.hostname(hostname)
		self.nvs = nvs
		self.led = led
		self.rot = rot
		self.rtc = self.nvs.rtc
		self.loop = loop
		self.wlan = None
		self.ap_mode = None
		self.espnow = None
		self.espnow_processing = None
		self.espnow_reprocessing = None
		self.atcp = None
		self.server = None
		self.microdot = None
		self.message_flag = asyncio.ThreadSafeFlag()
		self.channel = nvs.settings.get("channel", 11)
		self.channel_flag = True
		self.new_channel = None

	async def initialize_wifi(self):
		try:
			# Always ensure STA is active
			if not self.wlan:
				print(f"Initializing WiFi (STA_IF always active)...")
				self.wlan = network.WLAN(network.STA_IF)
			self.wlan.active(False)
			sleep_ms(100)
			self.wlan.active(True)
			print(f"TX power = {self.wlan.config('txpower')}")
			print("WiFi STA_IF initialized and active")
			self.ap_mode = False
			return True
		except Exception as e:
			print(f"Error initializing WiFi: {type(e).__name__}, {str(e)}")
			return False

	async def start_espnow(self):
		print("Starting ESPNOW...")
		await self.initialize_wifi()
		if not self.espnow and self.wlan:
			try:
				self.wlan.config(pm=network.WLAN.PM_NONE)
				print("Power Mode removed")
			except RuntimeError as e:
				print(f"Error setting channel: {e}")
				print(f"Continuing with current WiFi channel: {self.wlan.config('channel')}")
			self.espnow = ESPNOW(self.nvs, wlan=self.wlan)
			self.espnow.startup()
			self.espnow_processing = self.loop.create_task(self.espnow.process_message())
			self.espnow_reprocessing = self.loop.create_task(self.process_message())
			try:
				self.server.espnow = self.espnow
				print("Assigned espnow to server")
			except Exception as e:
				print(f"Error assigning espnow to server: {e}")
			# ping all channels to scan for master that might be on another channel at first boot
			if self.channel_flag:
				await self.aping_channels() 
		else:
			print("ESPNOW already started")
		return self.espnow
	
	def cleanup_espnow(self):
		if self.espnow:
			self.espnow.e.active(False)
			self.espnow = None
		if self.espnow_processing:
			self.espnow_processing.cancel()
			self.espnow_processing = None
		sleep_ms(100)  # Allow time for cleanup

	async def start_atcp(self):
		if self.led:
			self.led.stop()
			self.led.fill(0, 0, 50)
		await self.initialize_wifi()
		if not self.atcp:
			self.atcp = ATCP(self.nvs, wlan=self.wlan)
			self.atcp.flag = self.message_flag
			self.atcp.monitor = self
			if await self.atcp.initialize():
				self.ap_mode = True
			if self.rot:
				self.rot.atcp = self.atcp
		if self.led:
			self.led.fill(0, 50, 0)
			# self.led.stop()
		return self.atcp

	async def start_servers(self):
		await self.start_atcp()
		print(f"Setting up Config Server - Memory: {gc.mem_free()}")
		if not self.server:
			import app.config_server as config_server
			self.server = config_server.ConfigServer(self.nvs, self.led, self.rot, self.atcp)
			self.rot.config_server = self.server
			self.led.config_server = self.server
			self.nvs.config_server = self.server
			if self.espnow:
				self.server.espnow = self.espnow
			
			gc.collect()
			print(f"Config Server Set Up, starting server - Memory: {gc.mem_free()}")
			self.microdot = asyncio.create_task(self.server.app.start_server(debug=True, host='0.0.0.0', port=80))
			await asyncio.sleep(1)  # Allow server to start
			gc.collect()
			print(f"Server initiated - Memory: {gc.mem_free()}")

			self.server.IP = self.wlan.ifconfig()[0]
		if not self.atcp.server:
			try:
				await self.atcp.start_atcp_server()
				print("ATCP server started successfully")
			except Exception as e:
				print(f"ATCP server failed to start: {e}")
				# Continue anyway - HTTP server might still work

	async def process_message(self):
		print("Initiating Network process_messages......")
		while True:
			print("Waiting for message flag...")
			await self.message_flag.wait()
			self.message_flag.clear()
			print("Processing message: ", self.atcp.message)
			if self.espnow:
				self.espnow.broadcast(self.atcp.message)
				self.atcp.message = b''
			else:
				return
			await asyncio.sleep(0.1)

	def cleanup(self):
		self.cleanup_espnow()
		self.atcp = None
		if self.server:
			self.server.app.shutdown()
			self.microdot.cancel()
			self.microdot = None
		self.ap_mode = None
		if self.wlan:
			self.wlan.active(False)
			self.wlan = None
		sleep_ms(100)  # Allow time for cleanup


	async def aping_channels(self):
		print("Pinging channels for ESPNOW devices...")
		for channel in range(14, 0, -1):
			try:
				self.wlan.config(channel=channel)
				ping_message = f"PING_CHANNEL_{channel}"
				self.espnow.broadcast(ping_message.encode())
				print(f"Pinged channel {channel}")
				await asyncio.sleep(0.1)  # Short delay between pings
			except Exception as e:
				print(f"Error pinging channel {channel}: {e}")
		if self.new_channel:
			self.channel_flag = False
			self.wlan.config(channel=self.new_channel)
		else:
			self.wlan.config(channel=self.nvs.settings["espnow_channel"])


	async def update_device(self, host=None, branch=None, model=None):
		# Import ota here to avoid circular imports
		import ota.update
		
		if host is None:
			host = self.nvs.settings["ota_remote_host"]
		if branch is None:
			branch = self.nvs.settings["branch"]
		if model is None:
			model = self.nvs.settings["esp_model"]
			
		def update():
			if self.led:
				self.led.stop()
				self.led.fill(0, 20, 50)
			updater = ota.update.OTA(reboot=False)
			gc.collect()
			print("Setting up Updater")
			project_file = f"{model}_{branch}.bin"
			print(f"Project file: {project_file}")
			file_url = host + project_file
			
			print(f"Updating from HTTP - Memory: {gc.mem_free()}")
			updater.from_firmware_file(file_url)
			print("leaving updater")
			if self.led:
				self.led.fill(50, 0, 50)
			updater.close() 

		try:
			if self.microdot:
				self.microdot.cancel()
				await asyncio.sleep(0.5)  # Allow time for server to shut down
				self.microdot = None
			self.server = None
			gc.collect()
		except Exception as e:
			print("Failed to shutdown server: ", e) 
		try:
			await self.start_atcp()
			self.wlan.config(pm=network.WLAN.PM_NONE) # disable power management for update
			if self.ap_mode:
				print("In AP mode, skipping server restart. Please add credentials via the captive portal to connect to WiFi and receive the update.")
				await self.start_servers()
				return

			gc.collect()
			update()
			self.nvs.log.info(f"Updated to new firmware from {host} on branch {branch}")
			await asyncio.sleep(2)
			from machine import reset
			reset()
			return
		except Exception as e:
			if self.loop and self.led:
				await self.led.a_pulse(3, 1)
			print("Failed to update: ", e) 

	async def connect_wifi_on_boot(self):
		if self.nvs.settings["wifi_at_boot"]:
			await self.start_servers()

