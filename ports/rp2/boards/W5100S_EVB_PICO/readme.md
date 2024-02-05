# Wiznet W5100S-EVB-Pico

## Network Example

To use network / socket based code, connect ethernet port to network with DHCP running:

```
>>> import network
>>> nic = network.WIZNET5K()
>>> nic.ifconfig()
('192.168.0.18', '255.255.255.0', '192.168.0.1', '8.8.8.8')
>>> nic.dhcp(True)
('192.168.0.10', '255.255.255.0', '192.168.0.1', '192.168.0.1')
```
At this point standard network communications libraries should work.
