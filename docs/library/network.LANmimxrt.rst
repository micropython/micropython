.. currentmodule:: network
.. _mimxrt_network.LAN:

class LAN -- control Ethernet modules of the MIMXRT port
========================================================

This class allows you to control the Ethernet interface of the MIMXRT
MCUs. The PHY hardware type is board-specific.

Example usage::

    import network
    nic = network.LAN(0)
    print(nic.ifconfig())

    # now use socket as usual
    ...


Constructors
------------

.. class:: LAN(id, *, phy_type=<board_default>, phy_addr=<board_default>, phy_clock=<board_default>)

   Create a LAN driver object, initialise the LAN module using the given
   PHY driver name, and return the LAN object.

   Arguments are:

     - *id* is the number of the Ethernet port, either 0 or 1.
     - *phy_type* is the name of the PHY driver. For most board the on-board PHY has to be used and
       is the default. Suitable values are PHY_KSZ8081, PHY_DP83825, PHY_DP83848, PHY_LAN8720, and PHY_RTL8211F.
     - *phy_addr* specifies the address of the PHY interface. As with *phy_type*, the hardwired value has
       to be used for most boards and that value is the default.
     - *phy_clock* specifies, whether the data clock is provided by the Ethernet controller or the PYH interface.
       The default value is the one that matches the board. If set to *True*, the clock is driven by the
       Ethernet controller, otherwise by the PHY interface. 

   For example, with the Seeed Arch Mix board you can  use::

     nic = network.LAN(0, phy_type=PHY_LAN8720, phy_addr=2, phy_clock=False)

Methods
-------

.. method:: LAN.active([state])

   With a parameter, it sets the interface active id *True*, otherwise inactive.
   Without a parameter, it returns the state.

.. method:: LAN.isconnected()

   Returns ``True`` if the physical Ethernet link is connected and up.
   Returns ``False`` otherwise.

.. method:: LAN.status()

   Returns the LAN status.

.. method:: LAN.ifconfig([(ip, subnet, gateway, dns)])

   Get/set IP address, subnet mask, gateway and DNS.

   When called with no arguments, this method returns a 4-tuple with the above information.

   To set the above values, pass a 4-tuple with the required information.  For example::

    nic.ifconfig(('192.168.0.4', '255.255.255.0', '192.168.0.1', '8.8.8.8'))

.. method:: LAN.config(config_parameters)

   Sets or gets parameters of the LAN interface. The only parameter that can be retrieved is the MAC address, using::

      mac = LAN.config("mac")

   The parameters that can be set are trace and low_power.

   trace=n  sets a trace level. Suitable values for n are:

   - 2: trace TX
   - 4: trace RX
   - 8: Full trace

   low_power=True | False  Set or clear low power mode.

