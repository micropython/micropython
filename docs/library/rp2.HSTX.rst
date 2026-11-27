.. currentmodule:: rp2
.. _rp2.HSTX:

class HSTX -- access to the RP2350's HSTX controller
====================================================

The :class:`HSTX` class offers access to the RP2350's High-speed Serial Transmit (HSTX) peripheral, which can output data through up to 8 GPIO pins (only GPIO 12 through 19) at up to 150MHz, and is double-data rate (DDR) capable. This enables the HSTX to theoretically stream data up to 2.4Gbps, provided its FIFO buffer can be kept filled (it's typically necessary to use the DMA controller to achieve this). The HSTX also features a command expander, which can manipulate the data stream (the RP2350 currently only implements a TMDS encoder) and repeat data. For full details of the RP2350's HSTX system, see section 12.11 of the `RP2350 Datasheet <https://datasheets.raspberrypi.org/rp2350/rp2350-datasheet.pdf>`_.

Examples
--------

A simple use of the HSTX peripheral is to blink a pair of LEDs. This can be accomplished with the following complete example::

    import rp2
    import time
    from machine import Pin

    # Get the singleton HSTX object.
    hstx = rp2.HSTX()

    # Set GPIO pins 12 and 13 to the HSTX alternate function.
    pin_12 = Pin(12, mode=Pin.ALT, alt=Pin.ALT_HSTX)
    pin_13 = Pin(13, mode=Pin.ALT, alt=Pin.ALT_HSTX)

    # Configure pin 12 to output bit 0 of data in the HSTX FIFO for both the first
    # and second (positive and negative) phases of the HSTX clock cycle, and pin 13
    # to output the same but inverted value.
    hstx.bit(pin_12, hstx.pack_bit(sel_p=0, sel_n=0))
    hstx.bit(pin_13, hstx.pack_bit(sel_p=0, sel_n=0, inv=1))

    # Set up the CSR register to enable HSTX with no shifting.
    csr = hstx.pack_csr(
        shift = 0,
        enable = 1
    )
    hstx.csr(csr)

    # Data can be manually pushed into the FIFO, though a DMA channel should be used
    # for real applications.
    while True:
        hstx.fifo_put(1)
        time.sleep(1)
        hstx.fifo_put(0)
        time.sleep(1)

It's usually more practical to use a DMA channel to feed data to the HSTX FIFO. To support this, the HSTX class supports the buffer protocol (write-only), and can be used like so::

    import rp2
    import array

    # Get the singleton HSTX object. Configuration of the HSTX is omitted from this
    # example for brevity.
    hstx = rp2.HSTX()

    # Data to send to the HSTX FIFO.
    data = array.array("I", [0x01234567, 0x89ABCDEF])

    # Data request (DREQ) signal number for the HSTX FIFO.
    DREQ_HSTX = 52

    # Set up a DMA channel to feed data to the HSTX FIFO.
    dma = rp2.DMA()
    dma_ctrl = dma.pack_ctrl(
        inc_write = False, # Keep writing to the same HSTX FIFO address.
        dreq_sel = DREQ_HSTX # Pace DMA transfers by the HSTX data request signal.
    )
    dma.config(
        read = data,
        write = hstx,
        count = len(data),
        ctrl = dma_ctrl,
        trigger = True
    )

The command expander can be used with the TMDS encoder for driving DVI displays (HDMI displays also support this signal). The example below demonstrates the HSTX configuration, but is not a complete implementation. Special timing signals are required in addition to needing a complete frame buffer, so those are omitted here for simplicity::

    import rp2
    from machine import Pin

    # Define the GPIO pins used for each TMDS lane.
    pin_clk_p = 14
    pin_clk_n = 15
    pin_d0_p = 18
    pin_d0_n = 19
    pin_d1_p = 16
    pin_d1_n = 17
    pin_d2_p = 12
    pin_d2_n = 13

    # Get the singleton HSTX object.
    hstx = rp2.HSTX()

    # Configure the command expander's shift register. This simple example does not
    # actually perform any shifting, but it's possible to pack multiple pixels per
    # 32-bit data word, then shift the words to align the bits for the TMDS encoder
    # by changing the `enc_n_shifts` and `enc_shift` fields.
    expand_shift = hstx.pack_expand_shift(
        enc_n_shifts = 1,
        enc_shift = 0,
        raw_n_shifts = 1,
        raw_shift = 0
    )
    hstx.expand_shift(expand_shift)

    # Configure the TMDS encoder to convert the data in the command expander's shift
    # register to TMDS format. This example demonstrates RGB565 pixel format, which
    # is assumed to be packed into the 32-bit data words as follows:
    # 
    # (xxxxxxxx xxxxxxxx RRRRRGGG GGGBBBBB)
    # 
    # There are 3 TMDS data lanes for DVI video (lane 0 carries the blue channel,
    # lane 1 carries green, and lane 2 carries red). For each lane, the bits for
    # that channel need to start at bit 7 (8th bit position), so each lane needs to
    # shift the register to get the MSB aligned using the `ln_rot` field (right
    # rotate, wraps around). Up to 8 bits can be used per channel, specified by the
    # `ln_nbits` field (values of 0-7 correspond to 1-8 bits).
    expand_tmds = hstx.pack_expand_tmds(
        l2_nbits =  4, # 5 bits (red)
        l2_rot   =  8, # Shift right 8 bits to align MSB
        l1_nbits =  5, # 6 bits (green)
        l1_rot   =  3, # Shift right 3 bits to align MSB
        l0_nbits =  4, # 5 bits (blue)
        l0_rot   = 29, # Shift right 29 bits to align MSB (left 3 bits)
    )
    hstx.expand_tmds(expand_tmds)

    # The TMDS encoder sends 30 bits of data per pixel (10 bits per lane) to the
    # 32-bit output shift register. The bits for lane 0, 1, and 2 are as follows:
    # 
    # (xx 2222222222 1111111111 0000000000)
    # 
    # The output shift register will be configured to shift the whole register by 2
    # bits every system clock cycle (DDR) for 5 cycles (10 bits total). So we'll
    # configure each lane's pins to output bits 10*n and 10*n+1 on the positive and
    # negative phases of the system clock, respectively.
    # 
    # The clock lane outputs a continuous clock signal, so those GPIO pins just
    # follow the HSTX clock generator, which will be configured below.
    # 
    # Each pair of pins is differential, so the `_n` pin is just the inverted value
    # of the `_p` pin.
    hstx.bit(pin_clk_p, hstx.pack_bit(clk=1,              inv=0))
    hstx.bit(pin_clk_n, hstx.pack_bit(clk=1,              inv=1))
    hstx.bit(pin_d0_p,  hstx.pack_bit(sel_p= 0, sel_n= 1, inv=0))
    hstx.bit(pin_d0_n,  hstx.pack_bit(sel_p= 0, sel_n= 1, inv=1))
    hstx.bit(pin_d1_p,  hstx.pack_bit(sel_p=10, sel_n=11, inv=0))
    hstx.bit(pin_d1_n,  hstx.pack_bit(sel_p=10, sel_n=11, inv=1))
    hstx.bit(pin_d2_p,  hstx.pack_bit(sel_p=20, sel_n=21, inv=0))
    hstx.bit(pin_d2_n,  hstx.pack_bit(sel_p=20, sel_n=21, inv=1))

    # Set all HSTX pins (GPIO 12-19) to ALT function for HSTX output.
    for i in range(12, 20):
        Pin(i, mode=Pin.ALT, alt=Pin.ALT_HSTX)

    # DVI requires the clock lane to cycle once per 10 data bits, meaning every 5
    # system clock cycles (DDR).
    # 
    # The output shift register needs to shift by 2 bits every system clock cycle
    # for 5 cycles, so `shift` and `n_shifts` are set to 2 and 5, respectively.
    # 
    # Then we enable the command expander to activate the TMDS encoder, and enable
    # the HSTX peripheral.
    csr = hstx.pack_csr(
        clk_div = 5,
        n_shifts = 5,
        shift = 2,
        expand_enable = 1,
        enable = 1
    )
    hstx.csr(csr)

Constructor
-----------

.. class:: HSTX()

    Gets the singleton object for accessing the HSTX peripheral.

Methods
-------

.. method:: HSTX.csr([value])

    Gets or sets the HSTX control and status register (CSR) *value*. This is an integer value that is typically packed using :meth:`HSTX.pack_csr()`.

.. method:: HSTX.bit(pin, [value])

    Gets or sets the bit register *value* for the given *pin* (only GPIO 12 to 19, can be a `machine.Pin()` or integer). This is an integer value that is typically packed using :meth:`HSTX.pack_bit()`.
  
.. method:: HSTX.expand_shift([value])
  
    Gets or sets the command expander's shift register *value*. This is an integer value that is typically packed using :meth:`HSTX.pack_expand_shift()`.

.. method:: HSTX.expand_tmds([value])
  
    Gets or sets the command expander's TMDS encoder register *value*. This is an integer value that is typically packed using :meth:`HSTX.pack_expand_tmds()`.

.. method:: HSTX.pack_csr(**kwargs)

    Pack the values provided in the keyword arguments into the named fields of a new control register value. Any field that is not provided will be set to a default value.

    The keys for the keyword arguments can be any key returned by :meth:`HSTX.unpack_csr()`. The writable values are:

    - *enable*: ``bool`` Enable the HSTX peripheral (default: ``False``).

    - *expand_enable*: ``bool`` Enable the command expander (default: ``False``).

    - *coupled_mode*: ``bool`` Enable the PIO-to_HSTX connection (default: ``False``).

    - *coupled_select*: ``int`` Select which PIO instance to couple to (0-2, default: ``0``).

    - *shift*: ``int`` Number of bits to right-rotate the output shift register by each cycle (0-31, default: ``6``).

    - *n_shifts*: ``int`` Number of shifts to perform before reloading the shift register (0-31, with 0 meaning shift 32 times, default: ``5``).

    - *clk_phase*: ``int`` Initial phase of the generated HSTX clock (0-15, default: ``0``).

    - *clk_div*: ``int`` Clock perid of the generated HSTX clock, in system clock cycles (0-15, with 0 meaning *clk_div* of 16, default: ``1``).

.. method:: HSTX.pack_bit(**kwargs)

    Pack the values provided in the keyword arguments into the named fields of a new control register value. Any field that is not provided will be set to a default value.

    The keys for the keyword arguments can be any key returned by :meth:`HSTX.unpack_bit()`. The writable values are:

    - *sel_p*: ``int`` Data bit to output for the first half (positive phase) of the HSTX clock cycle (0-31, default: ``0``).

    - *sel_n*: ``int`` Data bit to output for the second half (negative phase) of the HSTX clock cycle (0-31, default: ``0``).

    - *inv*: ``bool`` Invert the output value (default: ``False``).

    - *clk*: ``bool`` Output the generated clock instead of a data bit (default: ``False``).

.. method:: HSTX.pack_expand_shift(**kwargs)

    Pack the values provided in the keyword arguments into the named fields of a new control register value. Any field that is not provided will be set to a default value.

    The keys for the keyword arguments can be any key returned by :meth:`HSTX.unpack_expand_shift()`. The writable values are:

    - *raw_shift*: ``int`` Number of bits to right-rotate the command expander's shift register by each shift when using the raw data command (0-31, default: ``0``).

    - *raw_n_shifts*: ``int`` Number of shifts to perform when using the raw data command before reloading the shift register (0-31, with 0 meaning shift 32 times, default: ``1``).

    - *enc_shift*: ``int`` Number of bits to right-rotate the command expander's shift register by each shift when using an encoder (eg. TMDS) command (0-31, default: ``0``).

    - *enc_n_shifts*: ``int`` Number of shifts to perform when using an encoder (eg. TMDS) command before reloading the shift register (0-31, with 0 meaning shift 32 times, default: ``1``).

.. method:: HSTX.pack_expand_tmds(**kwargs)

    Pack the values provided in the keyword arguments into the named fields of a new control register value. Any field that is not provided will be set to a default value.

    The keys for the keyword arguments can be any key returned by :meth:`HSTX.unpack_expand_tmds()`. The writable values are:

    - *l0_rot*: ``int`` Number of bits to right-rotate the command expander's shift register to align the MSB for lane 0 (0-31, default: ``0``).

    - *l0_nbits*: ``int`` Number of bits to use for lane 0 (0-7, corresponding to 1-8 bits, default: ``0``).

    - *l1_rot*: ``int`` Number of bits to right-rotate the command expander's shift register to align the MSB for lane 1 (0-31, default: ``0``).

    - *l1_nbits*: ``int`` Number of bits to use for lane 1 (0-7, corresponding to 1-8 bits, default: ``0``).

    - *l2_rot*: ``int`` Number of bits to right-rotate the command expander's shift register to align the MSB for lane 2 (0-31, default: ``0``).

    - *l2_nbits*: ``int`` Number of bits to use for lane 2 (0-7, corresponding to 1-8 bits, default: ``0``).

.. method:: HSTX.unpack_csr(value)

    Unpack the control and status register (CSR) *value* into a dictionary with key/value pairs for each of the fields in the register. This method will return values for all the keys that can be passed to ``HSTX.pack_csr``.

.. method:: HSTX.unpack_bit(value)

    Unpack the bit register *value* for a given pin into a dictionary with key/value pairs for each of the fields in the register. This method will return values for all the keys that can be passed to ``HSTX.pack_bit()``.

.. method:: HSTX.unpack_expand_shift(value)
  
    Unpack the command expander's shift register *value* into a dictionary with key/value pairs for each of the fields in the register. This method will return values for all the keys that can be passed to ``HSTX.pack_expand_shift()``.

.. method:: HSTX.unpack_expand_tmds(value)

    Unpack the command expander's TMDS encoder register *value* into a dictionary with key/value pairs for each of the fields in the register. This method will return values for all the keys that can be passed to ``HSTX.pack_expand_tmds()``.

.. method:: HSTX.fifo_put(value)

    Push a word into the HSTX FIFO.

.. method:: HSTX.fifo_level()

    Get the current number of words in the HSTX FIFO.

.. method:: HSTX.fifo_wof([value])

    Get or clear the HSTX FIFO write overflow flag. If *value* is provided, the flag is cleared regardless of the value.

Buffer protocol
---------------

The HSTX class supports the `buffer protocol`, allowing direct access to the HSTX FIFO, which is write-only. This is primarily in order to allow the HSTX object to be passed directly as the write parameter when configuring a `rp2.DMA()` channel.
