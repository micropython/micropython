.. currentmodule:: network
.. _network.USB_NET:

class USB_NET -- USB Network (NCM) interface
=============================================

This class allows you to control a USB Network Control Model (NCM) interface,
providing network connectivity over USB. When connected to a host computer, it
appears as a standard network interface.

The interface automatically assigns itself a link-local IP address based on the
device's unique hardware ID, ensuring a consistent address across reboots.

Example usage::

    import network
    nic = network.USB_NET()
    nic.active(True)
    print(nic.ifconfig())

    # now use socket as usual
    ...

.. note:: The USB NCM interface requires USB device mode to be enabled and
          properly configured with NCM support (``CFG_TUD_NCM=1``).

Constructors
------------

.. class:: USB_NET()

   Create a USB Network interface object. The interface is automatically
   initialized when the USB device is enumerated by the host.

   The interface uses a unique link-local IP address (169.254.x.1) generated
   from the device's MAC address.

Methods
-------

.. method:: USB_NET.active([state])

   With a parameter, it sets the interface active if *state* is true, otherwise
   it sets it inactive. When activated, the interface starts its DHCP server
   (if enabled) to provide IP addresses to the connected host.
   
   Without a parameter, it returns the current state.

.. method:: USB_NET.isconnected()

   Returns ``True`` if the USB network interface is up and active.
   Returns ``False`` otherwise.

.. method:: USB_NET.ifconfig([(ip, subnet, gateway, dns)])

   Get/set IP address, subnet mask, gateway and DNS.

   When called with no arguments, this method returns a 4-tuple with the above
   information.

   To set static IP configuration, pass a 4-tuple with the required information.
   For example::

    nic.ifconfig(('192.168.7.1', '255.255.255.0', '0.0.0.0', '8.8.8.8'))

.. method:: USB_NET.ipconfig(...)

   Get or set IP configuration parameters. See :meth:`network.AbstractNIC.ipconfig`.

.. method:: USB_NET.link_state([state])

   Get or set the network link state, simulating a physical cable connection.

   When called with no arguments, returns the current link state as a boolean
   (``True`` for link up, ``False`` for link down).

   When called with a boolean argument, sets the link state:
   
   - ``link_state(False)`` - Simulates "cable unplugged". The host computer will
     see the network interface as disconnected.
   - ``link_state(True)`` - Simulates "cable connected". The host computer will
     see the network interface as connected.

   This is useful for testing network error handling or temporarily disconnecting
   the network without deactivating the entire interface. When the link is down,
   no network traffic can flow, but the interface remains configured.

   Example::

    nic = network.USB_NET()
    nic.active(True)
    
    # Simulate unplugging the cable
    nic.link_state(False)
    print("Link is", "up" if nic.link_state() else "down")
    
    # Reconnect after 5 seconds
    time.sleep(5)
    nic.link_state(True)

Internet Gateway Configuration
------------------------------

By default, the USB network interface operates as a link-local network between
the device and the host computer. However, if the host computer is configured
to act as a gateway and share its internet connection with the USB network
interface, the device will automatically have internet access.

**Host Configuration for Internet Sharing:**

- **Windows**: Enable "Internet Connection Sharing" on your main network adapter
  and select the USB network interface as the connection to share with.
  
- **macOS**: In System Preferences → Sharing, enable "Internet Sharing" and
  share your connection from your main network interface to the USB network
  interface.
  
- **Linux**: Enable IP forwarding and configure NAT/masquerading for the USB
  network interface. For example::
  
    # Enable IP forwarding
    sudo sysctl net.ipv4.ip_forward=1
    
    # Setup NAT (replace eth0 with your internet-connected interface)
    sudo iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE
    sudo iptables -A FORWARD -i usb0 -o eth0 -j ACCEPT
    sudo iptables -A FORWARD -i eth0 -o usb0 -m state --state RELATED,ESTABLISHED -j ACCEPT

When a host connects and receives an IP address from the device's DHCP server,
the device automatically sets the host's IP as its default gateway, enabling
internet access if the host is configured for sharing.

DHCP Server
-----------

The USB network interface includes a built-in DHCP server (enabled by default)
that automatically assigns IP addresses to connected hosts. The DHCP server:

- Assigns addresses in the range x.x.x.16 to x.x.x.23 (where x.x.x is based
  on the device's IP network)
- Sets the device's IP as the DNS server for the host
- Automatically configures the host as the default gateway when it connects

This automatic configuration simplifies the connection process, as the host
computer doesn't need manual IP configuration.