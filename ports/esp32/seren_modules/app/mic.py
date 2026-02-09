import uasyncio as asyncio
from machine import Pin, I2S
import struct
from collections import deque
from math import sqrt

class AsyncMicReader:
    """Asynchronous microphone reader using I2S interface for real-time audio processing"""
    
    def __init__(self, nvs, led=None, espnow=None):
        """Initialize I2S microphone interface with settings from NVS"""
        self.nvs = nvs

        self.pin_defaults = {
            'mic_sck_pin': 9,
            'mic_ws_pin': 8,
            'mic_sd_pin': 7,
        }

        self._load_pins()

    





        self.audio_in = I2S(
            0,
            sck=Pin(self.nvs.settings["mic_sck_pin"]),
            ws=Pin(self.nvs.settings["mic_ws_pin"]),
            sd=Pin(self.nvs.settings["mic_sd_pin"]),
            mode=I2S.RX,
            bits=self.nvs.settings["mic_bit_depth"],
            format=I2S.STEREO,
            rate=self.nvs.settings["mic_sample_rate"],
            ibuf=self.nvs.settings["mic_buffer"]
        )
        print("Initialized with I2S microphone")

        
        self.is_monitoring = False
        self.sound_task = None
        self.samples_buffer = None
        self.level_callback = None

        self.max_val = 10
        self.max_val_ideal = self.max_val
        self.min_val = 1
        self.interval_ms = nvs.settings.get("mic_interval_ms", 50)  # Updated key
        self.max_normalization = nvs.settings.get("mic_norm", 0.7)  # Updated key
        self.decay_interval = nvs.settings.get("mic_decay_ms", 500)  # Updated key

        intial_samples = (1000,)  # Initialize with a default value
        self.number_of_samples = 200
        self.samples_memory = bytearray(self.number_of_samples * 4)  # 32-bit = 4 bytes per sample
        self.memory_deque = deque(intial_samples, 100)  # Store last 100 RMS values

        self.led = led
        self.espnow = espnow



    def _load_pins(self):
         # Ensure pins are set in NVS
        new_settings = False
        for key, default_pin in self.pin_defaults.items():
            try:
                self.nvs.settings[key] = self.nvs._pins.get_i32(key)
            except OSError:
                self.nvs.settings[key] = default_pin
                self.nvs._pins.set_i32(key, default_pin)
                new_settings = True
        if new_settings:
            self.nvs._pins.commit()
    
    
    def read_samples(self):
        """Read audio samples from I2S microphone and convert to proper format"""
        # Read samples from the microphone
        bytes_read = self.audio_in.readinto(self.samples_memory)
        
        if bytes_read == 0:
            print("No data read from microphone")
            return None
        
        # Convert bytearray to array of integers
        format_str = "<{}i".format(bytes_read // 4)  # 'i' for 32-bit signed int
        samples = struct.unpack(format_str, self.samples_memory[:bytes_read])
        
        # The INMP441 returns 24-bit values in the low 24 bits of each 32-bit word
        # Need to convert to proper signed values
        samples = [s >> 8 for s in samples]  # Right-shift by 8 to get 24-bit values
        return samples

    def rms_from_samples(self):
        """Calculate RMS (Root Mean Square) value from I2S audio samples"""
        samples = self.read_samples()

        if not samples or len(samples) == 0:
            return 0

        # Calculate mean of squares
        square_sum = 0
        for s in samples:
            square_sum += s * s
        mean_square = square_sum / len(samples)
        rms = int(sqrt(mean_square))
        return rms

    async def monitor_sound(self, callback=None):
        """Continuously monitor sound levels and call callback with normalized RMS values"""
        self.is_monitoring = True
        asyncio.create_task(self.min_max_vals())
        asyncio.create_task(self.sound_task_monitor())
        self.nvs.settings["mic_on"] = self.is_monitoring
 
        while self.is_monitoring:
            try:
                # Read fresh samples from the microphone
                rms = self.rms_from_samples()

                # Store the latest RMS value
                self.memory_deque.append(rms)


                # Normalize so that the top 30% of values all count as the max value
                if self.max_val != self.min_val:
                    threshold = self.min_val + self.max_normalization * (self.max_val - self.min_val)
                    if rms >= threshold:
                        normalized_rms = 1.0
                    else:
                        normalized_rms = (rms - self.min_val) / (threshold - self.min_val)
                        normalized_rms = max(0.0, min(normalized_rms, 1.0))
                else:
                    normalized_rms = 0
                
                                    
                if callback:
                    try:
                        callback(normalized_rms)
                        # Uncomment below for debugging
                        # print(f"Callback called with RMS: {normalized_rms:.3f}")
                    except Exception as e:
                        print(f"Error in mic callback: {e}")
            except Exception as e:
                print(f"Error in monitor_sound: {e}")
            await asyncio.sleep_ms(self.interval_ms)
    
    def stop_monitoring(self):
        """Stop the continuous sound monitoring"""
        self.is_monitoring = False
    
    def close(self):
        """Clean up I2S resources"""
        self.stop_monitoring()
        self.nvs.settings["mic_on"] = self.is_monitoring
        self.sound_task = None
        # self.audio_in.deinit()
        if self.espnow:
            self.espnow.broadcast("led.stop()")
            # self.espnow.broadcast("led.led.xp_on=True()")

    async def decay(self, decay_interval=500):
        """Gradually decay max/min values to prevent drift"""
        while self.max_val > self.max_val_ideal:
            self.max_val -= 500
            # self.min_val += 100
            await asyncio.sleep_ms(decay_interval)

    async def min_max_vals(self, check_interval=2):
        """Continuously update min/max values for normalization"""
        while self.is_monitoring:
            try:
                if len(self.memory_deque) > 0:
                    self.max_val_ideal = max(max(self.memory_deque), 5000)
                    if self.max_val > self.max_val_ideal:
                        asyncio.create_task(self.decay())
                    else:  
                        self.max_val = self.max_val_ideal
                    self.min_val = min(self.memory_deque)
                else:
                    print("Memory deque is empty")
            except Exception as e:
                print(f"min_max exception: {e}")
            await asyncio.sleep(check_interval)

    async def sound_task_monitor(self):
        """ informs the user periodically that the sound task is running"""
        while self.is_monitoring:
            print(f"Sound monitoring is active. Min-Max are : {self.min_val} - {self.max_val}")
            if self.espnow:
                self.espnow.broadcast("led.xp_on=True")
            await asyncio.sleep(10)

    def toggle_mic_task(self, callback=None):
        """Toggle the sound monitoring task on/off"""
        if self.sound_task:
            self.stop_monitoring()
            self.sound_task = None
            self.is_monitoring = False
            self.led.active_mode = "off"
            print("Sound monitoring stopped.")
        else:
            self.sound_task = asyncio.create_task(self.monitor_sound(callback))
            self.led.active_mode = "sound"
            print("Sound monitoring started.")

    def update_rms_from_ws(self, rms):
        """Update RMS from WebSocket client audio data"""
        try:
            # Store the RMS value in the memory deque for processing
            self.memory_deque.append(int(rms))
            print(f"Updated RMS from WebSocket: {rms}")
        except Exception as e:
            print(f"Error updating RMS from WebSocket: {e}")


if __name__ == "__main__":
    from app.led import LED
    from app.network_manager import NetworkManager
    from app.system import NVSManager
    
    print("Starting AsyncMicReader...")
    loop = asyncio.get_event_loop()
    nvs = NVSManager()
    led = LED(nvs)
    mic_reader = AsyncMicReader(nvs)
    # mic_reader.interval_ms = 500
    network_manager = NetworkManager(nvs, led, loop=loop)
    

    def limit_map(x, in_min, in_max, out_min, out_max):
        """Map a value from one range to another with bounds checking"""
        return int(((max(in_min, min(in_max, x))) - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)

    def duty_freq_col(duty):
        """Handle audio level callback - control LED and broadcast to network"""
        try:
            if nvs.settings["led_on"]:
                led.duty_freq_col(duty)
            # network_manager.espnow.broadcast(f"led.start_freq_normal({duty*20})")
            network_manager.espnow.broadcast(f"led.duty_freq_col({duty})")
            # print(f"Audio level: {int(duty * 100)}%")
        except Exception as e:
            print(f"Error in duty_freq_col: {e}")

    async def main():
        """Main async loop for microphone monitoring"""
        print("Starting sound monitoring...")
        await network_manager.start_espnow()
        mic_reader.sound_task = loop.create_task(mic_reader.monitor_sound(duty_freq_col))
        
        try:
            while True:
                await asyncio.sleep(5)  # Keep the main loop alive
        except KeyboardInterrupt:
            print("Keyboard interrupt received")
        finally:
            mic_reader.stop_monitoring()
            print("Sound monitoring stopped")


    asyncio.run(main())
