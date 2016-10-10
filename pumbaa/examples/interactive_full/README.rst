Interactive Interpreter with Network enabled
============================================

The Python interactive interpreter example. 

Build the application and upload it to the board.

.. code-block:: text

   make -s BOARD=esp12e upload
   make -s BOARD=esp12e console

Execute Python code in the interpreter. The example below creates TCP
sockets on an ESP12-E and sends data to and receives data from a PC.
   
.. code-block:: python

   MicroPython v1.8.3-88-gf98bb2d on 2016-09-17; Arduino Due with SAM3X8E
   Type "help()" for more information.
   >>> import socket
   >>>
   >>> listener = socket.socket()
   >>> listener.bind(('192.168.1.103', 8000))
   >>> listener.listen(5)
   >>> client, address = listener.accept()
   >>> client.recv(5)
   b'12345'
   >>> client.send('erik')
   4
   >>> client.close()
   >>>
   >>> server = socket.socket()
   >>> server.connect(('192.168.1.100', 8000))
   >>> server.send('12345')
   5
   >>> server.recv(4)
   b'erik'
   >>> server.close()
