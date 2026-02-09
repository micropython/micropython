# led_module.py
# import gc
import math
import ujson
from urandom import randint
import asyncio
from machine import Pin, Timer, ADC
from neopixel import NeoPixel
from micropython import schedule
from time import ticks_ms, ticks_diff
from random import uniform

def mapping(x, in_min, in_max, out_min, out_max):
    return int((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)

def limit_map(x, in_min, in_max, out_min, out_max):
    return int(((max(in_min,min(in_max,x))) - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)

class LED:
    def __init__(self, nvs, loop=None):
        self.nvs = nvs
        self.battery = Battery(nvs.settings["battery_pin"])
        self.loop = loop
        
        self.num_leds = self.nvs.settings["num_leds"]
        self.led_pin = self.nvs.settings["led_pin"]
        self.led = NeoPixel(Pin(self.led_pin), self.num_leds)
        self.timer = Timer(0)
        self.freq = self.nvs.settings["led_freq"]
        self.min_freq = 3.0
        self.max_freq = 20.0
        self.bright = self.nvs.settings["led_bright"]
        self.m_bri = self.nvs.settings["led_m_bri"]
        self.ran = self.nvs.settings["led_ran"]
        self.hue = self.nvs.settings["led_hue"]
        self.sat = self.nvs.settings["led_sat"]

        # NeoPixel channel order
        # 0 = RGB, 1 = GRB, 2 = BRG
        self.rgb_order = int(self.nvs.settings.get("rgb_order", 0))
        self._order = self._order_tuple(self.rgb_order)

        self.rgb = [255, 0, 0]
        self._rgb = tuple(self.rgb)
        self.off_tup = (0, 0, 0)

        self.led_on = self.nvs.settings["on_at_boot"]

        self.on = False
        self.left = False
        self.right = False
        self.bilateral_on = 0
        self.flick_modes = (
            self._flick,
            self._bilateral_v1,
            self._bilateral_v2,
        )
        self.flick_mode = self._flick
        self.flick_on = self._fill_defaults
        self.flick_off = self._off
        self.active_mode = None
        
        self.flick_state = 0
        self.duty = int(self.nvs.settings["led_duty"] * 10)
        self.duty_range = 10

        self.variant = None
        self._led_formation()
        self.mode = None
        self.xp_on = False 
        self.duration = self.nvs.settings["led_duration"]

        self.config_server = None

        self.bands = ((3.0, 8.0), (8.0, 12.0), (12.0, 21.0), (21.0, 30.0), (35.0, 45.0))

        self.start_time = 0
        self.end_time = 0

        # Additional properties from LED class
        self.modes = None
        self.set_rot_modes(('m_bri',))
        self.server = None

        # Ensure initial _rgb respects brightness and channel order
        self.set_brightness(self.bright)

    
    def _led_formation(self):
        if self.num_leds == 3:
            self.leds_left = (0,)
            self.leds_right = (2,)
            self.variant = False
        else:
            halfway = self.num_leds // 2
            self.leds_left = tuple(range(halfway))
            self.leds_right = tuple(range(halfway, self.num_leds))
            self.variant = True

    def fill(self, r, g, b):
        # Accept logical (r, g, b) and reorder for the physical strip
        packed = self._pack((int(r), int(g), int(b)))
        for led in self.leds_left + self.leds_right:
            self.led[led] = packed
        self.led.write()

    @staticmethod
    def _order_tuple(rgb_order):
        # Map logical (r, g, b) -> strip order
        if rgb_order == 1:  # GRB
            return (1, 0, 2)
        if rgb_order == 2:  # BRG
            return (2, 0, 1)
        return (0, 1, 2)  # RGB (default)

    def _pack(self, rgb):
        o0, o1, o2 = self._order
        return (rgb[o0], rgb[o1], rgb[o2])
    
    def _fill_defaults(self, args=None):
        self.led.fill(self._rgb)
        self.led.write()

    def _off(self, args=None):
        self.led.fill(self.off_tup)
        self.led.write()
        
    def _fill_left(self):
        for i in self.leds_left:
            self.led[i] = self._rgb
        for i in self.leds_right:
            self.led[i] = self.off_tup
        self.led.write()
        
    def _fill_right(self):
        for i in self.leds_right:
            self.led[i] = self._rgb
        for i in self.leds_left:
            self.led[i] = self.off_tup
        self.led.write()

    def start_flick(self, freq=None, send=None):
        self.freq = freq if freq else self.freq
        self.set_brightness(self.bright)
        callback = self.flick_mode
        freq = int(self.freq * self.duty_range) 
        self.timer.init(freq=freq, mode=Timer.PERIODIC, callback=callback)
        if send:
            self.nvs.ble.send(f"vals/freq/{self.freq}")
        if self.config_server:
            self.config_server.broadcast_message('value_update', "freq", freq)

    # update brightness, frequency and colour based on single float
    def b_f_c(self, bright):
        hue_inc = int(limit_map(bright, 0.0, 1.0, 0, 3))
        self.set_hs((self.hue + hue_inc) % 360)
        freq = limit_map(bright, 0.0, 1.0, self.min_freq, self.max_freq)
        self.bright = bright
        self.start_flick(freq)

    # universal and low-message length function to update flicker freq, brightness and color
    def uni(self, freq=-1, bright=-1, duty=-1, sat=-1, hue=-1, bilat=-1, stop=-1):
        if bilat != -1:
            self.change_bilateral(int(bilat))
        if duty != -1:
            self.duty = int(min(1.0, max(0.0, duty)) * 10)
        if hue != -1 or sat != -1:
            self.set_hs(None if hue == -1 else hue, None if sat == -1 else sat)
        bright = self.bright if bright == -1 else bright
        if freq != -1:
            if bright != -1:
                self.bright = bright
            self.start_flick(freq)
        else:
            self.set_brightness(bright)
        if stop != -1:
            self.stop_flick()
        
    def bright_freq(self, bright, freq):
        self.bright = bright
        self.start_flick(freq)
        
    def stop_flick(self):
        self.timer.deinit()
        self._off()
    
    def stop(self):
        print("Stopping All")
        if self.mode:
            self.mode.cancel()
            self.mode = None
            self.xp_on = False
            self.broadcast_state(self.active_mode, False)
            self.active_mode = None
        self.stop_flick()

    def _flick(self, timer):
        if not self.on and self.flick_state < self.duty:
            schedule(self.flick_on, None)
            self.on = True
        elif self.on and self.flick_state >= self.duty:
            schedule(self.flick_off, None)
            self.on = False
        self.flick_state = (self.flick_state + 1) % self.duty_range

        
    def _bilateral_v1(self, timer):
        if not self.on and self.flick_state > self.duty:
            self._fill_left()
            self.on = True
        elif self.on and self.flick_state <= self.duty:
            self._fill_right()
            self.on = False
        self.flick_state = (self.flick_state + 1) % self.duty_range

    def _bilateral_v2(self, timer):
        if 0 <= self.flick_state < 3:
            self._off()
        elif 3 <= self.flick_state < 5:
            self._fill_left()
        elif 5 <= self.flick_state < 7:
            self._off()
        elif 7 <= self.flick_state < 10:
            self._fill_right()
        self.flick_state = (self.flick_state + 1) % self.duty_range

    def set_brightness(self, bright):
        self.bright = bright
        real_bright = max(0.0, min(1.0, self.bright * self.m_bri))
        rgb_scaled = (
            max(0, min(255, int(self.rgb[0] * real_bright))),
            max(0, min(255, int(self.rgb[1] * real_bright))),
            max(0, min(255, int(self.rgb[2] * real_bright))),
        )
        if not self.rgb_order:
            self._rgb = rgb_scaled
        else:
            self._rgb = self._pack(rgb_scaled)

    
    def update_rgb_tuple(self, rgb):
        self.rgb = [int(rgb[0]), int(rgb[1]), int(rgb[2])]
        self.set_brightness(self.bright)

    def update_rgb(self, r, g, b):
        self.rgb = [int(r), int(g), int(b)]
        self.set_brightness(self.bright)


    def set_hs(self, hue=None, sat=None):
        h = (self.hue if hue is None else hue) % 360
        s = max(0.0, min(1.0, self.sat if sat is None else sat))
        self.hue, self.sat = h, s
        if not s:
            self.update_rgb(255, 255, 255)
            return self._rgb
        i = int(h / 60) % 6
        f = (h / 60) - int(h / 60)
        p = int(255 * (1 - s))
        q = int(255 * (1 - s * f))
        t = int(255 * (1 - s * (1 - f)))
        v = 255
        rgb = ((v, t, p), (q, v, p), (p, v, t), (p, q, v), (t, p, v), (v, p, q))[i]
        self.update_rgb(*rgb)
        return self._rgb

    def red(self, r):
        self.rgb[0] = int(r)
        self.set_brightness(self.bright)

    def green(self, g):
        self.rgb[1] = int(g)
        self.set_brightness(self.bright)

    def blue(self, b):
        self.rgb[2] = int(b)
        self.set_brightness(self.bright)
    
    def random_col(self):
        self.rgb = [randint(0, 255), randint(0, 255), randint(0, 255)]
        self.set_brightness(self.bright)
    


    async def a_freq_transition(self, new_freq, duration=500, start_freq=None): # creates smoother transition (100ms*pace long) from one freq to the next
        if start_freq:
            self.freq = start_freq
        interval_ms = 100
        step = duration // interval_ms
        step_freq = (new_freq - self.freq) / step
        for i in range(step):
            self.freq += step_freq
            await self.start_flick(self.freq)
            await asyncio.sleep_ms(interval_ms)
    
    async def a_freq_transition_dur(self, new_freq=10, new_bright=1.0, duration=500, start_freq=None): # creates smoother transition (100ms*pace long) from one freq to the next
        if start_freq:
            self.freq = start_freq
        step = max(1,duration // 500)
        step_freq = (new_freq - self.freq) / step
        try:
            step_bright  = (new_bright - self.bright) / step
        except Exception as e:
            print(f"transition {e}")
            step_bright = 0
        for i in range(step):
#             print("step ", i)
            self.freq += step_freq
            self.bright += step_bright
            await self.start_flick(self.freq)
            await asyncio.sleep_ms(500)


    def toggle_bilateral(self):
        try:
            index = self.flick_modes.index(self.flick_mode)
            index = (index + 1) % len(self.flick_modes) 
            self.flick_mode = self.flick_modes[index]
            self.start_flick(self.freq)
            print(f"Flick mode: {self.flick_mode} - index: {index}")
            self.nvs.ble.send(f"btn/bi{index}/true")
        except Exception as e:
            print(f"Error toggling bilateral mode: {e}")


    # 0 = flick, 1 = bilateral_v1, 2 = bilateral_v2
    def change_bilateral(self, mode):
        try:
            mode = int(mode)
            self.flick_mode = self.flick_modes[mode]
            self.start_flick(self.freq)
            print(f"Flick mode: {self.flick_mode.__name__} - index: {mode}")
            for modes in self.flick_modes:
                self.broadcast_state(modes.__name__, 0)
            self.broadcast_state(self.flick_mode.__name__, 1)
            self.nvs.ble.send(f"btn/bi{mode}/true")
        except Exception as e:
            print(f"Error changing bilateral mode: {e}")
        

    async def a_pulse(self, dur=3, hue=1):
        prev_rgb = list(self.rgb)
        prev_bright = self.bright
        prev_hue = self.hue
        prev_sat = self.sat

        self.set_hs(hue, 1.0)
        start_time = ticks_ms()
        end_time = start_time + (dur * 1000)

        while ticks_ms() < end_time:
            elapsed = ticks_diff(ticks_ms(), start_time)
            phase = (elapsed % (dur * 1000)) / (dur * 1000) * 2 * math.pi
            bright = abs(0.4 * math.sin(phase))
            self.set_brightness(bright)
            self._fill_defaults()
            await asyncio.sleep_ms(50)

        self.rgb = prev_rgb
        self.hue = prev_hue
        self.sat = prev_sat
        self.set_hs(self.hue, self.sat)
        self.set_brightness(prev_bright)
        self._off()

    def broadcast_state(self, button_ID, state):
        try:
            if self.config_server:
                self.config_server.broadcast_state(f'{button_ID}', state)

        except Exception as e:
            print(f"Error broadcasting state: {e}")
    
    def start_mode(self, mode, restart=False):
        if self.mode:
            self.mode.cancel()
        self.xp_on = True
        if self.loop:
            self.mode = self.loop.create_task(mode)
        else:
            self.mode = asyncio.create_task(mode)
        # self.nvs.ble.send(f"mode/{mode}/true")

    def set_rot_modes(self, modes):
        self.modes = modes
        if self.nvs.rot:
            self.nvs.rot.state = self.modes[0]
            self.nvs.rot.mode_selector = 0
    
    async def a_battery_display(self):
        start_bright = self.bright
        if self.battery:
            self.battery.read_battery_voltage()
            hue_indicator = limit_map(self.battery.reading, 3400, 4100, 0, 120)
            self.battery.percentage = hue_indicator
            await self.a_pulse(dur=3, hue=hue_indicator)
        else:
            # Fallback if no battery object
            await self.a_pulse(dur=3, hue=300)
        self.bright = start_bright
        self.stop_flick()

    async def a_xp_manual(self):
        self.xp_on = True
        self.active_mode = "manual_mode"
        self.broadcast_state(self.active_mode, True)
        print("started xp_manual")
        self.set_rot_modes(('m_bri', 'freq', 'hue', 'sat'))
        # self.duration = 60 * 30
        self.bright = 1.0
        self.start_flick(self.freq)
        timer = ticks_ms() + (self.duration * 1000)
        while ticks_ms() < timer:
            await asyncio.sleep(10)
        self.stop_flick()
        self.broadcast_state(self.active_mode, False)
        self.active_mode = None
        self.xp_on = False
        
    async def a_xp_custom(self,
                        min_freq=8,
                        max_freq=12,
                        duration=None,
                        bilateral=0
                        ):
        self.xp_on = True
        self.active_mode = "experience_mode"
        self.broadcast_state(self.active_mode, True)
        print("started xp_custom")
        self.change_bilateral(bilateral)
        duration = self.duration if not duration else duration
        min_freq=min_freq
        max_freq=max_freq
        self.set_rot_modes(('m_bri', 'hue', 'sat'))
        timer = ticks_ms() + (duration * 1000)
        while ticks_ms() < timer:
            new_freq = uniform(min_freq, max_freq)
            self.start_flick(new_freq)
            await asyncio.sleep(3)
        self.stop_flick()
        self.broadcast_state(self.active_mode, False)
        self.active_mode = None
        self.xp_on = False

    async def a_xp_group(self, duration=600):
        self.xp_on = True
        self.active_mode = "group_mode"
        self.broadcast_state(self.active_mode, True)
        print("started xp_group")
        self.set_rot_modes(('m_bri', ))
        self.stop_flick()
        await asyncio.sleep(duration)
        self.stop_flick()
        self.broadcast_state(self.active_mode, False)
        self.active_mode = None
        self.xp_on = False

    def xp_state(self, state=1):
        state = int(state)
        self.start_mode(self.a_xp_custom(min_freq=self.bands[state][0],max_freq=self.bands[state][1]))
        self.nvs.ble.send(f"btn/xp{state}/true")


    async def a_play_custom_program(self, program_path=None):
        program_data = {}
        metadata = {}
        if not program_path:
            print("No program path provided.")
            return

        
        async def async_program_loader(program_path):
            metadata = None
            async def lines_async(fp):
                for line in fp:
                    yield line
            with open(program_path, 'r') as f:
                # Read first line as metadata
                first_line = f.readline()
                if first_line:
                    metadata = ujson.loads(first_line)
                    yield ('metadata', metadata)
                # Read the rest as program data, yielding each after async sleep
                for line in f:
                    await asyncio.sleep(0)
                    entry = ujson.loads(line)
                    yield ('data', entry)
        program_loader = async_program_loader(program_path)

        
        self.xp = True
        self.active_mode = metadata.get('name', 'custom_program')
        self.broadcast_state(self.active_mode, True)
        print(f"Started {self.active_mode}")
        start_time = ticks_ms()
        duration = metadata.get('duration', 600)
        index = 0
        next_time = program_data[index][0]
        while ticks_ms() < start_time + (duration * 1000):
            now = ticks_diff(ticks_ms(), start_time) / 1000.0
            try:
                if now >= next_time:
                    entry = program_data[index][1]
                    index += 1
                    try:
                        next_time = program_data[index][0]
                    except KeyError:
                        break
                    if entry > 1.0:
                        self.start_flick(entry)
                    elif 0.0 <= entry <= 1.0:
                        self.set_brightness(entry)
                    elif isinstance(entry, tuple):
                        self.update_rgb_tuple(entry)
                    else:
                        exec(entry)
            except Exception as e:
                print(f"Error processing: {e}")
            await asyncio.sleep(0.01)
        
        self.stop_flick()
        self.broadcast_state(self.active_mode, False)
        self.active_mode = None
        self.xp_on = False
        print(f"{self.active_mode} program ended.")



class Battery():
    def __init__(self, battery_pin):
        self.shutdown_counter = 0
        # Set up the ADC on Pin 2
        self.adc = ADC(Pin(battery_pin))
        # Configure ADC
        self.adc.atten(ADC.ATTN_11DB)  # Full range: 3.3v
        self.reading = self.read_battery_voltage()
        self.percentage = 0
        
    def read_battery_voltage(self):
        total_voltage = 0
        for _ in range(16):
            # Read voltage in millivolts
            total_voltage += self.adc.read_uv() / 1000
        # Calculate average voltage
        average_voltage = total_voltage / 16
        # Convert to actual battery voltage
        # Factor is 2 because of voltage divider (100kΩ + 100kΩ)
        battery_voltage = 2 * (average_voltage)
        self.reading = int(battery_voltage)
        return self.reading


if __name__ == "__main__":
    from test_scripts.nvs import NVS
    nvs = NVS()
    led = LED(nvs)
    led.update_rgb(255, 255, 255)
    led._fill_defaults()
    # led.set_brightness(1.0)
    # led.start_flick(10.0)