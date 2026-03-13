.. _library_io.IOBase:

class ``io.IOBase``
===================

``io.IOBase`` is the abstract base class for all I/O classes. It provides
the basic interface for stream handling.

While ``IOBase`` itself is not meant to be instantiated directly, it serves as a
blueprint for creating custom stream-like objects in MicroPython. Objects
that inherit from ``IOBase`` are expected to implement methods for reading,
writing, and potentially other stream control operations.

MicroPython uses a stream protocol that ``IOBase`` helps to define. When an
object implements this protocol, it can be used with functions and methods
that expect a stream-like object (e.g., for file operations, network
communication, etc.).

Constructors
------------

.. class:: IOBase()

   This class is not intended to be constructed directly. Subclasses should
   provide their own constructors.

Methods
-------

The following methods are part of the ``IOBase`` interface and are typically
implemented by subclasses. If a subclass does not support a particular operation,
it should raise an appropriate exception (e.g., ``OSError`` or ``UnsupportedOperation``).

.. method:: read(size=-1)

   Read and return up to *size* bytes. If *size* is omitted, negative, or ``None``,
   it reads until EOF.
   Returns an empty ``bytes`` object at EOF.
   If the underlying stream is non-blocking and no data is available, it may
   return ``None``.

.. method:: readinto(buf, nbytes=0)

   Read up to *nbytes* into *buf* (a ``bytearray``-like object). If *nbytes*
   is not specified or is 0, reads up to ``len(buf)`` bytes.
   Returns the number of bytes read and stored into *buf*.
   Returns 0 on EOF.
   If the underlying stream is non-blocking and no data is available, it may
   return ``None``.

.. method:: readline(size=-1)

   Read and return one line from the stream. If *size* is specified, at most
   *size* bytes will be read.
   The line terminator is always ``b'\n'`` for binary files; for text files,
   the *newline* argument to ``open()`` can be used to select the line
   terminator(s) recognized.
   Returns an empty ``bytes`` object on EOF.

.. method:: write(buf)

   Write the given ``bytes``-like object, *buf*, to the underlying stream.
   Returns the number of bytes written, which may be less than ``len(buf)`` if
   the stream is non-blocking.
   If the stream is non-blocking and no data can be written, it may return ``None``.

.. method:: flush()

   Flush the write buffers of the stream, if applicable. This does nothing for
   read-only and non-blocking streams.

.. method:: close()

   Flush and close the stream. This method has no effect if the file is already
   closed. Once the file is closed, any operation on the file (e.g. reading or
   writing) will raise a ``ValueError``.

.. method:: seek(offset, whence=0)

   Change the stream position to the given byte *offset*. *offset* is interpreted
   relative to the position indicated by *whence*. The default value for *whence* is
   ``SEEK_SET`` (absolute file positioning). Values for *whence* are:

   * ``0`` -- start of the stream (the default); *offset* should be zero or positive
   * ``1`` -- current stream position; *offset* may be negative
   * ``2`` -- end of the stream; *offset* is usually negative

   Returns the new absolute position.

.. method:: tell()

   Return the current stream position.

.. method:: ioctl(request, arg=0)

   This method is specific to MicroPython and provides a way to send
   control commands to the underlying stream. The *request* is an integer
   representing the command, and *arg* is an optional argument for the command.
   The interpretation of *request* and *arg*, and the return value, are
   device-dependent. A common use case is to configure or query
   hardware-specific parameters or stream properties.

   While some ``ioctl`` requests are device-specific, MicroPython defines
   a set of standard stream control requests. These are defined in the C header
   file ``py/stream.h``. User code can access these requests by defining
   Python constants with the corresponding integer values.

   Standard ``ioctl`` requests:

   * ``MP_STREAM_FLUSH`` (1): Flush the stream's write buffer.

     * ``arg``: Not used.
     * Returns: 0 on success, or an OSError error code.
   * ``MP_STREAM_SEEK`` (2): Seek to a position in the stream.

     * ``arg``: A 3-element tuple ``(offset, whence, new_position_ptr)`` or a uctypes structure. ``new_position_ptr`` would be a memory location (e.g., a 1-element bytearray or an object created with uctypes) where the new position is stored by the C implementation.
       ``offset`` (int): The byte offset.
       ``whence`` (int): 0 for SEEK_SET, 1 for SEEK_CUR, 2 for SEEK_END.
       ``&new_position`` (int, passed by reference e.g. via a 1-element bytearray or array):
       This element will be updated with the new absolute stream position after the seek.
     * Returns: 0 on success, or an OSError error code.
     * Note: This provides a C-level seek. For Python-level seek, use the ``seek()`` method.
       The exact structure of ``arg`` for passing the pointer to ``new_position_ptr`` depends on the stream's C implementation.
   * ``MP_STREAM_POLL`` (3): Poll the stream for readiness.

     * ``arg`` (int): A combination of poll flags (e.g., ``MP_STREAM_POLL_RD | MP_STREAM_POLL_WR``)
       indicating which events to check for.
     * Returns: A bitmask of the poll flags that are ready.
       If the stream is not ready for any of the requested events, it may return 0
       or raise ``OSError(MP_EWOULDBLOCK)`` if non-blocking.
   * ``MP_STREAM_CLOSE`` (4): Close the stream.

     * ``arg``: Not used.
     * Returns: 0 on success, or an OSError error code.
     * Note: This provides a C-level close. For Python-level close, use the ``close()`` method.
   * ``MP_STREAM_TIMEOUT`` (5): Get or set the stream timeout for blocking operations.

     * ``arg`` (int): Timeout value in milliseconds.
       If ``arg`` is the special value ``-1`` (or ``0xFFFFFFFF``), get the current timeout.
       If ``arg == 0``, set to non-blocking.
       If ``arg > 0``, set timeout.
     * Returns: The current timeout value in milliseconds before the change (if setting) or the current timeout (if getting).
       Can also return an OSError error code on failure.
   * ``MP_STREAM_GET_OPTS`` (6): Get stream options. (Specific to stream type)
   * ``MP_STREAM_SET_OPTS`` (7): Set stream options. (Specific to stream type)
   * ``MP_STREAM_GET_DATA_OPTS`` (8): Get data/message options. (Specific to stream type)
   * ``MP_STREAM_SET_DATA_OPTS`` (9): Set data/message options. (Specific to stream type)
   * ``MP_STREAM_GET_FILENO`` (10): Get the underlying file descriptor number, if applicable.

     * ``arg``: Not used.
     * Returns: The file descriptor number, or -1 if not applicable or on error.
   * ``MP_STREAM_GET_BUFFER_SIZE`` (11): Get the preferred internal buffer size for the stream,
     if applicable (e.g., for efficient block-based operations).

     * ``arg``: Not used.
     * Returns: The buffer size, or 0 if not applicable or on error.

   Poll event flags for ``MP_STREAM_POLL`` (can be ORed together):

   * ``MP_STREAM_POLL_RD`` (0x0001): Data available for reading.
   * ``MP_STREAM_POLL_WR`` (0x0004): Stream is ready for writing.
   * ``MP_STREAM_POLL_ERR`` (0x0008): An error has occurred on the stream.
   * ``MP_STREAM_POLL_HUP`` (0x0010): Stream has been hung up (e.g., connection closed).
   * ``MP_STREAM_POLL_NVAL`` (0x0020): Invalid request (e.g., polling on a non-pollable stream).

   Subclasses should implement ``ioctl`` to handle these standard requests where
   applicable, and any custom requests. If a request is not supported,
   it's common to return -1 or raise an ``OSError``.

Error Handling
--------------

If an operation is not supported by a stream, ``UnsupportedOperation`` (which
is an alias for ``OSError`` in MicroPython) or ``TypeError`` may be raised.
Other ``OSError`` exceptions may be raised for I/O errors.

Example
-------

Here is an example of a custom stream class inheriting from `io.IOBase`,
focusing on the ``ioctl`` method and standard MicroPython stream requests:

.. code-block:: python

    import io
    import uctypes # For a more C-like MP_STREAM_SEEK arg representation
    import uerrno  # For error codes

    # Standard IOCTL requests (values from py/stream.h)
    MP_STREAM_FLUSH = const(1)
    MP_STREAM_SEEK = const(2)
    MP_STREAM_POLL = const(3)
    MP_STREAM_CLOSE = const(4)
    MP_STREAM_TIMEOUT = const(5)
    MP_STREAM_GET_FILENO = const(10)
    MP_STREAM_GET_BUFFER_SIZE = const(11)

    # Poll flags for MP_STREAM_POLL (values from py/stream.h)
    MP_STREAM_POLL_RD = const(0x0001)
    MP_STREAM_POLL_WR = const(0x0004)
    MP_STREAM_POLL_ERR = const(0x0008)
    MP_STREAM_POLL_HUP = const(0x0010)
    # MP_STREAM_POLL_NVAL = const(0x0020) # Less commonly used in Python examples

    class MyExampleStream(io.IOBase):
        def __init__(self, initial_data=b"abcdefghijklmnopqrstuvwxyz"):
            self.data = bytearray(initial_data)
            self.pos = 0
            self.closed = False
            self.timeout_ms = -1 # Blocking by default (MicroPython convention)
            print(f"MyExampleStream initialized. len(data)={len(self.data)}, pos={self.pos}")

        def readinto(self, buf, nbytes=0):
            if self.closed:
                raise ValueError("I/O operation on closed stream")
            if self.pos >= len(self.data):
                return 0 # EOF
            
            if nbytes == 0:
                nbytes = len(buf)
            else:
                nbytes = min(nbytes, len(buf))

            actual_read = min(nbytes, len(self.data) - self.pos)
            buf[:actual_read] = self.data[self.pos : self.pos + actual_read]
            self.pos += actual_read
            print(f"readinto: read {actual_read} bytes. New pos={self.pos}")
            return actual_read

        def write(self, buf):
            if self.closed:
                raise ValueError("I/O operation on closed stream")
            # This stream is primarily read-only for simplicity after init.
            # A real writable stream would append or overwrite self.data.
            # We'll pretend it's writable for MP_STREAM_POLL_WR demonstration.
            print(f"write: received {len(buf)} bytes (pretending to write).")
            return len(buf)

        def close(self):
            self.ioctl(MP_STREAM_CLOSE, 0)
            print("Stream closed via .close() method wrapper.")

        def ioctl(self, request, arg): # Explicitly require arg for clarity
            # print(f"IOCTL ENTER: request={request}, arg={arg}, closed={self.closed}")
            if self.closed and request != MP_STREAM_CLOSE:
                raise ValueError("I/O operation on closed stream")

            if request == MP_STREAM_FLUSH:
                print(f"IOCTL MP_STREAM_FLUSH: Flushing (no-op for this stream).")
                return 0
            
            elif request == MP_STREAM_SEEK:
                # arg for C is typically a pointer to a struct or array:
                # struct { int32_t offset; int32_t whence; int32_t *new_pos; }
                # In Python, we might pass a mutable object like a bytearray(4) for new_pos
                # or expect `arg` to be a tuple (offset, whence) and the ioctl updates
                # an internal new_pos_ptr if the C layer expects that.
                # This example simulates a Python-friendly ioctl for seek.
                # The standard .seek() method is usually preferred in Python.
                if not isinstance(arg, tuple) or len(arg) != 2:
                    print(f"IOCTL MP_STREAM_SEEK: Invalid arg format {arg}, expected (offset, whence).")
                    return -uerrno.EINVAL
                
                offset, whence = arg
                old_stream_pos = self.pos
                
                if whence == 0:   # SEEK_SET
                    new_pos = offset
                elif whence == 1: # SEEK_CUR
                    new_pos = self.pos + offset
                elif whence == 2: # SEEK_END
                    new_pos = len(self.data) + offset
                else:
                    print(f"IOCTL MP_STREAM_SEEK: Invalid whence value {whence}.")
                    return -uerrno.EINVAL

                if new_pos < 0:
                    # Do not change position if seek is invalid
                    print(f"IOCTL MP_STREAM_SEEK: Seek to negative position {new_pos} invalid.")
                    return -uerrno.EINVAL
                
                self.pos = max(0, min(new_pos, len(self.data)))
                print(f"IOCTL MP_STREAM_SEEK: Old pos={old_stream_pos}, offset={offset}, whence={whence}. New pos={self.pos}.")
                # If arg was a C struct pointer, we'd update *new_pos_ptr here.
                # For this Python simulation, returning 0 for success.
                return 0

            elif request == MP_STREAM_POLL:
                poll_mask = arg
                ready_mask = 0
                print(f"IOCTL MP_STREAM_POLL: Polling for events in mask {poll_mask:#06x}.")
                
                if (poll_mask & MP_STREAM_POLL_RD):
                    if not self.closed and self.pos < len(self.data):
                        ready_mask |= MP_STREAM_POLL_RD
                        print("  POLL: MP_STREAM_POLL_RD is ready.")
                
                if (poll_mask & MP_STREAM_POLL_WR):
                    if not self.closed: # This example stream is always "writable"
                        ready_mask |= MP_STREAM_POLL_WR
                        print("  POLL: MP_STREAM_POLL_WR is ready.")
                
                if (poll_mask & MP_STREAM_POLL_ERR):
                    # Simulate an error condition if needed for an example
                    pass 
                
                if (poll_mask & MP_STREAM_POLL_HUP):
                    if self.closed:
                        ready_mask |= MP_STREAM_POLL_HUP
                        print("  POLL: MP_STREAM_POLL_HUP is set (stream closed).")
                
                print(f"  POLL: Returning ready_mask {ready_mask:#06x}.")
                return ready_mask

            elif request == MP_STREAM_CLOSE:
                self.closed = True
                print(f"IOCTL MP_STREAM_CLOSE: Stream has been closed.")
                return 0

            elif request == MP_STREAM_TIMEOUT:
                # arg == -1 (or 0xFFFFFFFF) means get current timeout
                # arg == 0 means set non-blocking
                # arg > 0 means set timeout_ms
                old_timeout = self.timeout_ms
                if arg == -1 or arg == 0xFFFFFFFF: # Get current timeout
                    print(f"IOCTL MP_STREAM_TIMEOUT: Get current timeout. Returning {self.timeout_ms}ms.")
                elif isinstance(arg, int) and arg >= 0:
                    self.timeout_ms = arg
                    print(f"IOCTL MP_STREAM_TIMEOUT: Timeout set to {self.timeout_ms}ms. Was {old_timeout}ms.")
                else:
                    print(f"IOCTL MP_STREAM_TIMEOUT: Invalid argument {arg} for setting timeout.")
                    return -uerrno.EINVAL
                return old_timeout
                
            elif request == MP_STREAM_GET_FILENO:
                print(f"IOCTL MP_STREAM_GET_FILENO: Stream has no OS file descriptor.")
                return -1 

            elif request == MP_STREAM_GET_BUFFER_SIZE:
                # Example: return a preferred block size if this were a block device
                print(f"IOCTL MP_STREAM_GET_BUFFER_SIZE: No specific C-buffer. Returning 0.")
                return 0 

            print(f"IOCTL: Unknown or unsupported request {request}.")
            return -uerrno.EOPNOTSUPP # Operation not supported

    # Example Usage of MyExampleStream with ioctl:
    stream = MyExampleStream(b"Initial stream data for testing.")
    temp_buf = bytearray(10)

    # 1. Poll for reading and writing
    print("\n--- Testing MP_STREAM_POLL ---")
    events_to_poll = MP_STREAM_POLL_RD | MP_STREAM_POLL_WR
    ready_events = stream.ioctl(MP_STREAM_POLL, events_to_poll)
    if ready_events & MP_STREAM_POLL_RD:
        print("Stream is ready for reading.")
        stream.readinto(temp_buf)
        print(f"Read after poll: {temp_buf[:stream.pos]}")
    if ready_events & MP_STREAM_POLL_WR:
        print("Stream is ready for writing.")
        stream.write(b"data") # write is a no-op data wise but prints

    # 2. Seek using ioctl
    print("\n--- Testing MP_STREAM_SEEK ---")
    # Seek to position 8 from the start
    ret = stream.ioctl(MP_STREAM_SEEK, (8, 0)) 
    if ret == 0: print(f"Seek successful, new pos: {stream.pos}")
    else: print(f"Seek failed, error: {ret}")
    stream.readinto(temp_buf)
    print(f"Read after seek: {temp_buf[:5]}") # Read a few bytes

    # 3. Get/Set Timeout
    print("\n--- Testing MP_STREAM_TIMEOUT ---")
    initial_timeout = stream.ioctl(MP_STREAM_TIMEOUT, -1) # Get current
    print(f"Initial stream timeout: {initial_timeout} ms")
    stream.ioctl(MP_STREAM_TIMEOUT, 2000) # Set timeout to 2000ms
    current_timeout = stream.ioctl(MP_STREAM_TIMEOUT, 0xFFFFFFFF) # Get current again
    print(f"Updated stream timeout: {current_timeout} ms")
    stream.ioctl(MP_STREAM_TIMEOUT, 0) # Set to non-blocking
    print(f"Timeout after setting non-blocking: {stream.ioctl(MP_STREAM_TIMEOUT, -1)} ms")


    # 4. Flush (no-op for this example)
    print("\n--- Testing MP_STREAM_FLUSH ---")
    stream.ioctl(MP_STREAM_FLUSH, 0) # arg is usually 0 or ignored

    # 5. Get Fileno (returns -1 for this non-OS stream)
    print("\n--- Testing MP_STREAM_GET_FILENO ---")
    fd = stream.ioctl(MP_STREAM_GET_FILENO, 0) # arg is usually 0 or ignored
    print(f"File descriptor: {fd}")

    # 6. Get Buffer Size (returns 0 for this example)
    print("\n--- Testing MP_STREAM_GET_BUFFER_SIZE ---")
    buf_size = stream.ioctl(MP_STREAM_GET_BUFFER_SIZE, 0) # arg is usually 0 or ignored
    print(f"Preferred buffer size: {buf_size}")
    
    # 7. Close stream via ioctl and test polling HUP
    print("\n--- Testing MP_STREAM_CLOSE and POLL HUP ---")
    stream.ioctl(MP_STREAM_CLOSE, 0) # arg is usually 0 or ignored
    
    hup_poll_mask = MP_STREAM_POLL_RD | MP_STREAM_POLL_HUP
    ready_after_close = stream.ioctl(MP_STREAM_POLL, hup_poll_mask)
    if ready_after_close & MP_STREAM_POLL_HUP:
        print("Stream correctly reports HUP after being closed via ioctl.")
    if ready_after_close & MP_STREAM_POLL_RD: # Should not be ready
        print("Stream unexpectedly ready for read after close!")

    # Try an operation on a closed stream
    try:
        stream.readinto(temp_buf)
    except ValueError as e:
        print(f"Caught expected error on closed stream: {e}")

    print("\nExample finished.")
