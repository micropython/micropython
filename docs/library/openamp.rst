:mod:`openamp` -- provides standard Asymmetric Multiprocessing (AMP) support
============================================================================

.. module:: openamp
   :synopsis: provides standard Asymmetric Multiprocessing (AMP) support

The ``openamp`` module provides a standard inter-processor communications infrastructure
for MicroPython. The module handles all of the details of OpenAMP, such as setting up
the shared resource table, initializing vrings, etc. It provides an API for using the
RPMsg bus infrastructure with the `Endpoint` class, and provides processor Life Cycle
Management (LCM) support, such as loading firmware and starting and stopping a remote
core, via the `RemoteProc` class.

Example usage::

    import openamp

    def ept_recv_callback(src, data):
        print("Received message on endpoint", data)

    # Create a new RPMsg endpoint to communicate with the remote core.
    ept = openamp.Endpoint("vuart-channel", callback=ept_recv_callback)

    # Create a RemoteProc object, load its firmware and start it.
    rproc = openamp.RemoteProc("virtual_uart.elf") # Or entry point address (ex 0x081E0000)
    rproc.start()

    while True:
        if ept.is_ready():
            ept.send("data")

Functions
---------

.. function:: new_service_callback(ns_callback)

    Set the new service callback.

    The *ns_callback* argument is a function that will be called when the remote processor
    announces new services. At that point the host processor can choose to create the
    announced endpoint, if this particular service is supported, or ignore it if it's
    not. If this function is not set, the host processor should first register the
    endpoint locally, and it will be automatically bound when the remote announces
    the service.

Endpoint class
--------------

.. class:: Endpoint(name, callback, src=ENDPOINT_ADDR_ANY, dest=ENDPOINT_ADDR_ANY)

   Construct a new RPMsg Endpoint. An endpoint is a bidirectional communication
   channel between two cores.

   Arguments are:

        - *name* is the name of the endpoint.
        - *callback* is a function that is called when the endpoint receives data with the
          source address of the remote point, and the data as bytes passed by reference.
        - *src* is the endpoint source address. If none is provided one will be assigned
          to the endpoint by the library.
        - *dest* is the endpoint destination address. If the endpoint is created from the
          new_service_callback, this must be provided and it must match the remote endpoint's
          source address. If the endpoint is registered locally, before the announcement, the
          destination address will be assigned by the library when the endpoint is bound.

.. method:: Endpoint.deinit()

   Destroy the endpoint and release all of its resources.

.. method:: Endpoint.is_ready()

   Returns True if the endpoint is ready to send (i.e., has both a source and destination addresses)

.. method:: Endpoint.send(src=-1, dest=-1, timeout=-1)

   Send a message to the remote processor over this endpoint.

   Arguments are:

        - *src* is the source endpoint address of the message. If none is provided, the
          source address the endpoint is bound to is used.
        - *dest* is the destination endpoint address of the message. If none is provided,
          the destination address the endpoint is bound to is used.
        - *timeout* specifies the time in milliseconds to wait for a free buffer. By default
          the function is blocking.

RemoteProc class
----------------

.. class:: RemoteProc(entry)

   The RemoteProc object provides processor Life Cycle Management (LCM) support, such as
   loading firmware, starting and stopping a remote core.

   The *entry* argument can be a path to firmware image, in which case the firmware is
   loaded from file to its target memory, or an entry point address, in which case the
   firmware must be loaded already at the given address.

.. method:: RemoteProc.start()

   Starts the remote processor.

.. method:: RemoteProc.stop()

   Stops the remote processor. The exact behavior is platform-dependent. On the STM32H7 for
   example it's not possible to stop and then restart the Cortex-M4 core, so a complete
   system reset is performed on a call to this function.

.. method:: RemoteProc.shutdown()

   Shutdown stops the remote processor and releases all of its resources. The exact behavior
   is platform-dependent, however typically it disables power and clocks to the remote core.
   This function is also used as the finaliser (i.e., called when ``RemoteProc`` object is
   collected). Note that on the STM32H7, it's not possible to stop and then restart the
   Cortex-M4 core, so a complete system reset is performed on a call to this function.
