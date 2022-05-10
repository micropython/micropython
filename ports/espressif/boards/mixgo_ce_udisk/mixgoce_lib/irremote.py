"""
IR Remote

CircuitPython library for IR Remote - MixGoCE
=======================================================

Small Cabbage
20210811
"""

import pulseio
import board
import adafruit_irremote

first_init =True

def encode2hex(code):
    res = "0X"
    for c in code:
        c = hex(c)[2:]
        if len(c) == 1:
            c = "0" + c
        res = res + c.upper()
    return res

def decode2list(string):
    res = [0, 0, 0, 0]
    prefix = "0" * (8 - len(string[2:]))
    string = prefix + string[2:]
    for i in range(4):
        res[i] = int(string[2*i:2*(i+1)], 16)
    return res

def ir_receive_hex(pin=board.IO38, flag=False):
    global first_init
    global decoder
    global pulsein

    if first_init :
        pulsein = pulseio.PulseIn(pin, maxlen=120, idle_state=True)
        decoder = adafruit_irremote.GenericDecode()
        first_init=False

    pulses = decoder.read_pulses(pulsein, blocking=False)
    if pulses:
        try:
            # print("Heard", len(pulses), "Pulses:", pulses)
            code = decoder.decode_bits(pulses)
            res = encode2hex(code)
            if flag:
                print("Decoded:", res)
            # pulsein.deinit()
            return res
        except adafruit_irremote.IRNECRepeatException:  # unusual short code!
            print("NEC repeat!")
        except adafruit_irremote.IRDecodeException as e:  # failed to decode
            if flag:
                print("Failed to decode: ", e.args)
        # except TypeError as e:
        #     pass
        # finally:
        #     pulsein.deinit()

def ir_receive(pin=board.IO38,flag=False):
    global first_init
    global decoder
    global pulsein

    if first_init :
        pulsein = pulseio.PulseIn(pin, maxlen=120, idle_state=True)
        decoder = adafruit_irremote.GenericDecode()
        first_init=False

    pulses = decoder.read_pulses(pulsein, blocking=False)
    if pulses:
        try:
            code = decoder.decode_bits(pulses)
            if flag:
                print("Decoded:", code)
            return code
        except adafruit_irremote.IRNECRepeatException:  # unusual short code!
            print("NEC repeat!")
        except adafruit_irremote.IRDecodeException as e:  # failed to decode
            if flag:
                print("Failed to decode: ", e.args)

def ir_send(content=[255, 0, 0, 0], pin=board.IO39):
    pulseout = pulseio.PulseOut(pin, frequency=38000, duty_cycle=2 ** 15)
    # Create an encoder that will take numbers and turn them into NEC IR pulses
    encoder = adafruit_irremote.GenericTransmit(
        header=[9500, 4500], one=[550, 550], zero=[550, 1700], trail=550
    )
    encoder.transmit(pulseout, content)
    print("IR signal sent!")
    pulseout.deinit()

def ir_send_hex(content="0XFF000000", pin=board.IO39): # [255, 0, 0, 0]
    pulseout = pulseio.PulseOut(pin, frequency=38000, duty_cycle=2 ** 15)
    # Create an encoder that will take numbers and turn them into NEC IR pulses
    encoder = adafruit_irremote.GenericTransmit(
        header=[9500, 4500], one=[550, 550], zero=[550, 1700], trail=550
    )
    encoder.transmit(pulseout, decode2list(content))
    print("IR signal sent!")
    pulseout.deinit()
	


	
	
	