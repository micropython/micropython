# Workflows

Workflows are the process used to 1) manipulate files on the CircuitPython device and 2) interact
with the serial connection to CircuitPython. The serial connection is usually used to access the
REPL.

Starting with CircuitPython 3.x we moved to a USB-only workflow. Prior to that, we used the serial
connection alone to do the whole workflow. In CircuitPython 7.x, a BLE workflow was added with the
advantage of working with mobile devices. CircuitPython 8.x added a web workflow that works over the
local network (usually Wi-Fi) and a web browser. Other clients can also use the Web REST API. Boards
should clearly document which workflows are supported.

Code for workflows lives in `supervisor/shared`.

The workflow APIs are documented here.

## USB

These USB interfaces are enabled by default on boards with USB support. They are usable once the
device has been plugged into a host.

### CIRCUITPY drive
CircuitPython exposes a standard mass storage (MSC) interface to enable file manipulation over a
standard interface. This interface works underneath the file system at the block level so using it
excludes other types of workflows from manipulating the file system at the same time.

### CDC serial
CircuitPython exposes one CDC USB interface for CircuitPython serial. This is a standard serial
USB interface.

TODO: Document how it designates itself from the user CDC.

Setting baudrate 1200 and disconnecting will reboot into a bootloader. (Used by Arduino to trigger
a reset into bootloader.)

## BLE

The BLE workflow is enabled for nRF boards. By default, to prevent malicious access, it is disabled.
To connect to the BLE workflow, press the reset button while the status led blinks blue quickly
after the safe mode blinks. The board will restart and broadcast the file transfer service UUID
(`0xfebb`) along with the board's [Creation IDs](https://github.com/creationid/creators). This
public broadcast is done at a lower transmit level so the devices must be closer. On connection, the
device will need to pair and bond. Once bonded, the device will broadcast whenever disconnected
using a rotating key rather than a static one. Non-bonded devices won't be able to resolve it. After
connection, the central device can discover two default services. One for file transfer and one for
CircuitPython specifically that includes serial characteristics.

To change the default BLE advertising name without (or before) running user code, the desired name
can be put in the `settings.toml` file. The key is `CIRCUITPY_BLE_NAME`. It's limited to approximately
30 characters depending on the port's settings and will be truncated if longer.

### File Transfer API

CircuitPython uses [an open File Transfer API](https://github.com/adafruit/Adafruit_CircuitPython_BLE_File_Transfer)
to enable file system access.

### CircuitPython Service

The base UUID for the CircuitPython service is `ADAFXXXX-4369-7263-7569-7450794686e`. The `XXXX` is
replaced by the four specific digits below. The service itself is `0001`.

#### TX - `0002` / RX - `0003`

These characteristic work just like the Nordic Uart Service (NUS) but have different UUIDs to prevent
conflicts with user created NUS services.

#### Version - `0100`
Read-only characteristic that returns the UTF-8 encoded version string.

## Web
If the keys `CIRCUITPY_WIFI_SSID` and `CIRCUITPY_WIFI_PASSWORD` are set in `settings.toml`,
CircuitPython will automatically connect to the given Wi-Fi network on boot and upon reload.

If `CIRCUITPY_WEB_API_PASSWORD` is also set, the web workflow will also start.
The web workflow will only be enabled if the Wi-Fi connection succeeds upon boot.

The webserver is on port 80 unless overridden by `CIRCUITPY_WEB_API_PORT`. It also enables MDNS.
The name of the board as advertised to the network can be overridden by `CIRCUITPY_WEB_INSTANCE_NAME`.

Here is an example `/settings.toml`:

```bash
# To auto-connect to Wi-Fi
CIRCUITPY_WIFI_SSID="scottswifi"
CIRCUITPY_WIFI_PASSWORD="secretpassword"

# To enable the the webserver. Change this too!
# Leave the User field blank in the browser.
CIRCUITPY_WEB_API_PASSWORD="passw0rd"

CIRCUITPY_WEB_API_PORT=80
CIRCUITPY_WEB_INSTANCE_NAME=""
```

MDNS is used to resolve [`circuitpython.local`](http://circuitpython.local) to a device specific
hostname of the form `cpy-XXXXXX.local`. The `XXXXXX` is based on network MAC address. The device
also provides the MDNS service with service type `_circuitpython` and protocol `_tcp`.

Since port 80 (or the port assigned to `CIRCUITPY_WEB_API_PORT`) is used for web workflow, the `mdns`
[module](https://docs.circuitpython.org/en/latest/shared-bindings/mdns/index.html#mdns.Server.advertise_service)
can't advertise an additional service on that port.

### HTTP
The web server is HTTP 1.1 and may use chunked responses so that it doesn't need to precompute
content length.

The API generally consists of an HTTP method such as GET or PUT and a path. Requests and responses
also have headers. Responses will contain a status code and status text such as `404 Not Found`.
This API tries to use standard status codes to encode the status of the various operations. The
[Mozilla Developer Network HTTP docs](https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP)
are a great reference.

#### Examples
The examples use `curl`, a common command line program for issuing HTTP requests. The examples below
use `circuitpython.local` as the easiest way to work. If you have multiple active devices, you'll
want to use the specific `cpy-XXXXXX.local` version.

The examples also use `passw0rd` as the password placeholder. Replace it with your password before
running the example.

### `/`
The root welcome page links to the file system page and also displays other CircuitPython devices
found using MDNS service discovery. This allows web browsers to find other devices from one. (All
devices will respond to `circuitpython.local` so the device redirected to may vary.)

### CORS
The web server will allow requests from `cpy-XXXXXX.local`, `127.0.0.1`, the device's IP and
`code.circuitpython.org`. (`circuitpython.local` requests will be redirected to `cpy-XXXXXX.local`.)

### File REST API
All file system related APIs are protected by HTTP basic authentication. It is *NOT* secure but will
hopefully prevent some griefing in shared settings. The password is sent unencrypted so do not reuse
a password with something important. The user field is left blank.

The password is taken from `settings.toml` with the key `CIRCUITPY_WEB_API_PASSWORD`. If this is unset, the
server will respond with `403 Forbidden`. When a password is set, but not provided in a request, it
will respond `401 Unauthorized`.

#### `/fs/`

The `/fs/` page will respond with a directory browsing HTML once authenticated. This page is always
gzipped. If the `Accept: application/json` header is provided, then the JSON representation of the
root will be returned.

##### OPTIONS
When requested with the `OPTIONS` method, the server will respond with CORS related headers. Most
aren't needed for API use. They are there for the web browser.

* `Access-Control-Allow-Methods` - Varies with USB state. `GET, OPTIONS` when USB is active. `GET, OPTIONS, PUT, DELETE, MOVE` otherwise.

Example:

```sh
curl -v -u :passw0rd -X OPTIONS -L --location-trusted http://circuitpython.local/fs/
```

#### `/fs/<directory path>/`
Directory paths must end with a /. Otherwise, the path is assumed to be a file.

##### GET
Returns a JSON representation of the directory.

* `200 OK` - Directory exists and JSON returned
* `401 Unauthorized` - Incorrect password
* `403 Forbidden` - No `CIRCUITPY_WEB_API_PASSWORD` set
* `404 Not Found` - Missing directory

Returns information about each file in the directory:

* `name` - File name. No trailing `/` on directory names
* `directory` - `true` when a directory. `false` otherwise
* `modified_ns` - File modification time in nanoseconds since January 1st, 1970. May not use full resolution
* `file_size` - File size in bytes. `0` for directories

Example:

```sh
curl -v -u :passw0rd -H "Accept: application/json" -L --location-trusted http://circuitpython.local/fs/lib/hello/
```

```json
[
	{
		"name": "world.txt",
		"directory": false,
		"modified_ns": 946934328000000000,
		"file_size": 12
	}
]
```

##### PUT
Tries to make a directory at the given path. Request body is ignored. The custom `X-Timestamp`
header can provide a timestamp in milliseconds since January 1st, 1970 (to match JavaScript's file
time resolution) used for the directories modification time. The RTC time will used otherwise.

Returns:

* `204 No Content` - Directory exists
* `201 Created` - Directory created
* `401 Unauthorized` - Incorrect password
* `403 Forbidden` - No `CIRCUITPY_WEB_API_PASSWORD` set
* `409 Conflict` - USB is active and preventing file system modification
* `404 Not Found` - Missing parent directory
* `500 Server Error` - Other, unhandled error

Example:

```sh
curl -v -u :passw0rd -X PUT -L --location-trusted http://circuitpython.local/fs/lib/hello/world/
```

##### Move
Moves the directory at the given path to ``X-Destination``. Also known as rename.

The custom `X-Destination` header stores the destination path of the directory.

* `201 Created` - Directory renamed
* `401 Unauthorized` - Incorrect password
* `403 Forbidden` - No `CIRCUITPY_WEB_API_PASSWORD` set
* `404 Not Found` - Source directory not found or destination path is missing
* `409 Conflict` - USB is active and preventing file system modification
* `412 Precondition Failed` - The destination path is already in use

Example:

```sh
curl -v -u :passw0rd -X MOVE -H "X-Destination: /fs/lib/hello2/" -L --location-trusted http://circuitpython.local/fs/lib/hello/
```

##### DELETE
Deletes the directory and all of its contents.

* `204 No Content` - Directory and its contents deleted
* `401 Unauthorized` - Incorrect password
* `403 Forbidden` - No `CIRCUITPY_WEB_API_PASSWORD` set
* `404 Not Found` - No directory
* `409 Conflict` - USB is active and preventing file system modification

Example:

```sh
curl -v -u :passw0rd -X DELETE -L --location-trusted http://circuitpython.local/fs/lib/hello2/world/
```


#### `/fs/<file path>`

##### PUT
Stores the provided content to the file path.

The custom `X-Timestamp` header can provide a timestamp in milliseconds since January 1st, 1970
(to match JavaScript's file time resolution) used for the directories modification time. The RTC
time will used otherwise.

Returns:

* `201 Created` - File created and saved
* `204 No Content` - File existed and overwritten
* `401 Unauthorized` - Incorrect password
* `403 Forbidden` - No `CIRCUITPY_WEB_API_PASSWORD` set
* `404 Not Found` - Missing parent directory
* `409 Conflict` - USB is active and preventing file system modification
* `413 Payload Too Large` - `Expect` header not sent and file is too large
* `417 Expectation Failed` - `Expect` header sent and file is too large
* `500 Server Error` - Other, unhandled error

If the client sends the `Expect` header, the server will reply with `100 Continue` when ok.

Example:

```sh
echo "Hello world" >> test.txt
curl -v -u :passw0rd -T test.txt -L --location-trusted http://circuitpython.local/fs/lib/hello/world.txt
```

##### GET
Returns the raw file contents. `Content-Type` will be set based on extension:

* `text/plain` - `.py`, `.txt`
* `text/javascript` - `.js`
* `text/html` - `.html`
* `application/json` - `.json`
* `application/octet-stream` - Everything else

Will return:
* `200 OK` - File exists and file returned
* `401 Unauthorized` - Incorrect password
* `403 Forbidden` - No `CIRCUITPY_WEB_API_PASSWORD` set
* `404 Not Found` - Missing file

Example:

```sh
curl -v -u :passw0rd -L --location-trusted http://circuitpython.local/fs/lib/hello/world.txt
```


##### Move
Moves the file at the given path to the ``X-Destination``. Also known as rename.

The custom `X-Destination` header stores the destination path of the file.

* `201 Created` - File renamed
* `401 Unauthorized` - Incorrect password
* `403 Forbidden` - No `CIRCUITPY_WEB_API_PASSWORD` set
* `404 Not Found` - Source file not found or destination path is missing
* `409 Conflict` - USB is active and preventing file system modification
* `412 Precondition Failed` - The destination path is already in use

Example:

```sh
curl -v -u :passw0rd -X MOVE -H "X-Destination: /fs/lib/hello/world2.txt" -L --location-trusted http://circuitpython.local/fs/lib/hello/world.txt
```


##### DELETE
Deletes the file.


* `204 No Content` - File existed and deleted
* `401 Unauthorized` - Incorrect password
* `403 Forbidden` - No `CIRCUITPY_WEB_API_PASSWORD` set
* `404 Not Found` - File not found
* `409 Conflict` - USB is active and preventing file system modification

Example:

```sh
curl -v -u :passw0rd -X DELETE -L --location-trusted http://circuitpython.local/fs/lib/hello/world2.txt
```

### `/cp/`

`/cp/` serves basic info about the CircuitPython device and others discovered through MDNS. It is
not protected by basic auth in case the device is someone elses.

Only `GET` requests are supported and will return `405 Method Not Allowed` otherwise.

#### `/cp/devices.json`

Returns information about other devices found on the network using MDNS.

* `total`: Total MDNS response count. May be more than in `devices` if internal limits were hit.
* `devices`: List of discovered devices.
	* `hostname`: MDNS hostname
	* `instance_name`: MDNS instance name. Defaults to human readable board name.
	* `port`: Port of CircuitPython Web API
	* `ip`: IP address

Example:
```sh
curl -v -L http://circuitpython.local/cp/devices.json
```

```json
{
	"total": 1,
	"devices": [
		{
			"hostname": "cpy-951032",
			"instance_name": "Adafruit Feather ESP32-S2 TFT",
			"port": 80,
			"ip": "192.168.1.235"
		}
	]
}
```

#### `/cp/diskinfo.json`

Returns information about the attached disk(s). A list of objects, one per disk.

* `root`: Filesystem path to the root of the disk.
* `free`: Count of free bytes on the disk.
* `block_size`: Size of a block in bytes.
* `writable`: True when CircuitPython and the web workflow can write to the disk. USB may claim a disk instead.
* `total`: Total bytes that make up the disk.

Example:
```sh
curl -v -L http://circuitpython.local/cp/diskinfo.json
```

```json
[{
	"root": "/",
	"free": 2964992,
	"block_size": 512,
	"writable": true,
	"total": 2967552
}]
```

#### `/cp/serial/`


Serves a basic serial terminal program when a `GET` request is received without the
`Upgrade: websocket` header. Otherwise the socket is upgraded to a WebSocket. See WebSockets below for more detail.

This is an authenticated endpoint in both modes.

#### `/cp/version.json`

Returns information about the device.

* `web_api_version`: Between `1` and `3`. This versions the rest of the API and new versions may not be backwards compatible. See below for more info.
* `version`: CircuitPython build version.
* `build_date`: CircuitPython build date.
* `board_name`: Human readable name of the board.
* `mcu_name`: Human readable name of the microcontroller.
* `board_id`: Board id used in code and on circuitpython.org.
* `creator_id`: Creator ID for the board.
* `creation_id`: Creation ID for the board, set by the creator.
* `hostname`: MDNS hostname.
* `port`: Port of CircuitPython Web Service.
* `ip`: IP address of the device.

Example:
```sh
curl -v -L http://circuitpython.local/cp/version.json
```

```json
{
	"web_api_version": 1,
	"version": "8.0.0-alpha.1-20-ge1d4518a9-dirty",
	"build_date": "2022-06-24",
	"board_name": "ESP32-S3-USB-OTG-N8",
	"mcu_name": "ESP32S3",
	"board_id": "espressif_esp32s3_usb_otg_n8",
	"creator_id": 12346,
	"creation_id": 28683,
	"hostname": "cpy-f57ce8",
	"port": 80,
	"ip": "192.168.1.94"
}
```

#### `/code/`

The `/code/` page returns a small static html page that will pull in and load the full code editor from
[code.circuitpython.org](https://code.circuitpython.org) for a full code editor experience. Because most
of the resources reside online instead of the device, an active internet connection is required.

### Static files

* `/favicon.ico` - Blinka
* `/directory.js` - JavaScript for `/fs/`
* `/welcome.js` - JavaScript for `/`

### WebSocket

The CircuitPython serial interactions are available over a WebSocket. A WebSocket begins as a
special HTTP request that gets upgraded to a WebSocket. Authentication happens before upgrading.

WebSockets are *not* bare sockets once upgraded. Instead they have their own framing format for data.
CircuitPython can handle PING and CLOSE opcodes. All others are treated as TEXT. Data to
CircuitPython is expected to be masked UTF-8, as the spec requires. Data from CircuitPython to the
client is unmasked. It is also unbuffered so the client will get a variety of frame sizes.

Only one WebSocket at a time is supported.

### Versions

* `1` - Initial version.
* `2` - Added `/cp/diskinfo.json`.
* `3` - Changed `/cp/diskinfo.json` to return a list in preparation for multi-disk support.
