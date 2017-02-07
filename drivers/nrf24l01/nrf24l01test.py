"""Test for nrf24l01 module."""

import struct
import pyb
from pyb import Pin, SPI
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
        millis = pyb.millis()
        led_state = max(1, (led_state << 1) & 0x0f)
        print('sending:', millis, led_state)
        try:
            nrf.send(struct.pack('ii', millis, led_state))
        except OSError:
            pass

        # start listening again
        nrf.start_listening()

        # wait for response, with 250ms timeout
        start_time = pyb.millis()
        timeout = False
        while not nrf.any() and not timeout:
            if pyb.elapsed_millis(start_time) > 250:
                timeout = True

        if timeout:
            print('failed, response timed out')
            num_failures += 1

        else:
            # recv packet
            got_millis, = struct.unpack('i', nrf.recv())

            # print response and round-trip delay
            print('got response:', got_millis, '(delay', pyb.millis() - got_millis, 'ms)')
            num_successes += 1

        # delay then loop
        pyb.delay(250)

    print('master finished sending; successes=%d, failures=%d' % (num_successes, num_failures))

def slave():
    nrf = NRF24L01(SPI(2), Pin('Y5'), Pin('Y4'), payload_size=8)

    nrf.open_tx_pipe(pipes[1])
    nrf.open_rx_pipe(1, pipes[0])
    nrf.start_listening()

    print('NRF24L01 slave mode, waiting for packets... (ctrl-C to stop)')

    while True:
        pyb.wfi()
        if nrf.any():
            while nrf.any():
                buf = nrf.recv()
                millis, led_state = struct.unpack('ii', buf)
                print('received:', millis, led_state)
                for i in range(4):
                    if led_state & (1 << i):
                        pyb.LED(i + 1).on()
                    else:
                        pyb.LED(i + 1).off()
                pyb.delay(15)

            nrf.stop_listening()
            try:
                nrf.send(struct.pack('i', millis))
            except OSError:
                pass
            print('sent response')
            nrf.start_listening()

print('NRF24L01 test module loaded')
print('NRF24L01 pinout for test:')
print('    CE on Y4')
print('    CSN on Y5')
print('    SCK on Y6')
print('    MISO on Y7')
print('    MOSI on Y8')
print('run nrf24l01test.slave() on slave, then nrf24l01test.master() on master')
