# Circuitpython as Client

This example demonstrates the use of Socket as a server, accessed by a client program on a host development machine. This Circuitpython sketch uses the Bind, Listen, Accept, and recvfrom_into calls.

## Prerequisites

Circuitpython V6.2.0 minimum. Neither the host or client sketch has installed module prerequisites.

## Setup

Make sure that both devices are using the same WIFI! The Circuitpython Server will automatically pick an IP and print it over the CDC monitor. Copy this value into the host client sketch. Start the Server on Circuitpython first, then run the client sketch in this folder:

```
python host-client.py
```

Each sketch can have Timeout values changed.

## Expected behavior

The host machine will connect to the server running on the Circuitpython device, and send a "Hello, world" string which is then returned.

Expected client output:

```
Connecting
Sending
Receiving
Received b'Hello, world'
```

Expected server output:
```
Connecting to Wifi
Finding IP address
192.168.10.128
Creating socket
Accepting connections
Connected by ('192.168.10.179', 33274)
Receiving
bytearray(b'Hello, world')
Sending
```
