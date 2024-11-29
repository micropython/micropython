#### COPYRIGHT AMAS TECHNOLOGIES ####
## LICENSED UNDER AGPL-3.0 ##
#### COPYRIGHT AMAS TECHNOLOGIES ####

import network, gc, os
from httpclient import HttpClient
from machine import WDT
from ubinascii import hexlify
from time import sleep, localtime, time, gmtime
from sht3x import SHT3x_Sensor
from ota_updater import OTAUpdater
from ujson import loads, dumps
from ds3231_port import DS3231
import machine, sys, esp32
import uasyncio as asyncio
from ntptime import settime
from microdot import Microdot, Response
from microdot.websocket import with_websocket
from microdot.cors import CORS
from mqtt_as import MQTTClient, config
import uhashlib, random, string
from httpclient import HttpClient
import ssl
from binascii import a2b_base64


### Firmware Version ###
FIRMWARE_VERSION = 'v0.22.3'


### Check debug mode ###
# Force debug in pre-release firmwares
if FIRMWARE_VERSION.split('.')[0] == 'v0':
	debug = True
else:
	try:
		with open('debug'):
			debug = True
	except: debug = False


### Device ID and API Key ###

# NVS partition initialization
nvs = esp32.NVS('nvs')
custom_device = False
custom_device_setup = False
# Always try files first before NVS partition
# Encrpytion key and MAC encryption key (necessary for function)
try:
	with open('api_key', 'r') as cust_api_key_file:
		api_key = cust_api_key_file.readline().strip()
except:
	try:
		api_key = bytearray(32)
		nvs.get_blob(b'api_key',api_key)
		api_key = api_key.decode()
		gc.collect()
	except:
		sys.exit()
# Platform (defaults to esp32-GENERIC)
try:
	with open('platform') as platform_file:
		platform = platform_file.read().strip()
	custom_device = True
except:
	try:
		platform = bytearray(25)
		nvs.get_blob(b'platform', platform)
		platform = platform.decode().split('\x00')[0]
	except:
		platform = 'ESP32_GENERIC'
# Device ID
try:
	with open('device_id') as cust_dev_id_file:
		device_id = cust_dev_id_file.read()
	custom_device = True
except:
	try:
		device_id = bytearray(64)
		nvs.get_blob(b'device_id', device_id)
		device_id = device_id.decode().split('\x00')[0]
	except:
		device_id = 'nodeviceidset_'+str(time())
		custom_device_setup = True
# Serial and DHCP hostname (for custom devices, device serial is the same as username)
try:
	with open('username') as cust_username_file:
		lines=len(cust_username_file.readlines())
	with open('username') as cust_username_file:
		if lines == 1:
			serial = dhcp_hostname = cust_username_file.read().strip()
		elif lines == 2:
			serial = cust_username_file.readline().strip()
			dhcp_hostname = cust_username_file.readline().strip()
		else: raise Exception
	custom_device = True
except:
	try:
		serial = bytearray(25)
		nvs.get_blob(b'serial', serial)
		serial = serial.decode().split('\x00')[0]
		dhcp_hostname = False
	except:
		serial = 'noserialset_'+str(time())
		custom_device_setup = True

if debug or custom_device: print('Loaded device identifiers!')

# Start Watchdog Timer
wdt = WDT(timeout=60000)


### SYNC FUNCTIONS ###

# WiFi Connect Helper
def wifi_connect(ssid, password):
	global serial, custom_device
	if custom_device:
		try:
			global dhcp_hostname
			if dhcp_hostname:
				network.hostname('amas'+dhcp_hostname.lower())
			else: raise Exception
		except:
			network.hostname('amas'+serial.lower())
	else: network.hostname('amas'+serial.lower())
	station = network.WLAN(network.STA_IF)
	# Stop SoftAP is active
	if network.WLAN(network.AP_IF).active():
		network.WLAN(network.AP_IF).active(False)
	# Start WiFi client mode
	station.active(True)
	if debug or custom_device: print('Connecting to: '+ssid)
	station.connect(ssid, password)
	# Raise exception unless network is connected within 15 seconds
	for i in range (15):
		sleep(1)
		if station.isconnected():
			break
		else:
			if i == 14:
				station.active(False)
				raise OSError

# Access Point Helper
def soft_ap():
	global api_key, serial
	ap = network.WLAN(network.AP_IF)
	# Deactivate WiFi client mode
	if network.WLAN(network.STA_IF).active():
		network.WLAN(network.STA_IF).active(False)
	ap.active(True)
	# Start SoftAP with the api_key as the passphrase
	ap.config(essid='amas' + serial.lower(), authmode=network.AUTH_WPA_WPA2_PSK, password=api_key)

# Read file in chunks
def read_in_chunks(file_object, chunk_size=1024):
	while True:
		data = file_object.read(chunk_size)
		if not data:
			break
		yield data

# Create pseudo-random key of pre-defined length
def create_random_key(n=16):
	rand_key = ''
	for i in range(0,n):
		rand_key = rand_key + random.choice(string.ascii_lowercase + string.digits)
	return rand_key

# Unit State difference finder (lowers MQTT messaging costs)
def unit_state_diff(old, new):
	diff = {}
	for i in new:
		if type(new[i]) is dict:
			for j in new[i]:
				if type(new[i][j]) is dict:
					for k in new[i][j]:
						if new[i][j][k] != old[i][j][k]:
							try:
								diff[i][j][k]= new[i][j][k]
							except:
								try:
									diff[i][j] = {}
									diff[i][j][k]= new[i][j][k]
								except:
									diff[i] = {}
									diff[i][j] = {}
									diff[i][j][k]= new[i][j][k]
				else:
					if new[i][j] != old[i][j]:
						try:
							diff[i][j]= new[i][j]
						except:
							diff[i] = {}
							diff[i][j]= new[i][j]
		else:
			if new[i] != old[i]:
				diff[i]= new[i]
	return diff

# Standard epoch second finder (some espressif modules use epoch of 2000)
def unix_epoch_time():
	if gmtime(0)[0] == 2000:
		iso = time()+946684800 # converting epoch of 2000-01-01 00:00:00 to standard UNIX epoch
	else:
		iso = time()
	return iso

		
### ASYNC FUNCTIONS ###

# Ambient Temperature Humidity Sensor
async def ambient_sensor():
	global amb_scl, amb_sda
	try:
		# Check if SHT30 is connected
		sht30 = SHT3x_Sensor(freq=100000, sclpin=amb_scl, sdapin=amb_sda)
	except:
		# Return errors otherwise
		return -9999, -9999
	# Read from the sensor, return errors otherwise
	try:
		data = sht30.read_temp_humd()
	except:
		asyncio.sleep(1)
		try:
			data = sht30.read_temp_humd()
		except:
			asyncio.sleep(1)
			try:
				data = sht30.read_temp_humd()
			except:
				return -9999, -9999
	# temperature = data[0]
	# humdity = data[1]
	if int(data[0]) == -45 and int(data[1]) == 0:
		return -9999, -9999
	return data[0], data[1]

# Analog Water Sensor
async def water_sensor():
	global water_power, analog_water_sensor
	water_power.value(1)
	asyncio.sleep(0.10)
	water_level = analog_water_sensor.read()
	water_level = water_level/4500*100
	water_power.value(0)
	if water_level < 15:
		water_level = -9999
	return water_level

# Data Light Activity
async def dat_light_act():
	global dat_light
	dat_light.value(1)
	asyncio.sleep(0.05)
	dat_light.value(0)

# Error Light Activity
async def err_light_act():
	global err_light
	err_light.value(1)
	asyncio.sleep(0.1)
	err_light.value(0)

# Configure (change) unit states according to user signals
async def configurator(unit_state={}, query_state={}):
	global pump, light
	rebooting = False
	# Get existing states
	# Light state
	with open('light', 'r') as light_file:
		light_on = light_file.readline().strip()
		light_off = light_file.readline().strip()
	gc.collect()
	# Pump state
	with open('pump', 'r') as pump_file:
		try:
			pump_powered = bool(int(pump_file.readline().strip()))
			interval = int(pump_file.readline().strip())
			runtime = int(pump_file.readline().strip())
		except:
			pump_powered = bool(pump_file.readline())
			interval = int(pump_file.readline().strip())
			runtime = int(pump_file.readline().strip())
	gc.collect()
	if unit_state:
		if debug or custom_device: print(unit_state)
		# Check if user is controlling lights
		if 'light' in unit_state:
			try:
				# Validate user input
				if int(unit_state['light']['on']) in range(0,2400):
					if int(unit_state['light']['on'][:2]) in range(0,24) and int(unit_state['light']['on'][2:]) in range(0,60):
						light_on = unit_state['light']['on']
			except: pass
			try:
				# Validate user input
				if int(unit_state['light']['off']) in range(0,2400):
					if int(unit_state['light']['off'][:2]) in range(0,24) and int(unit_state['light']['off'][2:]) in range(0,60):
						light_off = unit_state['light']['off']
			except: pass
			# Save updated light schedule
			with open('light', 'w') as light_file:
				light_file.write(light_on+'\n'+light_off+'\n')
			# Check if user is overriding light schedule
			try:
				if (unit_state['light']['override'] == '1') or (unit_state['light']['override'] == '0') or (unit_state['light']['override'] == False):
					# If light schedule is being overriden
					if unit_state['light']['override'] != False:
						light.value(int(unit_state['light']['override']))
						with open('light_override', 'w') as light_override_file:
							light_override_file.write(unit_state['light']['override'])
					# Remove transient override indicator otherwise
					else:
						try:
							os.remove('light_override')
						except: pass
			# Follow light schedule otherwise
			except:
				# Get time
				try:
					ds_time = ds3231.get_time(set_rtc=False)
				except:
					ds_time = localtime()
				# Get military time string
				military = str(ds_time[3])+(str(ds_time[4]) if len(str(ds_time[4])) == 2 else '0'+str(ds_time[4]))
				# Turn off light if schedule is disabled
				if light_off == light_on:
					light.value(0)
				# Control light according to schedule otherwise
				elif int(light_on) < int(light_off):
					if int(military) in range(int(light_on), int(light_off)):
						light.value(1)
					else:
						light.value(0)
				elif int(light_on) > int(light_off):
					if int(military) in range(int(light_on), 2400) or int(military) in range(0, int(light_off)):
						light.value(1)
					else:
						light.value(0)
				del military, ds_time
				gc.collect()
		# Check if user is controlling pump
		if 'pump' in unit_state:
			# Check if user is controlling pump powered state
			try:
				if (unit_state['pump']['powered'] == False) or (unit_state['pump']['powered'] == True):
					with open('pump', 'w') as pump_file:
						pump_file.write(str(int(unit_state['pump']['powered']))+'\n'+str(interval)+'\n'+str(runtime)+'\n')
					pump_powered = unit_state['pump']['powered']
				# Turn off pump if pump schedule is being turned off
				if not pump_powered:
						pump.value(0)
				gc.collect()
			except: pass
			# Check if user is controlling the interval between two pump runtimes
			try:
				if type(unit_state['pump']['interval']) is int and unit_state['pump']['interval'] in range(600,86401):
					with open('pump', 'w') as pump_file:
						pump_file.write(str(int(pump_powered))+'\n'+str(unit_state['pump']['interval'])+'\n'+str(runtime)+'\n')
					interval = unit_state['pump']['interval']
				if type(unit_state['pump']['interval']) is not int and int(unit_state['pump']['interval']) in range(600,86401):
					pump_file.write(str(int(pump_powered))+'\n'+str(int(unit_state['pump']['interval']))+'\n'+str(runtime)+'\n')
					interval = int(unit_state['pump']['interval'])
				gc.collect()
			except: pass
			# Check if user is controlling the pump runtime
			try:
				if type(unit_state['pump']['runtime']) is int and unit_state['pump']['runtime'] in range(60,86401):
					with open('pump', 'w') as pump_file:
						pump_file.write(str(int(pump_powered))+'\n'+str(interval)+'\n'+str(unit_state['pump']['runtime'])+'\n')
					runtime = unit_state['pump']['runtime']
				if type(unit_state['pump']['runtime']) is not int and int(unit_state['pump']['runtime']) in range(60,86401):
					pump_file.write(str(int(pump_powered))+'\n'+str(interval)+'\n'+str(int(unit_state['pump']['runtime'])+'\n'))
					runtime = int(unit_state['pump']['runtime'])
				gc.collect()
			except: pass
			# Check if user is overriding pump run schedule (usually used while draining the reservoir)
			try:
				if unit_state['pump']['drain']:
					with open('drain_check', 'w') as drain_file:
						drain_file.write('1')
					pump.value(1)
				else:
					try:
						os.remove('drain_check')
					except:
						pass
					pump.value(0)
			except: pass
		# Check if user is changing auto_update preference
		if 'auto_update' in unit_state:
			auto_update = unit_state['auto_update']
			if auto_update:
				try:
					os.remove('no_auto_update')
				except: pass
			else:
				with open('no_auto_update') as no_update_file:
					no_update_file.write('1')
		# Check if changing anonymous_data_collection
		if 'anonymous_data_collection' in unit_state:
			anon_data_collection = unit_state['anonymous_data_collection']
			if anon_data_collection:
				try:
					os.remove('no_telemetry')
				except: pass
			else:
				with open('no_telemetry') as no_telemetry_file:
					no_telemetry_file.write('1')
		# Check if setting friendly name
		if 'friendly_name' in unit_state:
			with open('name', 'w') as name_file:
				name_file.write(str(unit_state['friendly_name']))
	if query_state:
		if debug or custom_device: print(query_state)
		# Check if user is restarting the device
		try:
			if query_state['restart']:
				for i in range(3):
					await dat_light_act()
				# Create reboot task
				asyncio.create_task(machine_reset_10_seconds())
				rebooting = True
		except: pass
		# Check if user is resetting network
		try:
			if query_state['network-reset']:
				for i in range(3):
					await dat_light_act()
				try:
					# Delete wifi info
					os.remove('wifi')
				except:
					pass
				# Create reboot task
				asyncio.create_task(machine_reset_10_seconds())
				rebooting = True
		except: pass
		# Check if user is factory resetting device
		try:
			if query_state['factory-reset']:
				try:
					with open('customer.json', 'r') as cust_file:
						cust_info = loads(cust_file.read())
					stale_cert_id = cust_info['cert_id']
					stale_cust_id = cust_info['cust_id']
				except: pass
				for i in range(3):
					await dat_light_act()
				# Delete all user generated files
				for i in os.listdir('/'):
					try:
						os.remove(i)
					except:
						pass
				# Delete stale cert
				http_client = HttpClient()
				try:
					try:
						resp= http_client.post('https://devicecert.amastech.cloud/', json={'del_cert': {'cust_id': stale_cust_id, 'cert_id': stale_cert_id}})
					except:
						await asyncio.sleep(1)
						gc.collect()
						resp= http_client.post('https://devicecert.amastech.cloud/', json={'del_cert': {'cust_id': stale_cust_id, 'cert_id': stale_cert_id}})
				except:
					# Save stale cert info for deletion on next setup
					with open('stale_cert', 'w') as cert_id_file:
						cert_id_file.write(dumps({'cust_id': stale_cust_id, 'cert_id': stale_cert_id}))
				# Create reboot task
				asyncio.create_task(machine_reset_10_seconds())
				rebooting = True
		except: pass
	if rebooting:
		return True
	return False

# Setup unit
async def setuper(setup_dic):
	global wifi_ch, cust_ch, custom_device_setup
	flag = False
	# Check if setting friendly name
	if 'friendly_name' in setup_dic:
		with open('name', 'w') as name_file:
			name_file.write(str(setup_dic['friendly_name']))
	# Check if WiFi needs to be set up
	if wifi_ch:
		try:
			# Save WiFi info on device
			with open('wifi', 'w') as wifi_file:
				wifi_file.write(str(setup_dic['ssid'])+'\n'+str(setup_dic['password']))
			flag = True
			await dat_light_act()
		except: pass
	# Check if device needs to be registereed
	if cust_ch:
		try:
			# Save customer id token on device to set up on next reboot
			with open('cust_id_token', 'w') as cust_file:
				cust_file.write(setup_dic['id_token'])
			flag = True
			await dat_light_act()
		except: pass
	# Check if the device is custom dev device
	if custom_device_setup:
		try:
			# Save customer username on device
			with open('username', 'w') as username_file:
				username = setup_dic['username']
				dhcp_hostname = username+str(time())
				username_file.write(username+'\n'+dhcp_hostname)
			flag = True
			await dat_light_act()
		except: pass
		try:
			# Save validated device ID
			with open('device_id', 'r') as cust_dev_id_file:
				cust_dev_id_file.write(setup_dic['device_id'])
			flag = True
			await dat_light_act()
		except: pass
	# Reboot device if any of the above tasks were performed
	if flag:
		asyncio.create_task(machine_reset_10_seconds())
		return True
	return False

# Unit State Finder
async def unit_state_finder():
	global pump, light, device_id, serial, custom_device, api_key, cust_ch
	# API key
	api_key_json = {'api_key': api_key}
	# Get internal IP address
	station = network.WLAN(network.STA_IF)
	if station.active():
		ip = station.ifconfig()[0]
	else:
		ip = '192.168.4.1'
	# Get Pump status
	with open('pump', 'r') as pump_file:
		pump_powered = bool(int(pump_file.readline().strip()))
		pump_interval = int(pump_file.readline().strip())
		pump_runtime = int(pump_file.readline().strip())
	# Get pump run state
	pump_status = bool(pump.value())
	# Get light schedule
	with open('light', 'r') as light_file:
		light_on = light_file.readline().strip()
		light_off = light_file.readline().strip()
	# Get light status
	light_status = bool(light.value())
	# Check if draining
	try:
		f = open('drain_check', 'r')
		drain_status = True
		f.close()
	except:
		drain_status = False
	# Check if light override
	try:
		with open ('light_override', 'r') as l_override:
			light_override_status = l_override.read()
	except: light_override_status = False
	# Check if auto_update is turned off
	try:
		with open('no_auto_update') as no_update_file:
			auto_update = False
	except: auto_update = True
	# Check if anonymous data collection is turned off
	try:
		with open('no_telemetry') as telm_file:
			anon_data_collection = False
	except:
		anon_data_collection = True
	# Garbage collection
	gc.collect()
	# Make payload
	payload = {
		'device_id': device_id,
		'serial': serial,
		'light': {
			'status': light_status,
			'on': light_on,
			'off': light_off,
			'override': light_override_status
		},
		'pump': {
			'powered': pump_powered,
			'interval': pump_interval,
			'runtime': pump_runtime,
			'status': pump_status,
			'drain': drain_status
		},
		'internal_ip': ip,
		'auto_update': auto_update,
		'anonymous_data_collection': anon_data_collection
	}
	del ip, light_on, light_off
	gc.collect()
	payload.update(api_key_json)
	# Check if registered to user
	try:
		with open('customer.json') as cust_file:
			cust_info = loads(cust_file.read())
		cust_id = cust_info['cust_id']
		cert_exp = cust_info['cert_exp']
		iso = unix_epoch_time()
		# Allow certificate rotation 30 days before expiration
		if iso > float(cert_exp):
			os.remove('customer.json')
			machine.reset()
		elif iso >= float(cert_exp)-30*24*3600:
			cust_ch = 1
			cert_exp_check = True
		else:
			cert_exp_check = False
		del cust_info
		gc.collect()
		payload.update({
			'customer_id': cust_id,
			'device_certificate': {
				'expiration:': cert_exp,
				'renewal_period_active': cert_exp_check
			}
		})
	except: 
		pass
	# Check friendly name
	try:
		with open('name') as name_file:
			friendly_name = name_file.read()
		payload.update({'friendly_name': friendly_name})
	except: pass
	# Check if custom device
	if custom_device:
		try:
			global dhcp_hostname
			payload.update({'dhcp_hostname': dhcp_hostname})
		except: pass
	# Update version
	payload.update({'firmware_version': FIRMWARE_VERSION})
	# Check alerts
	with open('alerts.json', 'r') as alert_file:
		alert_msg = loads(alert_file.read())
	payload.update(alert_msg)
	return payload

# Unit Sensor States to make full Payload
async def payload_maker(unit_state='run_finder'):
	# Get sensor data
	ambient_temp, humidity = await ambient_sensor()
	water_level = await water_sensor()
	payload = {
		'sensors': {
			'environment': {
				'temperature': ambient_temp,
				'humidity': humidity,
			},
			'water_level': water_level
		}
	}
	if unit_state:
		# If no unit state is given
		if unit_state == 'run_finder':
			payload.update(await unit_state_finder())
		# If unit state is given
		else:
			payload.update(unit_state)
	# Garbage collection
	del ambient_temp, humidity, water_level
	gc.collect()
	return payload

# Anonymized payload maker
async def anon_data_maker(payload):
	anon_payload = {}
	iso = unix_epoch_time()
	if payload:
		# Get state data
		payload = payload['state']['reported']
		# Build anon json from payload (no identifying info can be stored)
		for i in payload:
			if i in ['light', 'pump', 'firmware_version']:
				if type(payload[i]) is dict:
					for j in payload[i]:
						anon_payload[i+'_'+j] = payload[i][j]
				else:
					anon_payload[i] = payload[i]
			if i == 'sensors':
				for j in payload[i]:
					if type(payload[i][j]) is dict:
						for k in payload[i][j]:
							if not payload[i][j][k] == -9999:
								anon_payload[j+'_'+k] = payload[i][j][k]
					else:
						if not payload[i][j] == -9999:
							anon_payload[j] = payload[i][j]

	anon_payload.update({
		'iso': iso
		})
	
	return anon_payload

# Restart machine in 5 seconds
async def machine_reset_10_seconds():
	if debug or custom_device: print('Restarting device in 10 seconds!')
	await asyncio.sleep(10)
	machine.reset()

# Check for network reset button press
async def network_reset_button_checker():
	pass
"""
	global network_reset_button
	while True:
		state = network_reset_button.value()
		if state:
			# Start button press countdown
			await asyncio.sleep(3)
			state = network_reset_button.value()
			if state:
				# If button is still pressed after 3 seconds, reset
				await configurator({'network-reset': 1})
		await asyncio.sleep(2)
"""

# Check for hard reset button press
async def hard_reset_button_checker():
	pass
"""
	global hard_reset_button
	while True:
		state = hard_reset_button.value()
		if state:
			# Start button press countdown
			await asyncio.sleep(5)
			state = hard_reset_button.value()
			if state:
				# If button is still pressed after 5 seconds, reset
				await configurator({'factory-reset': 1})
		await asyncio.sleep(2)
"""

# Pump Checker
async def pump_check():
	global pump
	# First default interval
	await asyncio.sleep(600)
	while True:
		try:
			# If draining, pump schedule is bypassed
			with open('drain_check', 'r') as drain_file:
				pump.value(1)
		except:
			with open('pump', 'r') as pump_file:
				try:
					pump_powered = bool(int(pump_file.readline().strip()))
					interval = int(pump_file.readline().strip())
					runtime = int(pump_file.readline().strip())
				except:
					pump_powered = bool(pump_file.readline().strip())
					interval = int(pump_file.readline().strip())
					runtime = int(pump_file.readline().strip())
			gc.collect()
			# If pump is powered
			if pump_powered:
				# Turn on pump
				pump.value(1)
				# Keep the pump running for specified runtime
				await asyncio.sleep(runtime)
				# Turn pump off after specified runtime
				pump.value(0)
			else:
				# If pump is not powered, keep it off
				pump.value(0)
		# Wait for the next specified interval
		await asyncio.sleep(interval)

# Light checker
async def light_check():
	global light
	# See if external RTC is connected and get time
	try:
		global ds3231
		ds_time = ds3231.get_time(set_rtc=False)
	except: 
		# Use internal RTC otherwise
		ds_time = localtime()
	while True:
		try:
			# Check if light schedule is being overriden
			with open('light_override', 'r') as light_over_file:
				light_over = int(light_over_file.readline().strip())
				if light_over:
					light.value(1)
				else:
					light.value(0)
		except:
			# If light schedule is being followed
			with open('light', 'r') as light_file:
				light_on = light_file.readline().strip()
				light_off = light_file.readline().strip()
			gc.collect()				
			# Get time in military format
			military = str(ds_time[3])+(str(ds_time[4]) if len(str(ds_time[4])) == 2 else '0'+str(ds_time[4]))
			# Check if light schedule is turned off
			if light_off == light_on:
				light.value(0)
			# If light on has an earlier time than light off
			elif int(light_on) < int(light_off):
				if int(military) in range(int(light_on), int(light_off)):
					light.value(1)
				else:
					light.value(0)
			# If light on has a later time than light on
			elif int(light_on) > int(light_off):
				if int(military) in range(int(light_on), 2400) or int(military) in range(0, int(light_off)):
					light.value(1)
				else:
					light.value(0)
		await asyncio.sleep(30)

# Alert Check
async def alert_check():
	while True:
		# Do not check for alerts while pump is running
		if not pump.value():
			# Get water level
			water_level = await water_sensor()
			if water_level == -9999:
				water_alert = 'Sensor Error'
			elif water_level < 25:
				water_alert = 'Low'
			else:
				water_alert = False
			# Get temperature and humidity limits
			with open('ambient_limit') as ambient_limit_file:
				temp_high = int(ambient_limit_file.readline().strip())
				temp_low = int(ambient_limit_file.readline().strip())
				humid_high = int(ambient_limit_file.readline().strip())
				humid_low = int(ambient_limit_file.readline().strip())
			# Get temperature and humidity
			temp, humid = await ambient_sensor()
			if temp > temp_high:
				temp_alert = 'High'
			elif temp == -9999:
				temp_alert = 'Sensor Error'
			elif temp < temp_low:
				temp_alert = 'Low'
			else:
				temp_alert = False
			if humid > humid_high:
				humid_alert = 'High'
			elif humid == -9999:
				humid_alert = 'Sensor Error'
			elif humid < humid_low:
				humid_alert = 'Low'
			else:
				humid_alert = False
			# Make alert payload
			alert_payload = {
				'alerts': {
					'water_level_alert': water_alert,
					'temperature_alert': temp_alert,
					'humidity_alert': humid_alert
				}
			}
			# Write alert payload to file
			with open('alerts.json', 'w') as alert_file:
				alert_file.write(dumps(alert_payload))
			del alert_payload, alert_file
			gc.collect()
		# Feed watchdog regularly
		wdt.feed()
		await asyncio.sleep(10)

# No cust check light
async def no_cust_no_wifi_light():
	global err_light
	while True:
		err_light.value(1)
		await asyncio.sleep(0.5)
		err_light.value(0)
		await asyncio.sleep(1)

# No WiFi check light
async def no_wifi_light():
	global err_light
	while True:
		err_light.value(1)
		await asyncio.sleep(0.1)
		err_light.value(0)
		await asyncio.sleep(0.05)
		err_light.value(1)
		await asyncio.sleep(0.1)
		err_light.value(0)
		await asyncio.sleep(0.05)
		err_light.value(1)
		await asyncio.sleep(0.1)
		err_light.value(0)
		await asyncio.sleep(1)

# No cust check light
async def no_cust_light():
	global err_light
	while True:
		err_light.value(1)
		await asyncio.sleep(0.1)
		err_light.value(0)
		await asyncio.sleep(0.05)
		err_light.value(1)
		await asyncio.sleep(0.25)
		err_light.value(0)
		await asyncio.sleep(0.05)
		err_light.value(1)
		await asyncio.sleep(0.1)
		err_light.value(0)
		await asyncio.sleep(1)
		
# Time corrector
async def time_corrector():
	while True:
		# Set internal RTC with NTP
		try:
			settime()
		except:
			await asyncio.sleep(120)
			try:
				settime()
			except:
				await asyncio.sleep(120)
				try:
					settime()
				except:
					pass
		# Set external RTC if available
		try:
			global ds3231
			ds3231.save_time()
		except: pass
		await asyncio.sleep(86400)
			
# WiFi Check
async def wifi_check():
	global wifi_ch, wifi_error_on_boot
	while True:
		station = network.WLAN(network.STA_IF)
		# If WiFi is not connected
		if not station.active():
			try:
				# Try to connect to WiFi
				wifi_connect(ssid, password)
				# Reboot if WiFi errored out on boot
				if wifi_error_on_boot:
					machine.reset()
			except:
				# If WiFi does not connect, run AP to set WiFi
				wifi_ch = 1
				soft_ap()
		del station
		gc.collect()
		await asyncio.sleep(600)

# Auto Update
async def auto_update():
	try:
		global ds3231
	except: pass
	# Restart every seven days
	counter = 0
	while True:
		try:
			with open('no_auto_update') as auto_update_check:
				pass
			sleep_time = 900
		except:
			# Get time
			try:
				ds_time = ds3231.get_time(set_rtc=False)
			except:
				ds_time = localtime()
			military = str(ds_time[3])+(str(ds_time[4]) if len(str(ds_time[4])) == 2 else '0'+str(ds_time[4]))
			# Get lighton and lightoff
			with open('light', 'r') as f:
				lighton = f.readline().strip()
				lightoff = f.readline().strip()
			# Check time to update every 15 minutes
			sleep_time = 900
			# Sleep for 12 hours if light schedule is not set
			if lightoff == lighton:
				# Check light schedule every 15 minutes, if set, break to outer loop
				for i in range(0,48):
					await asyncio.sleep(900)
					if not lightoff == lighton:
						break
					# Update after 12 hours
					if i == 47:
						counter = counter + 1
						# Restart every seven days
						if counter == 6:
							await machine_reset_10_seconds()
						# Try OTA update 4 times in one go
						ota_result = await ota.update(FIRMWARE_VERSION, debug = debug or custom_device)
						if not ota_result:
							await asyncio.sleep(15)
							ota_result = await ota.update(FIRMWARE_VERSION, debug = debug or custom_device)
							if not ota_result:
								await asyncio.sleep(15)
								ota_result = await ota.update(FIRMWARE_VERSION, debug = debug or custom_device)
								if not ota_result:
									await asyncio.sleep(15)
									ota_result = await ota.update(FIRMWARE_VERSION, debug = debug or custom_device)
			# Try to update in the middle of light off period
			elif int(lightoff) < int(lighton):
				rough_middle = str(int((int(lightoff) + int(lighton))/2))[:-2]
				if int(rough_middle + '00') < int(military) < int(rough_middle + '20'):
					counter = counter + 1
					# Restart every seven days
					if counter == 6:
						await machine_reset_10_seconds()
					ota_result = await ota.update(FIRMWARE_VERSION, debug = debug or custom_device)
					if not ota_result:
						await asyncio.sleep(15)
						ota_result = await ota.update(FIRMWARE_VERSION, debug = debug or custom_device)
						if not ota_result:
							await asyncio.sleep(15)
							ota_result = await ota.update(FIRMWARE_VERSION, debug = debug or custom_device)
							if not ota_result:
								await asyncio.sleep(15)
								ota_result = await ota.update(FIRMWARE_VERSION, debug = debug or custom_device)
			# Try to update in the middle of light off period
			elif int(lightoff) > int(lighton):
				rough_middle = str(int((int(lightoff) - int(lighton))/2))[:-2]
				if int(rough_middle + '00') < int(military) < int(rough_middle + '20'):
					counter = counter + 1
					# Restart every seven days
					if counter == 6:
						await machine_reset_10_seconds()
					ota_result = await ota.update(FIRMWARE_VERSION, debug = debug or custom_device)
					if not ota_result:
						await asyncio.sleep(15)
						ota_result = await ota.update(FIRMWARE_VERSION, debug = debug or custom_device)
						if not ota_result:
							await asyncio.sleep(15)
							ota_result = await ota.update(FIRMWARE_VERSION, debug = debug or custom_device)
							if not ota_result:
								await asyncio.sleep(15)
								ota_result = await ota.update(FIRMWARE_VERSION, debug = debug or custom_device)
		await asyncio.sleep(sleep_time)


# Mark partition as valid
async def mark_running_partition_as_valid():
	await asyncio.sleep(65)
	if debug or custom_device: print('All services started successfully!')
	RUNNING_PARTITION.mark_app_valid_cancel_rollback()
	if debug or custom_device: print('Since no errors were found, marked boot partition as valid!')
			

## Global Variables ##
# Default pump states
try:
	with open('pump', 'r') as f:
		lines=len(f.readlines())
	if not lines == 3:
		raise Exception
except:
	with open('pump', 'w') as pump_file:
		pump_file.write('0\n600\n60\n')
# Default light states
try:
	with open('light', 'r') as f:
		lines=len(f.readlines())
	if not lines == 2:
		raise Exception
except:
	with open('light', 'w') as light_file:
		light_file.write('0000\n0000\n')
# Default ambient state alert limit
try:
	with open('ambient_limit', 'r') as f:
		lines=len(f.readlines())
	if not lines == 4:
		raise Exception
except:
	with open('ambient_limit', 'w') as light_file:
		light_file.write('40\n10\n90\n10\n')
# Anonymization salt
try:
	with open('salt') as salt_file:
		salt = salt_file.read()
	if debug or custom_device: print('Found existing salt')
except:
	if debug or custom_device: print('Creating new salt')
	salt = create_random_key()
	with open('salt', 'w') as salt_file:
		salt_file.write(salt)
# Import platform specific Pins
from vars_ESP32_GENERIC_S3 import amb_scl, amb_sda, analog_water_sensor, water_power, hard_reset_button, network_reset_button, pump, light, err_light, dat_light, i2c_r
# Try RTC
try:
	ds3231 = DS3231(i2c_r)
	ds3231.get_time(set_rtc=True)
except: pass
# Running Partition
RUNNING_PARTITION = esp32.Partition(esp32.Partition.RUNNING)
# OTA
ota = OTAUpdater(url='https://ota.amastechnologies.com/', watchdog=wdt, light_activities={'good_light':dat_light_act,'error_light':err_light_act}, platform=platform)
if debug or custom_device: print('Loaded global variables!')

# Feed watchdog
wdt.feed()

# Delete stale transient state files
try:
	os.remove('drain_check')
except: pass
try:
	os.remove('light_override')
except: pass
try:
	os.remove('null_desire')
except: pass

# Write the default cert in the filesystem
from default_api_cert import base64_cert_and_key
with open('def_cert.der', 'wb') as def_cert_file:
	def_cert_file.write(a2b_base64(base64_cert_and_key['base64_DER_cert']))
with open('def_key.der', 'wb') as def_key_file:
	def_key_file.write(a2b_base64(base64_cert_and_key['base64_DER_key']))
del base64_cert_and_key
gc.collect()


### API ###

app = Microdot()
CORS(app, allowed_origins='*', allow_credentials=True, max_age=20, expose_headers=['Allow', 'Content-Type', 'x-api-key', 'Access-Control-Allow-Origin', 'Access-Control-Allow-Methods', 'Access-Control-Allow-Headers', 'Access-Control-Request-Method', 'Access-Control-Expose-Headers', 'Access-Control-Allow-Credentials'])

# Control endpoint
@app.route('/control', methods=['POST', 'OPTIONS'])
async def configureDevice(request):
	global salt

	# Authorization
	global api_key
	sent_headers = request.headers
	if not sent_headers or 'x-api-key' not in sent_headers or sent_headers['x-api-key'] !=  api_key:
		return 'Unauthorized', 401
	
	# Get desired unit state
	unit_state = request.json
	if unit_state: 
		# If being configured by new state
		try:
			query_state = unit_state['state']['admin']
		except: query_state = {}
		try:
			unit_state = unit_state['state']['desired']
		except: unit_state = {}
	else:
		# If being configured for saved/default state
		query_state = {}
	# Check whether the device is being rebooted
	rebooting = await configurator(unit_state=unit_state, query_state=query_state)
	# Add a note if rebooting
	if rebooting:
		body = {'state': {'note': 'DEVICE REBOOTING IN 10 SECONDS!',
		     			  'admin': {'salt': salt, 'debug': debug or custom_device},
						  'reported': await payload_maker()}}
	else:
		body = {'state': {'admin': {'salt': salt, 'debug': debug or custom_device},
						  'reported': await payload_maker()}}
	response = Response(
		body=body,
		status_code=200,
		headers={'Content-Type': 'application/json'},
		)
	if request.method == 'OPTIONS': response.headers['Allow'] = 'OPTIONS, POST'
	await dat_light_act()
	return response

# Setup endpoint
@app.route('/setup', methods=['POST', 'OPTIONS'])
async def setupUnit(request):
	global salt

	# Authorization
	global api_key
	sent_headers = request.headers
	if not sent_headers or 'x-api-key' not in sent_headers or sent_headers['x-api-key'] !=  api_key:
		return 'Unauthorized', 401
	
	rebooting = False
	# Get setup dict
	setup_dic = request.json
	# Setup device
	if setup_dic:
		rebooting = await setuper(setup_dic)
	if rebooting:
		body = {'state': {'note': 'DEVICE REBOOTING IN 10 SECONDS!',
		     			  'admin': {'salt': salt, 'debug': debug or custom_device},
						  'reported': await payload_maker()}}
	else:
		body = {'state': {'admin': {'salt': salt, 'debug': debug or custom_device},
						  'reported': await payload_maker()}}
	response = Response(
		body=body,
		status_code=200,
		headers={'Content-Type': 'application/json'},
		)
	if request.method == 'OPTIONS': response.headers['Allow'] = 'OPTIONS, POST'
	return response

# Salt endpoint
@app.route('/salt', methods=['POST', 'OPTIONS'])
async def regenerateSalt(request):
	global salt

	# Authorization
	global api_key
	sent_headers = request.headers
	if not sent_headers or 'x-api-key' not in sent_headers or sent_headers['x-api-key'] !=  api_key:
		return 'Unauthorized', 401
	
	# Get salt dict
	salt_dic = request.json
	# Generate salt
	if salt_dic['generate']:
		if debug or custom_device: print('Creating new salt')
		salt = create_random_key()
		with open('salt', 'w') as salt_file:
			salt_file.write(salt)
	else: raise RuntimeError
	body={'state': {'note': 'New salt generated!'}}
	response = Response(
		body=body,
		status_code=200,
		headers={'Content-Type': 'application/json'},
		)
	if request.method == 'OPTIONS': response.headers['Allow'] = 'OPTIONS, POST'
	return response

# API Key endpoint
@app.route('/api_key', methods=['POST', 'OPTIONS'])
async def regenerateApiKey(request):
	global api_key

	# Authorization
	sent_headers = request.headers
	if not sent_headers or 'x-api-key' not in sent_headers or sent_headers['x-api-key'] !=  api_key:
		return 'Unauthorized', 401
	
	# Get api_key dict
	api_key_dic = request.json
	# Generate api_key if instructed
	if api_key_dic['generate']:
		cust_api_key = create_random_key(32)
		with open('api_key', 'w') as cust_api_key_file:
			cust_api_key_file.write(cust_api_key)
		body = {'api_key': cust_api_key}
		body = {'state': {'new': body}}
	else:
		body={'state': {'note': 'New API key not generated!'}}
	response = Response(
			body=body,
			status_code=200,
			headers={'Content-Type': 'application/json'},
			)
	try:
		api_key = cust_api_key
	except: pass
	if request.method == 'OPTIONS': response.headers['Allow'] = 'OPTIONS, POST'
	return response

# Update WebSocket endpoint
@app.route('/update')
@with_websocket
async def updateStream(request, ws):
	# Authorization
	global api_key
	sent_headers = loads(await ws.receive())
	if not sent_headers or 'x-api-key' not in sent_headers or sent_headers['x-api-key'] !=  api_key:
		await ws.send('close cmd')
		return 'Unauthorized', 401
	
	# Send the previous log first
	try:
		with open('update_log', 'r') as update_file:
			await ws.send('Previous run -----------')
			try:
				for line in update_file:
					await ws.send(line)
			except: pass
			await ws.send('End previous run -------')
	except: await ws.send('Update has not yet run after boot!')

	update_dic = loads(await ws.receive())
	
	try:
		# If setting auto updates
		if update_dic['auto']:
			try:
				os.remove('no_auto_update')
				await ws.send('Automatic update enabled!')
			except: await ws.send('Automatic update was already enabled!')
		else:
			with open('no_auto_update', 'w') as update_file:
				update_file.write('1')
			await ws.send('Automatic update disabled!')
	except: pass
	try:
		# If just tailing the log
		if update_dic['tail']:
			# Send the entire file first
			try:
				with open('update_log') as update_file:
					for line in update_file:
						await ws.send(line)
					if 'ERROR!!' in line or 'Update end UTC' in line or 'No update necessary' in line or 'Rebooting' in line:
						await ws.send('close cmd')
						return False
			except: 
				await ws.send('Update has not yet run after boot!')
				await ws.send('close cmd')
				return False
			# Last line repeatation testing vars 
			checker = ''
			counter = 0
			# Keep reading the file and only send the last line
			while True:
				try:
					with open('update_log') as update_file:
						for line in update_file:
							pass
						# Count repeatations of the last line
						if checker == line.encode():
							counter += 1
						checker = line.encode()
						await ws.send(line)
						if counter > 7 or 'ERROR!!' in line or 'Update end UTC' in line or 'No update necessary' in line or 'Rebooting' in line:
							await ws.send('close cmd')
							return False
				except:
					break
				await asyncio.sleep(5)
	except: pass
	try:
		# If starting a new update
		if update_dic['start']:
			ota_result = await ota.update(FIRMWARE_VERSION, ws, debug = debug or custom_device)
			if not ota_result:
				await ws.send('Retrying in 15 seconds!')
				await asyncio.sleep(15)
				ota_result = await ota.update(FIRMWARE_VERSION, ws, debug = debug or custom_device)
				if not ota_result:
					await ws.send('Retrying in 15 seconds!')
					await asyncio.sleep(15)
					ota_result = await ota.update(FIRMWARE_VERSION, ws, debug = debug or custom_device)
					if not ota_result:
						await ws.send('Retrying in 15 seconds!')
						await asyncio.sleep(15)
						ota_result = await ota.update(FIRMWARE_VERSION, ws, debug = debug or custom_device)
	except: 
		await ws.send('close cmd')
		return False

# Debug endpoint
@app.route('/debug', methods=['POST', 'OPTIONS'])
async def debugLogging(request):
	global debug

	# Authorization
	global api_key
	sent_headers = request.headers
	if not sent_headers or 'x-api-key' not in sent_headers or sent_headers['x-api-key'] !=  api_key:
		return 'Unauthorized', 401
	
	# Get debug dict
	debug_dic = request.json
	# Change debug state
	if debug_dic['debug']:
		with open('debug', 'w') as debug_file:
			debug_file.write('1')
		body = {'state': {'note': 'Debugging enabled, please connect the device to a computer to start!'}}
		response = Response(
			body=body,
			status_code=200,
			headers={'Content-Type': 'application/json'},
			)
		debug = True
	else:
		try:
			os.remove('debug')
		except: pass
		body={'state': {'note': 'Debugging disabled!'}}
		response = Response(
			body=body,
			status_code=200,
			headers={'Content-Type': 'application/json'},
			)
		debug = False
	if request.method == 'OPTIONS': response.headers['Allow'] = 'OPTIONS, POST'
	return response

# Local data polling endpoint
@app.route('/poll', methods=['GET', 'OPTIONS'])
async def poll_data(request):
	global salt, debug, custom_device
	
	# Authorization
	global api_key
	sent_headers = request.headers
	if not sent_headers or 'x-api-key' not in sent_headers or sent_headers['x-api-key'] !=  api_key:
		return 'Unauthorized', 401
	
	body = {'state': {'admin': {'salt': salt, 'debug': debug or custom_device},'reported': await payload_maker()}}
	response = Response(
		body=body,
		status_code=200,
		headers={'Content-Type': 'application/json'},
		)
	await dat_light_act()
	if request.method == 'OPTIONS': response.headers['Allow'] = 'OPTIONS, GET'
	return response

# Local data WebSocket endpoint
@app.route('/metrics')
@with_websocket
async def eventStreamApi(request, ws):
	global salt, debug, custom_device

	# Authorization
	global api_key
	sent_headers = loads(await ws.receive())
	if not sent_headers or 'x-api-key' not in sent_headers or sent_headers['x-api-key'] !=  api_key:
		await ws.send('close cmd')
		return 'Unauthorized', 401
	
	counter = 0
	# Send the first payload
	data_msg = dumps({'state': {'admin': {'salt': salt, 'debug': debug or custom_device}, 'reported': await payload_maker()}})
	await ws.send(data_msg)
	await dat_light_act()
	if debug or custom_device: print('Sent data through WebSocket')
	del data_msg
	gc.collect()
	old_state = await unit_state_finder()
	while True:
		# Check for change in device state every 2 seconds
		new_state = await unit_state_finder()
		# If there's change in state, send data
		if old_state != new_state:
			data_msg = dumps({'state': {'admin': {'salt': salt, 'debug': debug or custom_device}, 'reported': await payload_maker(new_state)}})
			old_state = new_state
			gc.collect()
			await ws.send(data_msg)
			await dat_light_act()
			if debug or custom_device: print('Sent data through WebSocket')
			gc.collect()
		if counter < 30:
			counter += 1
		else:
			# Send data every minute on local connection
			counter = 0
			gc.collect()
			data_msg = dumps({'state': {'admin': {'salt': salt, 'debug': debug or custom_device}, 'reported': await payload_maker(old_state)}})
			await ws.send(data_msg)
			await dat_light_act()
			if debug or custom_device: print('Sent data through WebSocket')
		await asyncio.sleep(2)

# Local connection test endpoint
@app.route('/local', methods=['GET', 'OPTIONS'])
async def local_connection(request):
	response = Response(
			body='OK',
			status_code=200,
			headers={'Content-Type': 'text/plain'},
			)
	await dat_light_act()
	if request.method == 'OPTIONS': response.headers['Allow'] = 'OPTIONS, GET'
	return response

# Error handlers
@app.errorhandler(404)
async def not_found(request):
	response = Response(
		body='No such endpoint!',
		status_code=404,
		headers={'Content-Type': 'text/plain', 'Access-Control-Allow-Origin': '*'},
		)
	await err_light_act()
	return response

# Error handlers
@app.errorhandler(RuntimeError)
async def runtime_error(request, exception):
	if not (debug or custom_device):
		exception = 'Runtime error! Restarting the device is advisable!'
	response = Response(
		body=exception,
		status_code=500,
		headers={'Content-Type': 'text/plain', 'Access-Control-Allow-Origin': '*'},
		)
	await err_light_act()
	return response

# Asynchronous webserver
async def start_microdot():
	global custom_device
	asyncio.create_task(mark_running_partition_as_valid())
	# Get the SSL certificate to start the server with
	sslctx = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
	try:
		with open('cert.der', 'rb'):
			pass
		with open('key.der', 'rb'):
			pass
		sslctx.load_cert_chain('cert.der', 'key.der')
		if debug or custom_device: print('Using IP specific API certificate')
	except:
		sslctx.load_cert_chain('def_cert.der', 'def_key.der')
		if debug or custom_device: print('Using default API certificate')
	# Start with debug, if it is a custom device, or if debugging is enabled
	await app.start_server(port=443, debug=debug or custom_device, ssl=sslctx)


### MQTT ###

# Callback to handle control JSON
def callback(topic, msg, retained):
	asyncio.create_task(dat_light_act())
	try:
		msg = msg.decode()
		if debug or custom_device: print('MQTT message received: '+msg)
		# Receive desired states
		config_dic = loads(msg)
		# Validate desired states
		if 'state' in config_dic:
			try:
				config_dic = config_dic['state']['desired']
			except:
				config_dic = config_dic['state']
				if 'pump' not in config_dic and 'light' not in config_dic and 'auto_update' not in config_dic:
					config_dic = {}
		elif 'desired' in config_dic:
			config_dic = config_dic['desired']
		if 'pump' not in config_dic and 'light' not in config_dic and 'auto_update' not in config_dic:
			config_dic = {}
		# Write transient file to empty desired states
		if config_dic:
			for i in config_dic:
				if i == 'auto_update':
					with open('null_desire', 'a') as desire:
						desire.write(dumps({i: None}))
						desire.write('\n')
				else:
					for j in config_dic[i]:
						with open('null_desire', 'a') as desire:
							desire.write(dumps({i: {j: None}}))
							desire.write('\n')
			gc.collect()
		asyncio.create_task(configurator(unit_state=config_dic))
	except Exception as e:
		if debug or custom_device: print(e) 
		pass

# Subscribe to update/delta device shadow topic to get control JSON
async def conn_han(client):
	global topic_sub
	await client.subscribe(topic_sub, 1)

# Data publication service to MQTT broker
async def data_pub(client, anon_hash=None):
	global topic_pub, anon_data_topic
	# Counter to check how much time has passed
	pub_timer = 0
	# Counter for sending full device state after every few slim updates
	full_state_counter = 0
	# Send initial data
	data_msg = {'state': {'reported': await payload_maker()}}
	data_msg = dumps(data_msg)
	await client.publish(topic_pub, data_msg,qos=1)
	await dat_light_act()
	if debug or custom_device: print('Sent state to MQTT broker')
	# Save last data sent time
	with open('last_mqtt_update', 'w') as last_update_file:
		last_update_file.write(str(time()))
	# Collect anonymized data
	data_msg = loads(data_msg)
	if 'light' in data_msg['state']['reported'] or 'pump' in data_msg['state']['reported'] or 'firmware_version' in data_msg['state']['reported']:
		if anon_hash:
			anon_msg = await anon_data_maker(data_msg)
			anon_msg.update({'anon_hash': anon_hash})
			anon_msg = dumps(anon_msg)
			await client.publish(anon_data_topic, anon_msg,qos=1)
			await dat_light_act()
			del anon_msg
			if debug or custom_device: print('Collected anonymized data')
			gc.collect()
	del data_msg
	gc.collect()
	# Send data twice on first connection
	data_msg = {'state': {'reported': await payload_maker()}}
	data_msg = dumps(data_msg)
	await client.publish(topic_pub, data_msg,qos=1)
	await dat_light_act()
	if debug or custom_device: print('Sent state to MQTT broker')
	# Update last data sent time
	with open('last_mqtt_update', 'w') as last_update_file:
		last_update_file.write(str(time()))
	del data_msg
	old_state = await unit_state_finder()
	while True:
		# Check for any change in state every 5 seconds
		new_state = await unit_state_finder()
		# If there is change in state
		if old_state != new_state:
			# Find just the state that changed
			state_diff = unit_state_diff(old_state, new_state)
			full_state_counter += 1
			# Send the full state once at least every few hours
			if old_state['pump']['interval'] <= 1200:
				if full_state_counter <= 6:
					# Slim payload
					data_msg = {'state': {'reported': await payload_maker(state_diff)}}
				else:
					# Full payload
					full_state_counter = 0
					data_msg = {'state': {'reported': await payload_maker(new_state)}}
			else:
				if full_state_counter <= 4:
					# Slim payload
					data_msg = {'state': {'reported': await payload_maker(state_diff)}}
				else:
					# Full payload
					full_state_counter = 0
					data_msg = {'state': {'reported': await payload_maker(new_state)}}
			old_state = new_state
			del new_state
			gc.collect()
			# Send data
			data_msg = dumps(data_msg)
			await client.publish(topic_pub, data_msg,qos=1)
			await dat_light_act()
			if debug or custom_device: print('Sent new state to MQTT broker')
			# Update last data sent time
			with open('last_mqtt_update', 'w') as last_update_file:
				last_update_file.write(str(time()))
			# Delete persistent desired states
			try:
				with open('null_desire', 'r') as desire:
					for line in desire:
						delete_desire = dumps({'state': {'desired': loads(line)}})
						await client.publish(topic_pub, delete_desire,qos=1)
						if debug or custom_device: print('Deleted desired values from MQTT broker: ' + line)
						await asyncio.sleep(1)
				try: os.remove('null_desire')
				except: pass
			except: pass
			# Collect anonymized data
			data_msg = loads(data_msg)
			if 'light' in data_msg['state']['reported'] or 'pump' in data_msg['state']['reported'] or 'firmware_version' in data_msg['state']['reported']:
				if anon_hash:
					anon_msg = await anon_data_maker(data_msg)
					anon_msg.update({'anon_hash': anon_hash})
					anon_msg = dumps(anon_msg)
					await client.publish(anon_data_topic, anon_msg,qos=1)
					await dat_light_act()
					del anon_msg
					gc.collect()
			del data_msg
			gc.collect()
		# Check last data sent time
		with open('last_mqtt_update') as last_update_file:
			last_update = int(last_update_file.read())
		# Send data every 15 minutes, even if there's no change in state
		if time() - last_update > 900:
			del new_state
			gc.collect()
			# Send the full state once at least every two hours
			if old_state['pump']['interval'] <= 1200:
				if full_state_counter <= 6:
					# Slim payload
					data_msg = {'state': {'reported': await payload_maker({})}}
				else:
					# Full payload
					full_state_counter = 0
					data_msg = {'state': {'reported': await payload_maker(old_state)}}
			else:
				if full_state_counter <= 3:
					# Slim payload
					data_msg = {'state': {'reported': await payload_maker({})}}
				else:
					# Full payload
					full_state_counter = 0
					data_msg = {'state': {'reported': await payload_maker(old_state)}}
			# Send data
			data_msg = dumps(data_msg)
			await client.publish(topic_pub, data_msg,qos=1)
			await dat_light_act()
			if debug or custom_device: print('Sent state to MQTT broker')
			# Update last data sent time
			with open('last_mqtt_update', 'w') as last_update_file:
				last_update_file.write(str(time()))
			# Collect anonymized data
			if anon_hash:
				anon_msg = await anon_data_maker(loads(data_msg))
				anon_msg.update({'anon_hash': anon_hash})
				anon_msg = dumps(anon_msg)
				await client.publish(anon_data_topic, anon_msg,qos=1)
				await dat_light_act()
				del anon_msg
				gc.collect()
				if debug or custom_device: print('Collected anonymized data')
			del data_msg
			gc.collect()
		# Reconnect every day
		if pub_timer < 2160:
			pub_timer += 1
		else:
			raise Exception
		await asyncio.sleep(5)

# Guarded data publication
async def data_pub_guarded(anon_hash=None):
	global client
	while True:
		try:
			await asyncio.wait_for(client.connect(), 50)
			await data_pub(client, anon_hash)
		except Exception as e:
			await err_light_act()
			if debug or custom_device: print(e)
			try:
				client.close()
			except: pass
			if debug or custom_device: print('MQTT connection closed, reconnecting...')
			await asyncio.sleep(10)
		gc.collect()

# Feed watchdog
wdt.feed()

# WiFi check initiator
wifi_file_available = False
wifi_error_on_boot = False
# Check if WiFi file is available
try:
	with open('wifi', 'r') as wifi_file:
		ssid = wifi_file.readline().strip()
		password = wifi_file.readline().strip()
	wifi_file_available = True
	if debug or custom_device: print('WiFi file available')
	wifi_connect(ssid, password)
	# If Wifi connects
	if debug or custom_device: print('WiFi connected')
	http_client = HttpClient()
	# Check if stale cert need to be deleted del_cert
	try:
		with open('stale_cert') as stale_file:
			stale_cert = loads(stale_file.read())
			os.remove('stale_cert')
		resp= http_client.post('https://devicecert.amastech.cloud/', json={'del_cert': stale_cert})
	except: pass
	try:
		# Check if registering under a user
		with open('cust_id_token', 'r') as id_token_file:
			id_token = id_token_file.read()
		if debug or custom_device: print('Customer id_token found, requesting certificates')
		# Feed watchdog
		wdt.feed()
		# Check if there is an existing certificate that needs to be rotated
		try:
			with open('customer.json', 'r') as cust_file:
				cust_info = loads(cust_file.read())
			stale_cert = {'cust_id': cust_info['cust_id'], 'cert_id': cust_info['cert_id']}
		except:
			try:
				with open('stale_cert') as stale_file:
					stale_cert = loads(stale_file.read())
				os.remove('stale_cert')
			except:
				stale_cert = False
		if stale_cert:
			body_payload = {'token': id_token, 'dev_id': device_id, 'serial': serial, 'stale_cert': stale_cert}
		else:
			body_payload = {'token': id_token, 'dev_id': device_id, 'serial': serial}
		resp= http_client.post('https://devicecert.amastech.cloud/', json=body_payload)
		if resp:
			cust_info = resp.json()
			if debug or custom_device: print('Got user certificates')

			with open('customer.json', 'w') as cust_file:
				cust_file.write(dumps(cust_info))
			os.remove('cust_id_token')
	except: pass
	# Check if new SSL certs need to be issued
	station = network.WLAN(network.STA_IF)
	new_ip = station.ifconfig()[0]
	try:
		with open('old_ip') as ip_file:
			old_ip = ip_file.read()
		if new_ip != old_ip:
			raise Exception
		with open('cert.der'):
			pass
		with open('key.der'):
			pass
	except:
		try:
			try:
				if dhcp_hostname:
					body_payload = {'ip': new_ip, 'serial': serial, 'dhcp_hostname': dhcp_hostname}
				else:
					raise Exception
			except:
				body_payload = {'ip': new_ip, 'serial': serial}
			resp= http_client.post('https://httpscert.amastech.cloud/', json=body_payload)
			if resp:
				if debug or custom_device: print('Got API certificates')
				api_https_info = resp.json()
				if debug or custom_device: print(api_https_info)
				with open('cert.der', 'wb') as cust_https_cert_file:
					cust_https_cert_file.write(a2b_base64(api_https_info['base64_DER_cert']))
				with open('key.der', 'wb') as cust_https_key_file:
					cust_https_key_file.write(a2b_base64(api_https_info['base64_DER_key']))
				with open('old_ip', 'w') as ip_file:
					ip_file.write(new_ip)
				del api_https_info, resp
				gc.collect()
				if debug or custom_device: print('Saved API certificates')
		except: 
			try:
				os.remove('cert.der')
			except: pass
			try:
				os.remove('key.der')
			except: pass
	# Check for user info
	try:
		with open('customer.json', 'r') as cust_file:
			cust_info = loads(cust_file.read())
		if debug or custom_device: print('Using saved device certificates')
		cust_id = cust_info['cust_id']
		if debug or custom_device: print('Registered under: ' + cust_id)
		cert = a2b_base64(cust_info['base64_DER_cert'])
		key = a2b_base64(cust_info['base64_DER_key'])
		del cust_info
		wifi_ch = 0
		cust_ch = 0
		# MQTT
		topic_sub = '$aws/things/' + cust_id + '/shadow/name/' + device_id + '/update/delta'
		topic_pub = '$aws/things/' + cust_id + '/shadow/name/' + device_id + '/update'
		anon_data_topic = '$aws/rules/anonymized_data_collection/device/' + device_id + '/data'
		config['server'] = 'iotcore.amastech.cloud'
		config['port'] = 8883
		config['client_id'] = device_id.encode()
		config['ssid'] = ssid
		config['wifi_pw'] = password
		config['ssl']=True
		config['ssl_params']={'cert':cert,'key':key}
		config['subs_cb'] = callback
		config['connect_coro'] = conn_han
		MQTTClient.DEBUG = custom_device or debug
		client = MQTTClient(config)
		# Get initial state
		asyncio.create_task(configurator())
		gc.collect()
		# Feed watchdog
		wdt.feed()
		# Start services
		if debug or custom_device: print('Starting services')
		asyncio.create_task(time_corrector())
		asyncio.create_task(wifi_check())
		asyncio.create_task(auto_update())
		asyncio.create_task(network_reset_button_checker())
		asyncio.create_task(hard_reset_button_checker())
		asyncio.create_task(light_check())
		asyncio.create_task(pump_check())
		asyncio.create_task(alert_check())
		# Feed watchdog
		wdt.feed()
		try:
			with open('no_telemetry') as telm_file:
				if debug or custom_device: print('No telemtry selected')
			asyncio.create_task(data_pub_guarded())
		except:
			if debug or custom_device: print('Anonymous telemetry selected')
			with open('salt') as salt_file:
				salt = salt_file.read()
			anon_hash = uhashlib.sha256((cust_id+device_id+salt).encode())
			anon_hash = hexlify(anon_hash.digest())
			if debug or custom_device: print('Got hash, continuing with services')
			asyncio.create_task(data_pub_guarded(anon_hash))
		# Feed watchdog
		wdt.feed()
		asyncio.run(start_microdot())
	# If no customer ID, start microdot with cust setup access
	except:
		wifi_ch = 0
		cust_ch = 1
		gc.collect()
		# Get initial state
		asyncio.create_task(configurator())
		if debug or custom_device: print('Starting services')
		# Feed watchdog
		wdt.feed()
		asyncio.create_task(time_corrector())
		asyncio.create_task(wifi_check())
		asyncio.create_task(auto_update())
		asyncio.create_task(network_reset_button_checker())
		asyncio.create_task(hard_reset_button_checker())
		asyncio.create_task(no_cust_light())
		asyncio.create_task(light_check())
		asyncio.create_task(pump_check())
		asyncio.create_task(alert_check())
		# Feed watchdog
		wdt.feed()
		asyncio.run(start_microdot())
# If no WiFi, start WiFi setup access
except:
	wifi_error_on_boot = True
	try:
		with open('customer.json', 'r') as cust_file:
			cust_info = loads(cust_file.read())
		cust_id = cust_info['cust_id']
		if debug or custom_device: print('Registered under: ' + cust_id)
		del cust_info
		wifi_ch = 1
		cust_ch = 0
		soft_ap()
		# Get initial state
		asyncio.create_task(configurator())
		if debug or custom_device: print('Starting services')
		# Feed watchdog
		wdt.feed()
		if wifi_file_available:
			if debug or custom_device: print("Couldn't connect to the WiFi!")
			asyncio.create_task(wifi_check())
		asyncio.create_task(network_reset_button_checker())
		asyncio.create_task(hard_reset_button_checker())
		asyncio.create_task(no_wifi_light())
		asyncio.create_task(light_check())
		asyncio.create_task(pump_check())
		asyncio.create_task(alert_check())
		# Feed watchdog
		wdt.feed()
		asyncio.run(start_microdot())
	except:
		wifi_ch = 1
		cust_ch = 1
		soft_ap()
		# Get initial state
		asyncio.create_task(configurator())
		if debug or custom_device: print('Starting services')
		# Feed watchdog
		wdt.feed()
		if wifi_file_available:
			if debug or custom_device: print("Couldn't connect to the WiFi!")
			asyncio.create_task(wifi_check())
		asyncio.create_task(network_reset_button_checker())
		asyncio.create_task(hard_reset_button_checker())
		asyncio.create_task(no_cust_no_wifi_light())
		asyncio.create_task(light_check())
		asyncio.create_task(pump_check())
		asyncio.create_task(alert_check())
		# Feed watchdog
		wdt.feed()
		asyncio.run(start_microdot())
