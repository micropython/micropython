"""Test for nrf24l01 module."""

import ustruct
import time
from machine import Pin, SPI
from nrf24l01 import NRF24L01

pipes = (b'\xf0\xf0\xf0\xf0\xe1', b'\xf0\xf0\xf0\xf0\xd2')

def master():
    # init the SPI bus and pins
    spi = SPI(1, baudrate=4000000, polarity=0, phase=0)
    cs = Pin(15, mode=Pin.OUT, pull=None)
    ce = Pin(2, mode=Pin.OUT, pull=None)

    nrf = NRF24L01(spi, cs, ce, payload_size=8)

    nrf.open_tx_pipe(pipes[0])
    nrf.open_rx_pipe(1, pipes[1])
    nrf.start_listening()

    num_needed = 16
    num_successes = 0
    num_failures = 0
    payload = 0

    print('NRF24L01 master mode, sending %d packets...' % num_needed)

    while num_successes < num_needed and num_failures < num_needed:
        # stop listening and send packet
        nrf.stop_listening()
        millis = time.ticks_ms()
        payload = max(1, (payload << 1) & 0x0f)
        print('sending:', millis, payload)
        try:
            nrf.send(ustruct.pack('ii', millis, payload))
        except OSError:
            pass

        # start listening again
        nrf.start_listening()

        # wait for response, with 250ms timeout
        start_time = time.ticks_ms()
        timeout = False
        while not nrf.any() and not timeout:
            if time.ticks_diff(time.ticks_ms(), start_time) > 250:
                timeout = True

        if timeout:
            print('failed, respones timed out')
            num_failures += 1

        else:
            # recv packet
            got_millis, = ustruct.unpack('i', nrf.recv())

            # print response and round-trip delay
            print('got response:', got_millis, '(delay', time.ticks_ms() - got_millis, 'ms)')
            num_successes += 1

        # delay then loop
        time.sleep_ms(250)

    print('master finished sending; succeses=%d, failures=%d' % (num_successes, num_failures))

def slave():
    # init the SPI bus and pins
    spi = SPI(1, baudrate=4000000, polarity=0, phase=0)
    cs = Pin(15, mode=Pin.OUT, pull=None)
    ce = Pin(2, mode=Pin.OUT, pull=None)

    nrf = NRF24L01(spi, cs, ce, payload_size=8)

    nrf.open_tx_pipe(pipes[1])
    nrf.open_rx_pipe(1, pipes[0])
    nrf.start_listening()

    print('NRF24L01 slave mode, waiting for packets... (ctrl-C to stop)')

    while True:
        #pyb.wfi()
        # not sure if there is an ESP8266 equivalent of pyb.wfi(). Deep sleep?
        if nrf.any():
            while nrf.any():
                buf = nrf.recv()
                millis, payload = ustruct.unpack('ii', buf)
                print('received:', millis, payload)
                time.sleep_ms(15)

            nrf.stop_listening()
            try:
                nrf.send(ustruct.pack('i', millis))
            except OSError:
                pass
            print('sent response')
            nrf.start_listening()

print('NRF24L01 test module loaded')
print('NRF24L01 pinout for test:')
print('    CE on GPIO2')
print('    CSN on GPIO15')
print('    SCK on GPIO14')
print('    MISO on GPIO12')
print('    MOSI on GPIO13')
print('run nrf24l01test.slave() on slave, then nrf24l01test.master() on master')
