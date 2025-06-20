WLAN step by step
=================

The WLAN is a system feature of the WiPy, therefore it is always enabled
(even while in ``machine.SLEEP``), except when deepsleep mode is entered.

In order to retrieve the current WLAN instance, do::

   >>> from network import WLAN
   >>> wlan = WLAN() # we call the constructor without params

You can check the current mode (which is always ``WLAN.AP`` after power up)::

   >>> wlan.mode()

.. warning::
    When you change the WLAN mode following the instructions below, your WLAN
    connection to the WiPy will be broken. This means you will not be able
    to run these commands interactively over the WLAN.

    There are two ways around this::
     1. put this setup code into your :ref:`boot.py file<wipy_filesystem>` so that it gets executed automatically after reset.
     2. :ref:`duplicate the REPL on UART <wipy_uart>`, so that you can run commands via USB.

Connecting to your home router
------------------------------

The WLAN network card always boots in ``WLAN.AP`` mode, so we must first configure
it as a station::

   from network import WLAN
   wlan = WLAN(mode=WLAN.STA)


Now you can proceed to scan for networks::

    nets = wlan.scan()
    for net in nets:
        if net.ssid == 'mywifi':
            print('Network found!')
            wlan.connect(net.ssid, auth=(net.sec, 'mywifikey'), timeout=5000)
            while not wlan.isconnected():
                machine.idle() # save power while waiting
            print('WLAN connection succeeded!')
            break

Assigning a static IP address when booting
------------------------------------------

If you want your WiPy to connect to your home router after boot-up, and with a fixed
IP address so that you can access it via telnet or FTP, use the following script as /flash/boot.py::

   import machine
   from network import WLAN
   wlan = WLAN() # get current object, without changing the mode

   if machine.reset_cause() != machine.SOFT_RESET:
       wlan.init(WLAN.STA)
       # configuration below MUST match your home router settings!!
       wlan.ifconfig(config=('192.168.178.107', '255.255.255.0', '192.168.178.1', '8.8.8.8'))

   if not wlan.isconnected():
       # change the line below to match your network ssid, security and password
       wlan.connect('mywifi', auth=(WLAN.WPA2, 'mywifikey'), timeout=5000)
       while not wlan.isconnected():
           machine.idle() # save power while waiting

.. note::

   Notice how we check for the reset cause and the connection status, this is crucial in order
   to be able to soft reset the WiPy during a telnet session without breaking the connection.
