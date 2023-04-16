from machine import Pin
import utime
import uasyncio


class EncoderInput:
    def __init__(self, encoder_pins):
        self.encoder_pins = encoder_pins
        self.prev_state = [float('inf')] * len(encoder_pins)
        self.enc_callback = [None] * len(encoder_pins)
        for i in range(len(self.encoder_pins)):
            Pin(self.encoder_pins[i][0], Pin.IN)
            Pin(self.encoder_pins[i][1], Pin.IN)

    async def loop(self):
        while (True):
            await uasyncio.sleep_ms(0)
            for i in range(len(self.encoder_pins)):
                val = 0

                state_left = Pin(self.encoder_pins[i][0], Pin.IN).value()
                if state_left != self.prev_state[i] and self.prev_state[i] != float('inf'):
                    if Pin(self.encoder_pins[i][1], Pin.IN).value() != state_left:
                        val = 1
                    else:
                        val = -1
                self.prev_state[i] = state_left

                if val != 0 and self.enc_callback[i] is not None:
                    self.enc_callback[i](i, val)

    def set_enc_callback(self, index, enc_listener):
        print("index: " + str(index) + ", len: " + str(len(self.enc_callback)))
        self.enc_callback[index] = enc_listener
