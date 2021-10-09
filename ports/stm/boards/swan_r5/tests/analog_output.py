import board
import analogio
import digitalio


def test_dac_analog(p_in, p_out):
    print(f"Running dac analog test with pin {p_in} as input and {p_out} as output\n")
    pin_in = analogio.AnalogIn(p_in)
    pin_out = analogio.AnalogOut(p_out)

    for v in range(0, 65536, 4096):
        pin_out.value = v
        print(f"Value {v} read as {pin_in.value}")

    pin_in.deinit()
    pin_out.deinit()


def test_dac_digital(p_in, p_out):
    print(f"Running dac digital test with pin {p_in} as input and {p_out} as output")
    pin_in = digitalio.DigitalInOut(p_in)
    pin_out = analogio.AnalogOut(p_out)

    for v in range(0, 65536, 4096):
        pin_out.value = v
        print(f"Value {v} read as {pin_in.value}")

    pin_in.deinit()
    pin_out.deinit()


def test_dual(pair1, pair2):
    # verifies that the DACs can be set independently
    print(f"Running pair test\n")
    pin1_in = analogio.AnalogIn(pair1[0])
    pin1_out = analogio.AnalogOut(pair1[1])
    pin2_in = analogio.AnalogIn(pair2[0])
    pin2_out = analogio.AnalogOut(pair2[1])

    for v in range(0, 65536, 4096):
        v2 = 65535 - v
        pin1_out.value = v
        pin2_out.value = v2
        print(f"Pair1: Value {v} read as {pin1_in.value}")
        print(f"Pair2: Value {v2} read as {pin2_in.value}")

    pin1_in.deinit()
    pin1_out.deinit()
    pin2_in.deinit()
    pin2_out.deinit()


def test_analog_hilo(p_in, p_out):
    print(f"Running analog hilo test with pin {p_in} as input and {p_out} as output")
    pin_in = analogio.AnalogIn(p_in)
    pin_out = digitalio.DigitalInOut(p_out)
    pin_out.switch_to_output()

    for v in (False, True, False, True):
        pin_out.value = v
        print(f"Value {v} read as {pin_in.value}")

    pin_in.deinit()
    pin_out.deinit()


def test_pair(pair):
    # FIXME: test_analog_hilo works fine alone, but fails when the other dac functions are executed
    test_analog_hilo(*pair)
    test_dac_analog(*pair)
    test_dac_digital(*pair)


def main():
    pair1 = (board.A3, board.DAC1)
    pair2 = (board.A2, board.DAC2)
    print("running DAC1 tests")
    test_pair(pair1)
    print("running DAC2 tests")
    test_pair(pair2)
    print("running dual DAC tests")
    test_dual(pair1, pair2)


main()
