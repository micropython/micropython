# MicroPython BLE class for ESP32
import uasyncio as asyncio
import aioble
import bluetooth
from collections import deque 

from app.aioble_ota import AiobleOTA
# import time

class BLE:
	def __init__(self, nvs, loop):
		self.nvs = nvs
		self.loop = loop
		self.espnow = None
		self.name = nvs.settings.get('hostname', 'ZenSpecs') + "_" + nvs.settings.get('device_id', 'noidea')
		self.connected = False
		self.conn = None
		self.mtu = 20
		self.msg_event = asyncio.Event()
		self.tx_msg_event = asyncio.ThreadSafeFlag()
		self.inbound = bytearray(512)
		self.outbound = deque((), 10)
		self.rx_task = None
		self.tx_task = None
		self._ota_task = None
		self.ota_helper = AiobleOTA(self, status_cb=self.notify_immediate)
		self._ota_paused = False

		# UUIDs
		self.SERVICE_UUID = bluetooth.UUID('bb0823d2-1660-4123-b3f2-800b37b09106')
		self.RX_UUID = bluetooth.UUID('bbc70aee-10e3-4dd6-991f-c20a74b92ab5')
		self.TX_UUID = bluetooth.UUID('690a9212-5a98-434a-97a3-679e9269fd36')

		# GATT
		service = aioble.Service(self.SERVICE_UUID)
		self.tx_char = aioble.Characteristic(service, self.TX_UUID, notify=True)
		self.rx_char = aioble.BufferedCharacteristic(service, self.RX_UUID, read=True, write=True, capture=True, max_len=512)
		aioble.register_services(service)

		# Tasks
		self.peripheral_task = self.loop.create_task(self._peripheral_loop())
		self.recv_task = self.loop.create_task(self._process_commands())

		# Timing variables
		self.read_time = 0
		self.process_time = 0
		self.write_time = 0


	async def _peripheral_loop(self):
		_ADV_INTERVAL_US = 500_000
		while True:
			adv = aioble.advertise(
				interval_us=_ADV_INTERVAL_US,
				services=[self.SERVICE_UUID],
				name=self.name
			)
			try:
				self.conn = await adv
				self.connected = True
				await self._start_transport_tasks()
				self.loop.create_task(self.a_stream_settings())
				# self.nvs.led.fill(0, 0, 150)
				print("BLE Connected to App:", self.conn.device if self.conn else "")
				await self.conn.disconnected()
			finally:
				self.connected = False
				print("BLE Device Disconnected")
				await self._stop_transport_tasks()
				self.conn = None

	async def _rx_loop(self):
		while True:
			try:
				connection, chunk = await self.rx_char.written()
				if not chunk:
					continue
				self.inbound = chunk
				self.msg_event.set()

			except aioble.DeviceDisconnectedError:
				raise
			except Exception as e:
				print("Error processing BLE message:", e)

	async def _process_commands(self):
		while True:
			try:
				await self.msg_event.wait()
				self.msg_event.clear()
				self._handle_command()
				await asyncio.sleep(0)
			except Exception as e:
				print("Error in command processing:", e)

	def _handle_command(self):
		message = self.inbound
		# OTA uses binary writes; never let invalid UTF-8 crash command parsing.
		try:
			decoded = message.decode().strip()
		except Exception:
			return
		parts = decoded.split("/")
		print("BLE message received:", message)
		if not parts or not parts[0]:
			return
		cmd, *rest = parts
		if cmd == "ota_begin" and self.ota_helper:
			if len(rest) < 2:
				self.notify_immediate("ota/error/args")
				return
			try:
				total_bytes = int(rest[0])
			except ValueError:
				self.notify_immediate("ota/error/size")
				return
			sha_hex = rest[1]
			self._schedule_ota(total_bytes, sha_hex)
			return
		if cmd == "exec":
			exec(parts[1])
			return
		args = []
		for arg in rest:
			try:
				args.append(float(arg))
			except ValueError:
				args.append(arg)
		func = self.nvs.commands.get(cmd)
		if cmd.startswith("a_"):
			self.loop.create_task(func(*args))
		else:
			func(*args)
		if self.espnow:
			self.espnow.broadcast(message)

	def send(self, data):
		if not self.connected:
			# print("BLE not connected, cannot send data")
			return
		# During OTA, normal queued sends must be dropped to avoid queue growth
		# while transport tasks are paused.
		if self.ota_helper and self.ota_helper.active:
			return
		self.outbound.append(data)
		# print(f"BLE queued message for sending: {data}")
		self.tx_msg_event.set()

	
	async def _tx_loop(self):
		while True:
			try:
				await self.tx_msg_event.wait()
				self.tx_msg_event.clear()
				message = self.outbound.popleft()
				if isinstance(message, str):
					message = message.encode()
				print(f"BLE sending message: {message}")
				self.tx_char.notify(self.conn, message)
				await asyncio.sleep(0.05)
				if self.outbound:
					self.tx_msg_event.set()
			except Exception as e:
				print("Error sending BLE message:", e)


	def stream_settings(self):
		self.loop.create_task(self.a_stream_settings())
	
	async def a_stream_settings(self):
		await asyncio.sleep(2)  # wait a moment to ensure connection is stable
		for key, value in self.nvs.settings.items():
			message = f"set/{key}/{value}"
			self.send(message)
			await asyncio.sleep(0.05)  # slight delay to avoid overwhelming the connection
		for key, value in self.nvs.rot.values.items():
			message = f"vals/{key}/{round(value[0], 2)}"
			self.send(message)
			await asyncio.sleep(0.05)  # slight delay to avoid overwhelming the connection

	
	
	
	
	def notify_immediate(self, data):
		if not self.connected or not self.conn:
			return
		if isinstance(data, str):
			data = data.encode()
		try:
			self.tx_char.notify(self.conn, data)
		except Exception as e:
			print("Error sending immediate BLE data:", e)

	async def _start_transport_tasks(self):
		if not self.conn:
			return
		if self.rx_task is None:
			self.rx_task = self.loop.create_task(self._rx_loop())
		if self.tx_task is None:
			self.tx_task = self.loop.create_task(self._tx_loop())

	async def _stop_transport_tasks(self):
		for attr in ("rx_task", "tx_task"):
			task = getattr(self, attr)
			if task is None:
				continue
			task.cancel()
			try:
				await task
			except asyncio.CancelledError:
				pass
			setattr(self, attr, None)

	async def pause_transport_tasks(self):
		self._ota_paused = True
		try:
			self.outbound.clear()
		except Exception:
			pass
		await self._stop_transport_tasks()

	async def resume_transport_tasks(self):
		self._ota_paused = False
		if self.connected:
			await self._start_transport_tasks()

	def _schedule_ota(self, total_bytes, sha_hex):
		if not self.ota_helper:
			print("OTA helper not configured")
			return
		if self._ota_task and not self._ota_task.done():
			self.notify_immediate("ota/error/busy")
			return
		self.nvs.led.xp_on = True
		self._ota_task = self.loop.create_task(
			self.ota_helper.start_session(total_bytes, sha_hex)
		)




