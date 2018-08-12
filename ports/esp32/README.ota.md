# Over the air updates

This is an example of how to do ota-updates using micropython and a very simple bottle server application. It assumes you have some way of notifying your esp of updates and transmit the update hashe securely (for example using ssl with certificate pinning).

## Folders

We are using the following folder structure:
```
/
├─ boot.py
├─┬ factory
│ ├── VERSION
│ └── main.py  
├─ ota_0
└─ ota_1
```

The folders "factory", "ota_o" and "ota_1" musst have corresponding partitions. "VERSION" contains our currently running software version identifier, for example a git commit hash.

## Basic concept
Our boot.py checks for the currently running partition, changes the working directory to the corresponding folder and exits. Micropython then searches for a main.py in the current working directory and executes it.

When we do an update we overwrite a (currently not running) partition with our new micropython version and download a tar archiv with our new python code. We extract the python code in the corresponding folder and set a new boot partition. Then we reboot.
Optionaly we want to be able to only update the micropython interpreter or the python code. To accomplish that we copy the currently running version if we do not get one to update.

## Example boot.py
```python
import uos
import esp
import sys

def bytecompare(a,b):
	if (len(a) != len(b)):
		return False

	for i in range(0, len(a)):
		if (a[i] != b[i]):
			return False
	return True

def getrunningpartname():
	part = {}
	part["factory"] = esp.partition_find_first(0, 0, None)
	part["ota_0"] = esp.partition_find_first(0, 16, None)
	part["ota_1"] = esp.partition_find_first(0, 17, None)
	curbootpart = esp.ota_get_running_partition();

	if bytecompare(part["factory"][6], curbootpart):
		return "factory"
	if bytecompare(part["ota_0"][6], curbootpart):
		return "ota_0"
	if bytecompare(part["ota_1"][6], curbootpart):
		return "ota_1"
	return None

runningpartname = getrunningpartname()
print("Booting from partition {}".format(runningpartname))
pyversion = 'ERR'
try:
	with open("/{}/VERSION".format(runningpartname), "r") as f:
		pyversion = f.read()
except:
	pass
print("Version hash: {}".format(pyversion.strip()))
sys.path.append("/{}".format(runningpartname))
uos.chdir("/{}".format(runningpartname))
```

## Example ota updater class
This class requires the module "urequests", you can find it here: https://github.com/micropython/micropython-lib/tree/master/urequests
It expects a "log" class as constructor parameter with two methodes: "log" and "log" to log strings or exceptions. A simple implementation could look like this: 

```python
import uio
import sys
import time

class Logger(object):
	def __init__(self):
		self.log('log started', 'log')

	def log(self, msg, module=None):
		generatedmsg = msg
		if module:
			generatedmsg = '{}: {}'.format(module, generatedmsg)
		generatedmsg = '{} {}'.format(time.ticks_ms(), generatedmsg)
		print(generatedmsg)

	def loge(self, e):
		tmp = uio.StringIO('')
		sys.print_exception(e, tmp)
		self.log(tmp.getvalue())
```

```python
import upip_utarfile as utar
import os
import sys
import machine
import esp
import gc
import ubinascii
import hashlib
import json
import urequests as requests

def bytecompare(a,b):
	if (len(a) != len(b)):
		return False

	for i in range(0, len(a)):
		if (a[i] != b[i]):
			return False
	return True

def copyfileobj(src, dest, length=512):
	if hasattr(src, "readinto"):
		buf = bytearray(length)
		while True:
			sz = src.readinto(buf)
			if not sz:
				break
			if sz == length:
				dest.write(buf)
			else:
				b = memoryview(buf)[:sz]
				dest.write(b)
	else:
		while True:
			buf = src.read(length)
			if not buf:
				break
			dest.write(buf)

def copyFile(src, dest, length=512):
	with open(src, "rb") as fsrc:
		with open(dest, "wb") as fdest:
			while True:
				buf = fsrc.read(length)
				if not buf:
					return
				fdest.write(buf)

def deepCopyFolder(src, dest):
	for f in os.ilistdir(src):
		psrc = "{}/{}".format(src, f[0])
		pdest = "{}/{}".format(dest, f[0])
		if f[1] == 0x4000:
			try:
				os.mkdir(pdest)
			except:
				pass
			deepCopyFolder(psrc, pdest)
		else:
			copyFile(psrc, pdest)

def deepDeleteFolder(path):
	for f in os.ilistdir(path):
		ppath = "{}/{}".format(path, f[0])
		if f[1] == 0x4000:
			deepDeleteFolder(ppath)
			try:
				os.rmdir(ppath)
			except:
				pass
		else:
			try:
				os.remove(ppath)
			except:
				pass
	try:
		os.rmdir(path)
	except:
		pass

class Updater(object):
	"""Handles OTA updates"""

	SEC_SIZE = 4096
	CHUNK_SIZE = 512
	TAR_PATH = "/update.tar"

	def __init__(self, log):
		self.log = log
		self.partitions = {}
		self.partitions["factory"] = esp.partition_find_first(0, 0, None)
		self.partitions["ota_0"] = esp.partition_find_first(0, 16, None)
		self.partitions["ota_1"] = esp.partition_find_first(0, 17, None)
		self.bootpartition = esp.ota_get_running_partition()
		self.nextbootpartition = None
		self.curbootpartition = None
		if bytecompare(self.partitions["factory"][6], self.bootpartition):
			self.nextbootpartition = "ota_0"
			self.curbootpartition = "factory"
		if bytecompare(self.partitions["ota_0"][6], self.bootpartition):
			self.nextbootpartition = "ota_1"
			self.curbootpartition = "ota_0"
		if bytecompare(self.partitions["ota_1"][6], self.bootpartition):
			self.nextbootpartition = "ota_0"
			self.curbootpartition = "ota_1"
		if not self.nextbootpartition:
			 raise ValueError("Unsupported boot partition {}".format(self.bootpartition))
		self.nextbootpartbasesec = self.partitions[self.nextbootpartition][2]//4096

	def update(self, metadata):
		gc.collect()
		if not self.checkSignature(metadata):
				raise RuntimeError("Update meta data corrupt")

		self.log.log('starting partition update', 'ota')
		if "partition" in metadata:
			self.downloadPartitionImage(metadata["partition"]["url"], metadata["partition"]["size"])
			if not self.checkPartition(metadata["partition"]["hash"], metadata["partition"]["size"]):
				self.deletePartition()
				raise RuntimeError("Partition update corrupt")
		else:
			self.copyPartition()

		self.log.log('starting vfs update', 'ota')
		if "vfs" in metadata:
			self.downloadFile(self.TAR_PATH, metadata["vfs"]["url"], metadata["vfs"]["size"])
			if not self.checkFileHash(self.TAR_PATH, metadata["vfs"]["hash"]):
				os.remove(self.TAR_PATH)
				raise RuntimeError("Vfs update corrupt")
			else:
				self.deleteOldVFS()
				self.unpackTar()
		else:
			self.copyVFS()
		self.finishUpdate()

	def checkSignature(self, metadata):
		# you should implement a signature check here, we omitted it for a short example
		return True

	def downloadChunk(self, url, id):
		count = 0
		while count <= 3:
			try:
				r = requests.get("{}/{}".format(url, id))
				content = r.content
				r.close()
				break
			except Exception as e:
				self.log.loge(e)
				self.log.log('chunk download failed, retry {}/3'.format(count), 'ota')
				pass
			count += 1
		return content

	def downloadPartitionImage(self, baseurl, updatesize):
		if self.partitions[self.nextbootpartition][3] < updatesize:
			raise RuntimeError("Partition too small for update, {}k needed, but partition size is {}k".format(updatesize//1024, self.partitions[self.nextbootpartition][3]//1024))
		writtensize = 0
		for i in range(0, updatesize/self.CHUNK_SIZE):
			buf = self.downloadChunk(baseurl, i)
			writtensize += self.writePartitionChunk(buf, i)
			buf = None
		if writtensize != updatesize:
			raise RuntimeError("Update corrupt, downloaded partition size does not match announced size")

	def deletePartition(self):
		buf = bytearray(self.CHUNK_SIZE)
		for i in range(0, self.partitions[self.nextbootpartition][3]/self.CHUNK_SIZE):
			self.writePartitionChunk(buf, i)

	def copyPartition(self):
		buf = bytearray(self.CHUNK_SIZE)
		for i in range(0, self.partitions[self.curbootpartition][3]/self.CHUNK_SIZE):
			esp.flash_read(self.partitions[self.curbootpartition][2]+self.CHUNK_SIZE*i, buf)
			self.writePartitionChunk(buf, i)

	def writePartitionChunk(self, buffer, id):
		chunkspersec = (self.SEC_SIZE//self.CHUNK_SIZE)
		if id%chunkspersec == 0:
			esp.flash_erase(self.nextbootpartbasesec + id//chunkspersec)
		esp.flash_write(self.partitions[self.nextbootpartition][2]+self.CHUNK_SIZE*id, buffer)
		return len(buffer)

	def checkPartition(self, hash, updatesize):
		h = hashlib.sha256()
		buf = bytearray(int((updatesize/self.CHUNK_SIZE - updatesize//self.CHUNK_SIZE)*self.CHUNK_SIZE))
		position = self.partitions[self.nextbootpartition][2]
                for i in range(0, updatesize/self.CHUNK_SIZE):
			esp.flash_read(position, buf)
			h.update(buf)
			position += len(buf)
			buf = bytearray(self.CHUNK_SIZE)
		parthash = (ubinascii.hexlify(h.digest()).decode())
		self.log.log('partition hash is "{}", should be "{}"'.format(parthash, hash), 'ota')
		return parthash == hash

	def downloadFile(self, path, baseurl, updatesize):
		with open(path, "wb") as f:
			writtensize = 0
			for i in range(0,updatesize/self.CHUNK_SIZE):
				buf = self.downloadChunk(baseurl, i)
				writtensize += f.write(buf)
				buf = None
				f.flush()
			if writtensize != updatesize:
				raise RuntimeError("Update corrupt, downloaded {} size does not match announced size".format(path))

	def checkFileHash(self, path, hash):
		h = hashlib.sha256()
		with open(path, 'rb') as f:
			buf = None
			while True:
				buf = f.read(self.CHUNK_SIZE)
				if not buf:
					break
				h.update(buf)
		filehash = (ubinascii.hexlify(h.digest()).decode())
		self.log.log('{} hash is "{}", should be "{}"'.format(path, filehash, hash), 'ota')
		return filehash == hash

	def deleteOldVFS(self):
		deepDeleteFolder("/{}".format(self.nextbootpartition))
		os.mkdir("/{}".format(self.nextbootpartition))

	def copyVFS(self):
		self.deleteOldVFS()
		deepCopyFolder("/{}".format(self.curbootpartition), "/{}".format(self.nextbootpartition))

	def unpackTar(self):
		t = utar.TarFile(self.TAR_PATH)
		updatebasepath = "/{}/".format(self.nextbootpartition)
		for i in t:
			gc.collect()
			if i.type == utar.DIRTYPE:
				os.mkdir(updatebasepath+i.name.replace('/',''))
			else:
				with open(updatebasepath+i.name, 'wb') as ef:
					pf = t.extractfile(i)
					copyfileobj(pf, ef)

	def finishUpdate(self):
		esp.ota_set_boot_partition(bytearray(self.partitions[self.nextbootpartition][6]))
	        self.log.log("Update finished, restarting", 'ota')
		machine.reset()
```

## Example of how to call the updater class
```python
from foo import Logger, Updater
import urequests

log = Logger()
ota =  Updater(log)
# WARNING: you MUSST secure this connection in productive code, else anybody who can intercept this traffic has arbitrary code execution on your device. You could for example use ssl with certificate pinning
metadata = urequests.get('http://10.42.0.1:8000/updatemeta').json()
ota.update(metadata)
```

## Example server side
This server requires bottle, you can find it here: https://bottlepy.org/

```python
#!/usr/bin/env python3
from bottle import route, run, response, request, url, debug
import os
import json
import datetime
import hashlib

partfile = 'ota.img'
vfsfile = 'ota.tar'
otahost = 'http://10.42.0.1:8000'
chunk_size = 512

do_update = False

@route('/updatemeta', method=['POST', 'GET'])
def trackerprobe():
	if do_update:
		res['update'] = {}
		res['update']['partition'] = {'url': '{}/chunk/part'.format(otahost), 'size': int(os.stat(partfile).st_size), 'hash': hashlib.sha256(open(partfile, 'rb').read()).hexdigest()}
		res['update']['vfs'] = {'url': '{}/chunk/vfs'.format(otahost), 'size': int(os.stat(vfsfile).st_size), 'hash': hashlib.sha256(open(vfsfile, 'rb').read()).hexdigest()}
	print(res)
	return json.dumps(res)


@route('/chunk/<type>/<id:int>', name='chunk_id')
@route('/chunk/<type>/', name='chunk')
def chunk(type, id=0):
	path = None
	if type == 'part':
		path = partfile
	elif type == 'vfs':
		path = vfsfile
	if path:
		f = open(path, 'rb')
		f.seek(chunk_size*id)
		data = f.read(chunk_size)
		f.close()
		response.set_header('Content-type', 'application/octet-stream')
		print('type {}, chunk {}'.format(type, id))
		return data
	else:
		return None


debug()
run(host='0.0.0.0', port=8000)
```
