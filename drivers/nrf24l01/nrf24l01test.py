"""Test for nrf24l01 module."""

import struct
import utime
from machine import Pin, SPI
from nrf24l01 import NRF24L01

pipes = (b'\xf0\xf0\xf0\xf0\xe1', b'\xf0\xf0\xf0\xf0\xd2')

def master():
    nrf = NRF24L01(SPI(2), Pin('Y5'), Pin('Y4'), payload_size=8)

    nrf.open_tx_pipe(pipes[0])
    nrf.open_rx_pipe(1, pipes[1])
    nrf.start_listening()

    num_needed = 16
    num_successes = 0
    num_failures = 0
    led_state = 0

    print('NRF24L01 master mode, sending %d packets...' % num_needed)

    while num_successes < num_needed and num_failures < num_needed:
        # stop listening and send packet
        nrf.stop_listening()
        millis = utime.ticks_ms()
        led_state = max(1, (led_state << 1) & 0x0f)
        print('sending:', millis, led_state)
        try:
            nrf.send(struct.pack('ii', millis, led_state))
        except OSError:
            pass

        # start listening again
        nrf.start_listening()

        # wait for response, with 250ms timeout
        start_time = utime.ticks_ms()
        timeout = False
        while not nrf.any() and not timeout:
            if utime.ticks_diff(utime.ticks_ms(), start_time) > 250:
                timeout = True

        if timeout:
            print('failed, response timed out')
            num_failures += 1

        else:
            # recv packet
            got_millis, = struct.unpack('i', nrf.recv())

            # print response and round-trip delay
            print('got response:', got_millis, '(delay', utime.ticks_diff(utime.ticks_ms(), got_millis), 'ms)')
            num_successes += 1

        # delay then loop
        utime.sleep_ms(250)

    print('master finished sending; successes=%d, failures=%d' % (num_successes, num_failures))

def slave():
    nrf = NRF24L01(SPI(2), Pin('Y5'), Pin('Y4'), payload_size=8)

    nrf.open_tx_pipe(pipes[1])
    nrf.open_rx_pipe(1, pipes[0])
    nrf.start_listening()

    print('NRF24L01 slave mode, waiting for packets... (ctrl-C to stop)')

    while True:
        machine.idle()
        if nrf.any():
            while nrf.any():
                buf = nrf.recv()
                millis, led_state = struct.unpack('ii', buf)
                print('received:', millis, led_state)
                for led in leds:
                    if led_state & 1:
                        led.on()
                    else:
                        led.off()
                    led_state >>= 1
                utime.sleep_ms(15)

            nrf.stop_listening()
            try:
                nrf.send(struct.pack('i', millis))
            except OSError:
                pass
            print('sent response')
            nrf.start_listening()

try:
    import pyb
    leds = [pyb.LED(i + 1) for i in range(4)]
except:
    leds = []

print('NRF24L01 test module loaded')
print('NRF24L01 pinout for test:')
print('    CE on Y4')
print('    CSN on Y5')
print('    SCK on Y6')
print('    MISO on Y7')
print('    MOSI on Y8')
print('run nrf24l01test.slave() on slave, then nrf24l01test.master() on master')
