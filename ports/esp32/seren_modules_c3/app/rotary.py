from machine import Pin, Timer, reset, deepsleep
from micropython import const
import asyncio
from time import ticks_diff, ticks_ms

# Constants for states and directions
_DIR_CW = const(0x10)
_DIR_CCW = const(0x20)
_R_START = const(0x0)
_R_CW_1 = const(0x1)
_R_CW_2 = const(0x2)
_R_CW_3 = const(0x3)
_R_CCW_1 = const(0x4)
_R_CCW_2 = const(0x5)
_R_CCW_3 = const(0x6)
_R_ILLEGAL = const(0x7)



class Rotary:
    def __init__(self, pin_clk, pin_dt):
        self.pin_clk = Pin(pin_clk, Pin.IN, Pin.PULL_UP)
        self.pin_dt = Pin(pin_dt, Pin.IN, Pin.PULL_UP)
        self._state = _R_START
        self._listeners = []


        self.pin_clk.irq(trigger=Pin.IRQ_RISING | Pin.IRQ_FALLING, handler=self._update_state)
        self.pin_dt.irq(trigger=Pin.IRQ_RISING | Pin.IRQ_FALLING, handler=self._update_state)
        
        # State transition tables for rotary encoder
        self._transition_table = [
            [_R_CW_3,            _R_CW_2,  _R_CW_1,  _R_START],
            [_R_CW_3 | _DIR_CCW, _R_START, _R_CW_1,  _R_START],
            [_R_CW_3 | _DIR_CW,  _R_CW_2,  _R_START, _R_START],
            [_R_CW_3,            _R_CCW_2, _R_CCW_1, _R_START],
            [_R_CW_3,            _R_CW_2,  _R_CCW_1, _R_START | _DIR_CW],
            [_R_CW_3,            _R_CCW_2, _R_CW_3,  _R_START | _DIR_CCW],
            [_R_START,           _R_START, _R_START, _R_START],
            [_R_START,           _R_START, _R_START, _R_START]]
        
        
        
    def add_listener(self, listener):
        if listener not in self._listeners:
            self._listeners.append(listener)

    def remove_listener(self, listener):
        if listener in self._listeners:
            self._listeners.remove(listener)

    def _update_state(self, pin):
        clk_dt = (self.pin_clk.value() << 1) | self.pin_dt.value()
        self._state = self._transition_table[self._state & 0x07][clk_dt]
        direction = self._state & 0x30
        # print(f"clk_val {self.pin_clk.value()}, dt_val {self.pin_dt.value()}, CLK_DT; {clk_dt}, Rotary State: {self._state}, Direction: {direction}")
        # Notify listeners of value change
        if direction and self._listeners:
            for listener in self._listeners:
                listener(direction)

    def close(self):
        self.pin_clk.irq(handler=None)
        self.pin_dt.irq(handler=None)


class RotaryEncoder(Rotary):
    def __init__(self, nvs, led=None, network_manager=None, loop=None,
                 pin_num_clk=None, pin_num_dt=None, pin_num_btn=None, grounded=None):
        # Use provided pins or fall back to nvs settings
        pin_num_clk = pin_num_clk if pin_num_clk is not None else nvs.settings["clk_pin"]
        pin_num_dt = pin_num_dt if pin_num_dt is not None else nvs.settings["dt_pin"] 
        pin_num_btn = pin_num_btn if pin_num_btn is not None else nvs.settings["butt_pin"]
        grounded = grounded if grounded is not None else nvs.settings["grounded"]

        
        super().__init__(pin_num_clk, pin_num_dt)
        
        # Store dependencies
        self.nvs = nvs
        self.led = led
        self.network_manager = network_manager
        self.loop = loop
        self.atcp = None
        
        # Button Variables
        self.grounded = grounded
        self.pin_btn = Pin(pin_num_btn, Pin.IN, Pin.PULL_UP if grounded else Pin.PULL_DOWN)
        self.button_pressed = asyncio.ThreadSafeFlag()
        self.presses = 0
        self.last_button_state = self.pin_btn.value()
        self.timer = Timer(nvs.settings["timer_number"])
        self.debounce_time = 200  # milliseconds
        self.button_delay = 700  # milliseconds
        
        # Additional state
        self.mode_selector = 0
        self.state = 'm_bri'
        self.config_server = None
        self.reverse_dir = bool(self.nvs.settings.get("reversed_knob", False))
        
        self.add_listener(self._value_changed)  # Register listener for state changes
        self.pin_btn.irq(
            trigger=Pin.IRQ_FALLING if grounded else Pin.IRQ_RISING, 
            handler=self._button_change
        )
        

        # default, min, max, increment
        self.values = {
            "bright" : [1.0, 0.01, 1.0, 0.2],
            "m_bri" : [0.1, 0.01, 1.0, 0.2],
            "freq" : [10.0, 3.0, 40.0, 0.1],
            'ran' : [5, 1, 10, 1],
            'states' : [1, 0, 4, 1],
            'duration' : [1, 1, 255, 1],
            'duty' : [0.4, 0.1, 0.9, 0.1],
            'min_freq' : [3.0, 3.0, 40.0, 0.1],
            'max_freq' : [20.0, 3.0, 40.0, 0.1],
            'sat' : [1.0, 0.01, 1.0, 0.1],
            'hue' : [1, 1, 360, 10],
            }
        
    def toggle_direction(self):
        print("Toggling Rotary Direction")
        self.reverse_dir = not self.reverse_dir

    def _value_changed(self, rotary_output): # Probably takes about 2ms to execute. Not ideal but not the end of the world :p
        value = self.values[self.state][0]
        if isinstance(self.values[self.state][3], int):
            incr =  self.values[self.state][3]
        else:
            incr = value * self.values[self.state][3]

        if self.reverse_dir:
            rotary_output = 48 - rotary_output  # swap 32 and 16
            
        if rotary_output == 32:
            value = min(self.values[self.state][2], value + incr)
        elif rotary_output == 16:
            value = max(self.values[self.state][1], value - incr)
                
        self.u_vals(value, ble=True)
    
    def u_vals(self, value, key=None, ble=None):
        broadcast = False
        if key == None:
            key = self.state
            broadcast = True
            
        if key == 'm_bri' and self.led:
            self.led.m_bri = value
            self.led.set_brightness(self.led.bright)
        elif key == 'freq' and self.led:
            self.led.start_flick(value)
        elif key == 'ran' and self.led:
            value = int(value)
            self.led.ran = value
        elif key == 'bright' and self.led:
            self.led.bright = value
            self.led.set_brightness(self.led.bright)
        elif key == 'duty' and self.led:
            # value = int(value*10)
            self.led.duty = value
        elif key == 'hue' and self.led:
            value = int(value)
            self.led.set_hs(hue=value)
        elif key == 'sat' and self.led:
            self.led.set_hs(sat=value)

        self.values[key][0] = value
        print(f"Updated {key} to {value}")

        if self.nvs.ble:
            self.nvs.ble.send(f"vals/{key}/{round(value, 2)}")

        if broadcast:
            if self.config_server:
                self.config_server.broadcast_message("value_update", key, value)

        if self.atcp:
            self.atcp.message = f"rot.update_vals({value}, '{key}')".encode()
            self.atcp.flag.set()

    def _button_change(self, pin):
        # Handle button presses with debounce
        current_time = ticks_ms()
        # print(f"Button IRQ Triggered at {current_time} ms")
        if ticks_diff(current_time, self.last_button_state) > self.debounce_time:
            self.presses += 1
            print("Button Pressed:", self.presses)
            self.timer.init(period=self.button_delay, mode=Timer.ONE_SHOT, callback=self._button_action)
            self.last_button_state = current_time
            self.start_time = ticks_ms()

    def _button_action(self, timer):
        self.button_pressed.set()

    async def _butt_results(self):
        actions = {
            1: self._change_mode,
            2: self.a_activate_xp_basic,
            3: self.a_activate_xp_ran,
            4: self.a_toggle_bilateral,
            5: self.a_start_servers,
            6: self.a_update_device,
            7: self.a_toggle_group_mode,
            10: self.a_prepare_for_sale
        }
        
        # button reponse loop. Activates the above actions based on number of button presses
        while True:
            try:
                await self.button_pressed.wait()
                action = actions.get(self.presses)
                if action:
                    if self.loop:
                        self.loop.create_task(action())
                    else:
                        asyncio.create_task(action())
                else:
                    print("I am confused, resetting pressed count")
            except Exception as e:
                print(f"Error in butt_results: {type(e).__name__}, {str(e)}")
            self.presses = 0

    async def _change_mode(self):
        if self.pin_btn.value() != self.grounded:
            print("Closing effects and switching off lights")
            if self.led:
                self.led.stop()
                while self.pin_btn.value() != self.grounded:
                    await self.led.a_battery_display()
                    await asyncio.sleep(2)
                    if self.pin_btn.value() != self.grounded:
                            await self.a_shutdown()
        elif self.led and self.led.modes:
            self.mode_selector = (self.mode_selector + 1) % len(self.led.modes)
            self.state = self.led.modes[self.mode_selector]
            print(f"Changing to {self.state} Mode")
            if self.atcp:
                self.atcp.message = f"rot.state=led.modes[{self.mode_selector}]".encode()
                self.atcp.flag.set()
            # self.nvs.log.info(f"Changing to {self.state} Mode. Brightness: {self.led.bright}, Max Brightness: {self.led.m_bri}, Flicker Frequency: {self.led.freq}, Colour: {self.led.col}")

        elif self.led and not self.led.modes:
            pass

    
    async def a_select_xp(self):
        if self.led and hasattr(self.led, 'xps'):
            num_xps = -1
            for i in self.led.xps: # fills the max xps in the rot.values to match the number of experiences saved in led.xps 
                num_xps += 1
            print(f"num_xps: {num_xps}")
            if 'xps' in self.values:
                self.values['xps'][2] = num_xps
            if self.loop:
                self.loop.create_task(self.led.select_xp())
            else:
                asyncio.create_task(self.led.select_xp())
    
    async def a_activate_xp_basic(self):
        if self.led:
            self.led.start_mode(self.led.a_xp_manual())
        if self.atcp:
            self.atcp.message = b"led.start_mode(led.a_xp_manual())"
            self.atcp.flag.set()

    async def a_activate_xp_ran(self):
        if self.led:
            self.led.start_mode(self.led.a_xp_custom())
        if self.atcp:
            self.atcp.message = b"led.start_mode(led.a_xp_custom())"
            self.atcp.flag.set()
        
    async def a_toggle_bilateral(self):
        if self.led:
            self.led.change_bilateral()
        if self.atcp:
            self.atcp.message = b"led.change_bilateral()"
            self.atcp.flag.set()
    
    async def a_start_servers(self):
        if self.network_manager:
            await self.network_manager.start_servers()
    
    async def a_update_device(self):
        if self.network_manager:
            await self.network_manager.update_device()

    async def a_toggle_group_mode(self):
        self.nvs.toggle_mode("group_mode")
        await self.led.a_pulse(2, 120)
        reset()

    
    async def a_prepare_for_sale(self):
        if self.led:
            self.led.stop()
            self.led.fill(255,255,255)
        if self.network_manager:
            await self.network_manager.update_device(branch="main")
        if self.nvs:
            self.nvs.clear_networks()
        if self.led:
            self.led.fill(255,255,255)

    async def a_shutdown(self):
        try:
            await self.led.a_pulse(2, 200)
            self.network_manager.cleanup()
            self.led.stop()
        except Exception as e:
            print("Problem resetting network: ", e)
        finally:
            self.nvs.shutdown()

    def set_dependencies(self, led=None, network_manager=None, loop=None):
        """Set or update dependencies after initialization"""
        if led is not None:
            self.led = led
        if network_manager is not None:
            self.network_manager = network_manager
        if loop is not None:
            self.loop = loop


if __name__ == "__main__":
    # Test code for RotaryEncoder
    import time
    rot = RotaryEncoder(nvs=None, pin_num_clk=14, pin_num_dt=27, pin_num_btn=26, grounded=True)

    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("Exiting...")
    finally:
        rot.close()