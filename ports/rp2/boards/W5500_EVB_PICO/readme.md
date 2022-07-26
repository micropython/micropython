# Wiznet W5500-EVB-Pico

## Network Example

To use network / socket based code, connect ethernet port to network with DHCP running:

```
>>> import network
>>> nic = network.WIZNET5K()
>>> nic.active(True)
>>> nic.ifconfig()
('0.0.0.0', '0.0.0.0', '0.0.0.0', '0.0.0.0')
>>> nic.ifconfig("dhcp")
('192.168.0.10', '255.255.255.0', '192.168.0.1', '192.168.0.1')
>>> nic.isconnected()
True
```
At this point standard network communications libraries should work.
