from machine import PDM_PCM
import uio
import time


def record_audio_blocking(filename, time_sec):
    rx_buf = bytearray([0] * 128)
    with uio.open(filename, "wb") as f:
        start_time = time.time()
        while time.time() - start_time < time_sec:
            num_read = pdm_pcm.readinto(rx_buf)
            f.write(rx_buf[:num_read])


def record_audio_non_blocking(filename, time_sec):
    global data_ready
    data_ready = False

    def cback_data_ready(obj):
        global data_ready
        data_ready = True

    rx_buf = bytearray([0] * 1024)
    pdm_pcm.irq(cback_data_ready)

    with uio.open(filename, "wb") as f:
        start_time = time.time()
        while time.time() - start_time < time_sec:
            pdm_pcm.readinto(rx_buf)
            while not data_ready:
                pass

            data_ready = False
            f.write(rx_buf)


########################################
#### Audio configuration parameters ####
########################################

pdm_pcm_config = {
    "sample_rate": 16000,
    "bits_per_sample": 16,
    "format": PDM_PCM.STEREO,
    "mode": "blocking",
    "audio_file": "audio.raw",
    "time_sec": 5,
}

try:
    import audio_conf

    pdm_pcm_config = audio_conf.user_config
except ImportError:
    print("audio_conf.py not found. Using default config")

print("########## Audio configuration ##########")
print(f"format:          {pdm_pcm_config['format']}")
print(f"sample_rate:     {pdm_pcm_config['sample_rate']}")
print(f"bits_per_sample: {pdm_pcm_config['bits_per_sample']}")
print(f"mode:            {pdm_pcm_config['mode']}")
print(f"audio_file:      {pdm_pcm_config['audio_file']}")
print(f"time_sec:        {pdm_pcm_config['time_sec']}")
print("########################################")

if pdm_pcm_config["mode"] == "blocking":
    record_audio = record_audio_blocking
else:
    record_audio = record_audio_non_blocking

########################################

print("-- Starting audio recording...")

pdm_pcm = PDM_PCM(
    sck="P8_5",
    data="P8_6",
    sample_rate=pdm_pcm_config["sample_rate"],
    bits=pdm_pcm_config["bits_per_sample"],
    format=pdm_pcm_config["format"],
)
record_audio(pdm_pcm_config["audio_file"], pdm_pcm_config["time_sec"])
pdm_pcm.deinit()

print("-- Audio recorded")
