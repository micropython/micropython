import os
import machine
from machine import PDM_PCM, Pin
import time

# Allocate pin based on board
board = os.uname().machine
if "CY8CPROTO-062-4343W with PSoC62" in board:
    clk_pin = "P10_4"
    data_pin = "P10_5"
    send_signal_to_tx_pin = "P10_3"
elif "CY8CPROTO-063-BLE" in board:
    print("SKIP")
    raise SystemExit
elif "CY8CKIT-062S2-AI" in board:
    print("SKIP")
    raise SystemExit

print("*** PDM_PCM tests - RX ***")

send_signal = Pin(send_signal_to_tx_pin, mode=Pin.OUT, pull=Pin.PULL_DOWN, value=False)


def generate_exp_seq(data):
    exp_seq = bytearray(data * 64)
    return exp_seq


print("1. blocking read implementation ")

_sampling_rate = [8000, 16000, 32000, 48000, 22050, 44100]
_mode = [PDM_PCM.MONO_LEFT, PDM_PCM.MONO_RIGHT, PDM_PCM.STEREO]
_bits = 16
exp_data = [[0x00], [0xFF]]
iterations = 100
rounds = 2

for m in range(rounds):
    send_signal.value(0)
    exp_seq = generate_exp_seq(exp_data[m])
    if m == 0:
        print("*** Test for data high ***")
    else:
        print("*** Test for data low ***")

    for i in range(len(_sampling_rate)):  # Check for all sampling rates
        set_sampling_rate = _sampling_rate[i]
        if _sampling_rate[i] == 22050 or _sampling_rate[i] == 44100:
            machine.freq(machine.AUDIO_PDM_22_579_000_HZ)
        else:
            machine.freq(machine.AUDIO_PDM_24_576_000_HZ)
        for j in range(len(_mode)):  # Check for all modes
            set_mode = _mode[j]
            if set_mode == PDM_PCM.STEREO:
                iterations = 200
            pdm_pcm = PDM_PCM(
                0,
                sck=clk_pin,
                data=data_pin,
                sample_rate=set_sampling_rate,
                decimation_rate=64,
                bits=PDM_PCM.BITS_16,
                format=set_mode,
                left_gain=0,
                right_gain=0,
            )
            pdm_pcm.init()  # Start

            for k in range(iterations):
                rx_buf = bytearray([1] * 64)
                num_read = pdm_pcm.readinto(rx_buf)
                # print("buf :", ''.join(f'{x:02x} ' for x in rx_buf))
                if rx_buf[:3] == bytearray([exp_data[m][0], exp_data[m][0], exp_data[m][0]]):
                    is_seq_received = rx_buf == exp_seq
                    if is_seq_received:
                        print(
                            f"data received for mode = {set_mode}, bits = PDM_PCM.BITS_16, rate = {set_sampling_rate} : {is_seq_received}"
                        )
                        break
            pdm_pcm.deinit()
    send_signal.high()
    time.sleep(1)  # Wait to sync

send_signal.deinit()

###############################################################################
print("\n2. irq non-blocking read implementation ")

rx_done = False


def rx_complete_irq(obj):
    global rx_done
    rx_done = True


machine.freq(machine.AUDIO_PDM_22_579_000_HZ)
pdm_pcm = PDM_PCM(
    0,
    sck=clk_pin,
    data=data_pin,
    sample_rate=set_sampling_rate,
    decimation_rate=64,
    bits=PDM_PCM.BITS_16,
    format=set_mode,
    left_gain=0,
    right_gain=0,
)

pdm_pcm.init()  # Start

rx_buf = bytearray([0] * 64)
pdm_pcm.irq(rx_complete_irq)
num_read = pdm_pcm.readinto(rx_buf)

while not rx_done:
    pass

# if we get pass this rx_done flag has been
# modified by the interrupt

print("rx blocking done")
pdm_pcm.deinit()
