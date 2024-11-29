#### COPYRIGHT AMAS TECHNOLOGIES ####
## LICENSED UNDER AGPL-3.0 ##
#### COPYRIGHT AMAS TECHNOLOGIES ####

import os, gc, ubinascii, time, machine
from esp32 import Partition
from httpclient import HttpClient
import uasyncio as asyncio


OTA_PARTITION = Partition(Partition.RUNNING).get_next_update()
PARTITION_BLOCKS = OTA_PARTITION.ioctl(4,...)

def multiple_estimator(http_client, url, chunk_dir, multiple_file):
	try:
		multiple = http_client.get(url + chunk_dir + '/' + multiple_file)
		if multiple.status_code == 200:
			multiple = int(multiple.content.decode())
			return multiple
		else: raise RuntimeError
	except:
		time.sleep(1)
		gc.collect()
		try:
			multiple = http_client.get(url + chunk_dir + '/' + multiple_file)
			if multiple.status_code == 200:
				multiple = int(multiple.content.decode())
				return multiple
			else: raise RuntimeError
		except: 
			time.sleep(2)
			gc.collect()
			try:
				multiple = http_client.get(url + chunk_dir + '/' + multiple_file)
				if multiple.status_code == 200:
					multiple = int(multiple.content.decode())
					return multiple
				else: raise RuntimeError
			except: return False

def update_retry_check():
	with open('update_log', 'r') as log_file:
		last_try_version = False
		retry_from_chunk = 1
		try:
			for line in log_file:
				if 'Updating to' in line:
					last_try_version = line.strip().split(': ')[1]
				elif 'Downloaded chunks' in line:
					retry_from_chunk = int(float(line.strip().split(': ')[1]))
			return last_try_version, retry_from_chunk
		except:
			return False, False

def stop_check():
	counter = 0
	checker = 'Stopped duplicate update instance!'
	with open('update_log', 'r') as log_file:
		for line in log_file:
			if checker in line:
				counter += 1
				if counter == 4:
					return False
			else:
				counter = 0
	return True

async def upstream_version_check(http_client,url,chunk_dir,version):
	# Check upstream version
	try:
		updated_version = http_client.get(url + chunk_dir + '/' + version)
		if updated_version.status_code == 200:
			updated_version = updated_version.content.decode().strip()
		else: raise RuntimeError
	except:
		await asyncio.sleep(1)
		gc.collect()
		try:
			updated_version = http_client.get(url + chunk_dir + '/' + version)
			if updated_version.status_code == 200:
				updated_version = updated_version.content.decode().strip()
			else: raise RuntimeError
		except:
			await asyncio.sleep(2)
			gc.collect()
			try:
				updated_version = http_client.get(url + chunk_dir + '/' + version)
				if updated_version.status_code == 200:
					updated_version = updated_version.content.decode().strip()
				else: raise RuntimeError
			except Exception as e:
				return False
	return updated_version
	

async def auto_restart():
		await asyncio.sleep(10)
		machine.reset()

async def logger(data, ws, debug = False):
	with open('update_log', 'a') as log_file:
		log_file.write(data+'\n')
	if debug: print(data)
	if ws != None:
		await ws.send(data)


class OTAUpdater:
	"""
	A class to update your MicroController with the latest version from base URL,
	optimized for low power usage.
	"""

	def __init__(self, url, watchdog, light_activities, platform=None, chunk_dir='base64chunks', common_name='chunk-', chunk_multiple=False, last_chunk_file='last_chunk', version_file='version', headers={}):
		if headers:
			self.http_client = HttpClient(headers=headers)
		else:
			self.http_client = HttpClient(headers={'Accept': '*/*', 'Connection': 'keep-alive'})
		self.url = url
		if platform:
			self.chunk_dir = platform
		else:
			self.chunk_dir = chunk_dir
		self.common_name = common_name
		self.watchdog = watchdog
		self.multiple = chunk_multiple
		self.multiple_file = 'multiple'
		self.last_chunk = last_chunk_file
		self.version = version_file
		self.good_light = light_activities['good_light']
		self.error_light = light_activities['error_light']


	async def update(self, FIRMWARE_VERSION, ws=None, debug=False):
		# Time the update process
		if time.gmtime(0)[0] == 2000:
			start_time = str(time.time()+946684800)
		else:
			start_time = str(time.time())
		last_try_version = False
		retry_from_chunk = 1
		# Check for already running update processes
		try:
			with open('update_log', 'r') as log_file:
				for line in log_file:
					pass
				if 'ERROR!!' in line:
					last_try_version, retry_from_chunk = update_retry_check()
					os.remove('update_log')
				elif 'No update necessary' in line or 'Rebooting' in line:
					os.remove('update_log')
				elif 'rebooting' in line:
					last_try_version, retry_from_chunk = update_retry_check()
					os.remove('update_log')
					await logger('Trying to resume update after restart...',ws, debug)
				else:
					await self.error_light()
					await logger('Stopped duplicate update instance! Update will resume after 4 tries and a restart!',ws, debug)
					# Remove log and restart if updates are getting stopped repeatedly
					if stop_check():
						return False
					else:
						await logger('Encountered errors during previous few downloads, rebooting!',ws, debug)
						asyncio.create_task(auto_restart())
						return False
		except: pass
		# Log time
		await logger('Update start UTC: ' + start_time,ws, debug)
		updated_version = await upstream_version_check(self.http_client,self.url,self.chunk_dir,self.version)
		if not updated_version:
			return False
		await asyncio.sleep(0.1)
		# Compare upstream and local versions
		local_version = FIRMWARE_VERSION.split('.')
		remote_version = updated_version.split('.')
		if int(local_version[0].split('v')[1]) < int(remote_version[0].split('v')[1]):
			await logger('Update available',ws, debug)
		elif int(local_version[0].split('v')[1]) > int(remote_version[0].split('v')[1]):
			await logger('No update necessary, installed firmware: '+FIRMWARE_VERSION+', upstream version: '+updated_version,ws, debug)
			return True
		elif int(local_version[1]) < int(remote_version[1]):
			await logger('Update available',ws, debug)
		elif int(local_version[1]) > int(remote_version[1]):
			await logger('No update necessary, installed firmware: '+FIRMWARE_VERSION+', upstream version: '+updated_version,ws, debug)
			return True
		elif int(local_version[2]) >= int(remote_version[2]):
			await logger('No update necessary, installed firmware: '+FIRMWARE_VERSION+', upstream version: '+updated_version,ws, debug)
			return True
		else:
			await logger('Update available',ws, debug)
		await self.good_light()
		await asyncio.sleep(0.1)
		# Get total chunk numbers
		try:
			last_chunk = self.http_client.get(self.url + self.chunk_dir + '/' + self.last_chunk)
			if last_chunk.status_code == 200:
				last_chunk = int(last_chunk.content.decode())
				await self.good_light()
			else: raise RuntimeError
		except:
			await asyncio.sleep(1)
			gc.collect()
			try:
				last_chunk = self.http_client.get(self.url + self.chunk_dir + '/' + self.last_chunk)
				if last_chunk.status_code == 200:
					last_chunk = int(last_chunk.content.decode())
					await self.good_light()
				else: raise RuntimeError
			except:
				await asyncio.sleep(2)
				gc.collect()
				try:
					last_chunk = self.http_client.get(self.url + self.chunk_dir + '/' + self.last_chunk)
					if last_chunk.status_code == 200:
						last_chunk = int(last_chunk.content.decode())
						await self.good_light()
					else: raise RuntimeError
				except Exception as e:
					await self.error_light()
					await logger(str(e),ws, debug)
					await logger('ERROR!!',ws, debug)
					return False
		# Estimate block multiples if necessary
		if not self.multiple:
			self.multiple = multiple_estimator(self.http_client, self.url, self.chunk_dir, self.multiple_file)
		if not self.multiple:
			await self.error_light()
			await logger('ERROR!! Upstream version compromised!',ws, debug)
			return False
		# Check if partition has enough space
		if last_chunk*self.multiple <= PARTITION_BLOCKS:
			await self.good_light()
			await logger('Update possible',ws, debug)
		else:
			await self.error_light()
			await logger('ERROR!! Update larger than partition space!',ws, debug)
			return False
		# Log total number of chunks
		await logger('Total of '+ str(last_chunk) + ' individual chunks, each ' + str(self.multiple*4096) + ' bytes!',ws, debug)
		await asyncio.sleep(0.1)
		# Feed watchdog
		self.watchdog.feed()
		# Check for update retry
		if last_try_version:
			if last_try_version == updated_version:
				chunk_start = retry_from_chunk
				await logger('Resuming update from chunk: '+str(chunk_start),ws, debug)
			else:
				chunk_start = 1
				await logger('Cannot resume update!',ws, debug)
		else: 
			chunk_start = 1
		# Log upstream version
		await logger('Updating to: '+updated_version,ws, debug)
		check_fail = 0
		for i in range(chunk_start, last_chunk+1):
			# Check upstream version every 5 blocks
			if not i%5:
				gc.collect()
				tmp_version = await upstream_version_check(self.http_client,self.url,self.chunk_dir,self.version)
				if tmp_version != False:
					check_fail = 0
					if tmp_version != updated_version:
						await logger('ERROR!! Upstream version changed!',ws, debug)
						return False
				else:
					check_fail += 1
					if check_fail > 9:
						await logger('ERROR!! Upstream version could not be verified as the same, please retry!',ws, debug)
						return False
			# Start downloading individual chunks
			try:
				chunk_enc = self.http_client.get(self.url + self.chunk_dir + '/' + self.common_name + str(i))
				await self.good_light()
			except:
				await self.error_light()
				await asyncio.sleep(1)
				try:
					chunk_enc = self.http_client.get(self.url + self.chunk_dir + '/' + self.common_name + str(i))
					await self.good_light()
				except:
					await self.error_light()
					await asyncio.sleep(1)
					gc.collect()
					try:
						chunk_enc = self.http_client.get(self.url + self.chunk_dir + '/' + self.common_name + str(i))
						await self.good_light()
					except:
						await self.error_light()
						await asyncio.sleep(1)
						gc.collect()
						try:
							chunk_enc = self.http_client.get(self.url + self.chunk_dir + '/' + self.common_name + str(i))
							await self.good_light()
						except:
							await self.error_light()
							await asyncio.sleep(1)
							gc.collect()
							try:
								chunk_enc = self.http_client.get(self.url + self.chunk_dir + '/' + self.common_name + str(i))
								await self.good_light()
							except Exception as e:
								await self.error_light()
								await asyncio.sleep(2)
								gc.collect()
								try:
									chunk_enc = self.http_client.get(self.url + self.chunk_dir + '/' + self.common_name + str(i))
									await self.good_light()
								except Exception as e:
									await self.error_light()
									await asyncio.sleep(2)
									gc.collect()
									try:
										chunk_enc = self.http_client.get(self.url + self.chunk_dir + '/' + self.common_name + str(i))
										await self.good_light()
									except Exception as e:
										await self.error_light()
										await logger(str(e),ws, debug)
										await logger('ERROR!!',ws, debug)
										return False
			await asyncio.sleep(0.1)
			# Feed watchdog
			self.watchdog.feed()
			if chunk_enc.status_code != 200:
				await self.error_light()
				await logger('ERROR!! Got status code: ' + str(chunk_enc.status_code),ws, debug)
				return False
			gc.collect()
			await logger('Downloaded chunks: '+str(i),ws, debug)
			if i == last_chunk:
				tmp_version = await upstream_version_check(self.http_client,self.url,self.chunk_dir,self.version)
				if tmp_version:
					if tmp_version != updated_version:
						await logger('ERROR!! Upstream version changed!',ws, debug)
						return False
				else:
					tmp_version = await upstream_version_check(self.http_client,self.url,self.chunk_dir,self.version)
					if tmp_version:
						if tmp_version != updated_version:
							await logger('ERROR!! Upstream version changed!',ws, debug)
							return False
					else:
						await logger('ERROR!! Upstream version could not be verified as the same, please retry!',ws, debug)
						return False
				await logger('Download complete! Last chunk remaining to be written to the partition!',ws, debug)
				# Erase last part of the partition
				await logger('Erasing reminaing partition blocks before writing the last chunk',ws, debug)
				for j in range((i-1)*self.multiple,PARTITION_BLOCKS):
					OTA_PARTITION.ioctl(6,j)
					await asyncio.sleep(0.1)
				await logger('Done!',ws, debug)
			# Decode base64 chunks to binary file chunks
			try:
				chunk = ubinascii.a2b_base64(chunk_enc.content)
			except:
				await asyncio.sleep(1)
				try:
					chunk = ubinascii.a2b_base64(chunk_enc.content)
				except Exception as e:
					await logger(str(e),ws, debug)
					await logger('ERROR!!',ws, debug)
					return False
			await asyncio.sleep(0.1)
			# Feed watchdog
			self.watchdog.feed()
			try:
				# Write to blocks of partition
				OTA_PARTITION.writeblocks((i-1)*self.multiple, chunk)
				await self.good_light()
			except:
				# Check if last base64 chunk
				if i == last_chunk:
					try:
						# Write to last partition blocks without being a block multiple
						OTA_PARTITION.writeblocks((i-1)*self.multiple, chunk, 0)
						await self.good_light()
					except:
						await asyncio.sleep(1)
						gc.collect()
						try:
							for j in range(0,self.multiple):
								OTA_PARTITION.ioctl(6,(i-1)*self.multiple+j)
							OTA_PARTITION.writeblocks((i-1)*self.multiple, chunk, 0)
							await self.good_light()
						except Exception as e:
							await self.error_light()
							await logger(str(e),ws, debug)
							await logger('ERROR!!',ws, debug)
							return False
				else:
					await asyncio.sleep(1)
					gc.collect()
					try:
						OTA_PARTITION.writeblocks((i-1)*self.multiple, chunk)
						await self.good_light()
					except:
						await self.error_light()
						await logger('ERROR!! Download compromised!',ws, debug)
						return False
			await asyncio.sleep(0.1)
			# Log partition writes
			if self.multiple > 1:
				msg = 'Wrote to partition blocks: ' + str(i*self.multiple)+' (BLOCKNUMs = '+str((i-1)*self.multiple)+' to '+str(i*self.multiple - 1)+')'
				await logger(msg,ws, debug)
			else:
				msg = 'Wrote to partition blocks: ' + str(i)+' (BLOCKNUM = '+str(i-1)+')'
				await logger(msg,ws, debug)
			chunk_enc.close()
			del chunk
			await asyncio.sleep(0.1)
			# Feed watchdog
			self.watchdog.feed()
		try:
			# Set partition to boot
			OTA_PARTITION.set_boot()
			await self.good_light()
			await logger('Set the partition to boot!',ws, debug)
			if time.gmtime(0)[0] == 2000:
				end_time = str(time.time()+946684800)
			else:
				end_time = str(time.time())
			# Log time
			await logger('Update end UTC: ' + end_time,ws, debug)
			asyncio.create_task(auto_restart())
			await logger('Rebooting...',ws, debug)
			return True
		except Exception as e:
			await self.error_light()
			await logger(str(e),ws, debug)
			await logger('ERROR!!',ws, debug)
			return False
		