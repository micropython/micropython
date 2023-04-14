from machine import Pin
import utime

class EncoderInput:
    def __init__(self, encoder_pins):
        self.encoder_pins = encoder_pins
        self.prev_state_left = [float('inf')] * len(encoder_pins)
        self.prev_state_right = [float('inf')] * len(encoder_pins)
        self.left_enc_callback = [None] * len(encoder_pins)
        self.right_enc_callback = [None] * len(encoder_pins)

    def begin(self):
        for i in range(len(self.encoder_pins)):
            Pin(self.encoder_pins[i][0], Pin.IN)
            Pin(self.encoder_pins[i][1], Pin.IN)
            Pin(self.encoder_pins[i][2], Pin.IN)
            Pin(self.encoder_pins[i][3], Pin.IN)

    def loop(self):
        for i in range(len(self.encoder_pins)):
            left_val = 0
            right_val = 0

            state_left = Pin(self.encoder_pins[i][0], Pin.IN).value()
            if state_left != self.prev_state_left[i] and self.prev_state_left[i] != float('inf'):
                if Pin(self.encoder_pins[i][1], Pin.IN).value() != state_left:
                    left_val = 1
                else:
                    left_val = -1
            self.prev_state_left[i] = state_left

            state_right = Pin(self.encoder_pins[i][2], Pin.IN).value()
            if state_right != self.prev_state_right[i] and self.prev_state_right[i] != float('inf'):
                if Pin(self.encoder_pins[i][3], Pin.IN).value() != state_right:
                    right_val = 1
                else:
                    right_val = -1
            self.prev_state_right[i] = state_right

            if left_val != 0 and self.left_enc_callback[i]:
                self.left_enc_callback[i](i, left_val)

            if right_val != 0 and self.right_enc_callback[i]:
                self.right_enc_callback[i](i, right_val)

    def set_left_enc_callback(self, index, left_enc_listener):
        self.left_enc_callback[index] = left_enc_listener

    def set_right_enc_callback(self, index, right_enc_listener):
        self.right_enc_callback[index] = right_enc_listener
