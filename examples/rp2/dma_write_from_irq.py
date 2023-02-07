import uasyncio
import time
from rp2 import DMA, Timer
from machine import Pin, PWM


async def led():
    print("DMA write_from demo")
    led = Pin(16, Pin.OUT)
    pwm1 = PWM(led)
    pwm1.freq(1000)
    pwm1.duty_u16(0)

    # pwm 0A attached to GPIO 16
    dest = 0x4005000E
    dma = DMA()
    dma.claim()
    t = Timer()

    # slow the timer to it's slowest rate
    X = 0x0001
    Y = 0xFFFF
    t.claim()
    t.set(X, Y)

    print(f"Starting\nDMA:{dma}\nTimer:{t}")

    dreq = t.dreq()

    data_size = dma.DMA_SIZE_16
    buffer_size = 32768
    from_buffer = bytearray(buffer_size)

    # create a 16 bit fade
    for i in range(0, buffer_size, 2):
        from_buffer[i]= i & 0xFF
        from_buffer[i+1]= (i>>8) & 0xFF

    start = time.ticks_ms()

    e = uasyncio.ThreadSafeFlag()

    def _callback(dma_status):
        print("callback")
        e.set()

    dma.write_from(from_buffer=from_buffer,
        to_address=dest,
        transfer_count=buffer_size>>data_size,
        dreq=dreq,
        data_size=data_size,
        handler=_callback
    )

    await e.wait()

    end = time.ticks_ms()

    t.unclaim()
    dma.unclaim()
    print(f"Write from fade took {end-start}mS")
    print("Done")
    pwm1.duty_u16(0)


async def async_led():
    t = uasyncio.create_task(led())
    await t


def main():
    uasyncio.run(async_led())


if __name__ == '__main__':
    main()
