# Circuitpython as Client

This example demonstrates the use of Socket as a client, accessing a server on a host development machine. This Circuitpython sketch uses the Connect, Send, and Recv_Into methods.

## Prerequisites

Circuitpython V6.2.0 minimum. Neither the host or client sketch has installed module prerequisites.

## Setup

Find a viable IP address for the host machine first and insert it in both sketches as HOST. On mac, this can be done by going to System Preferences/Network and checking the IP address used to connect to the local wireless network. Make sure that both devices are using the same WIFI!

Each sketch can have Timeout values changed. The host sketch usually needs a value above 0, or the recv() will fail. Currently, Circuitpython's Connect function is always blocking, so changing the client timeout will not cause much change in behavior.

Start the Server on the host PC first, within this folder:

```
python host-server.py
```

Then, reload the client sketch in Circuitpython.

## Expected Behavior

The example should connect to a server running on the host machine. The client will send a "Hello world" string to the server, which will return it.

Expected client output:

```
Connecting to wifi
Creating Socket
Connecting
Sending
Receiving
bytearray(b'Hello, world')
```

Expected Server output (IP/port values will vary):

```
Create Socket
Accepting connections
Connected by ('192.168.10.128', 64509)
got: b'Hello, world'
sent: b'Hello, world'
```
