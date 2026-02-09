from machine import unique_id, RTC, WDT, deepsleep, reset
import esp32
import ubinascii
import json
import sys
import os
import collections
import app.version as version



class NVSManager:
    def __init__(self):
        self.rtc = RTC()
        # self.wdt = WDT(timeout=1000000)  # Set watchdog timer with 70 seconds timeout
        self.ble = None
        self.led = None
        self.rot = None
        self.log = Logger("NVSManager", rtc=self.rtc)
        self._nvs = esp32.NVS('settings')
        self._wifis = esp32.NVS('wifis')
        self._pins = esp32.NVS('pins')
        self.esp_model = sys.implementation[2].split()[-1]  # e.g., 's3' or 'c3'
        self.pin_keys = ('led_pin','butt_pin','clk_pin','dt_pin', 'battery_pin')
        self.pins_by_model = {
            "s3": {
            # 'led_pin': 38,
            # 'clk_pin': 5,
            # 'dt_pin': 6,
            # 'butt_pin': 7,
            # 'battery_pin': 15,
            "led_pin": 38,
            "clk_pin": 37,
            "dt_pin": 36,
            "butt_pin": 21,
            "battery_pin": 4,
            },
            "c3": {
            "led_pin": 10,
            "clk_pin": 7,
            "dt_pin": 6,
            "butt_pin": 4,
            "battery_pin": 5,
            },
            "c6": {
            "led_pin": 23,
            "clk_pin": 22,
            "dt_pin": 21,
            "butt_pin": 20,
            "battery_pin": 6,
            },
        }   
        
        self.defaults = {
            # Config values
            'generation': ('unknown', 'str'),
            'timer_number': (1, 'i32'),
            'num_leds': (8, 'i32'),
            'grounded': (1, 'i32'),  
            'microphone': (0, 'i32'),
            'auto_off': (1, 'i32'),
            'butt_debounce': (200, 'i32'),
            
            # LED settings
            'led_freq': (10.0, 'float'),
            'led_bright': (1.0, 'float'),
            'led_m_bri': (0.1, 'float'),
            'led_hue': (1, 'i32'),
            'led_sat': (1.0, 'float'),
            'led_duration': (600, 'i32'),
            'led_ran': (5, 'i32'),
            'led_duty': (0.4, 'float'),
            'min_freq': (8.0, 'float'),
            'max_freq': (15.0, 'float'),
            'rgb_order': (0, 'i32'), # 0= RGB, 1= GRB, 2= BRG

            # General settings
            'device_id': (ubinascii.hexlify(unique_id()).decode(), 'str'),
            'group_mode': (0, 'i32'),
            'wifi_at_boot': (0, 'i32'),
            'ble_at_boot': (1, 'i32'),
            'on_at_boot': (1, 'i32'),
            'shutdown_timer': (5, 'i32'),
            'timezone': (1, 'i32'),  # Timezone offset in hours from UTC

            # Network Settings
            'hostname': ('zenspecs', 'str'),
            'password': ('starstar', 'str'),
            'espnow_channel': (11, 'i32'),
            
            # OTA settings
            'ota_remote_host': ('https://updates.serenspecs.com/', 'str'),
        }
        
        # Cache for settings
        self.settings = {}
        self.config = {}
        self.config_server = None
        
        # Initialize and load settings
        self._init_defaults()
        self._load_settings()
        self.commands = {}
        self.send_queue = collections.deque((), 50)

        
        
    def _load_version_info(self):
        try:
            from app.version import GIT_BRANCH, GIT_COMMIT, BUILD_DATE
            print(f"Compiled from branch: {GIT_BRANCH}, commit: {GIT_COMMIT}, build date: {BUILD_DATE}")
            return GIT_BRANCH, GIT_COMMIT, BUILD_DATE
        except ImportError:
            print("Version info not available")
            return "dev", "unknown", "unknown"

        
    def _load_pins(self): 
        """Load pin settings from NVS"""
        pin_switch = False
        for key in self.pin_keys:
            try:
                self.settings[key] = self._pins.get_i32(key)
            except OSError:
                try:
                    pin_c3 = self._nvs.get_i32(key)
                    self._pins.set_i32(key, pin_c3)
                    self.settings[key] = pin_c3
                    print(f"{key} transfered to _pins as {pin_c3}")
                except OSError:
                    self._pins.set_i32(key, self.pins_by_model[self.esp_model][key])
                    self.settings[key] = self.pins_by_model[self.esp_model][key]
                    print(f"{key} set in _pins as {self.pins_by_model[self.esp_model][key]}")
                pin_switch = True
        if pin_switch:
            self._pins.commit()
            print("Pins set to default values")
        

    def _init_defaults(self):
        """Initialize default values if they don't exist"""
        defaults_switch = False
        for key, (value, type_) in self.defaults.items():
            try:
                if type_ == 'i32':
                    self._nvs.get_i32(key)
                elif type_ == 'float':
                    self._nvs.get_i32(f"{key}")  # Store floats as integer percentages
                elif type_ == 'json':
                    self._get_str(key)
                else:
                    self._get_str(key)
                
            except OSError:
                if type_ == 'i32':
                    self._nvs.set_i32(key, value)
                elif type_ == 'float':
                    self._nvs.set_i32(f"{key}", int(value * 100))
                else:
                    self._set_str(key, value)
                print(f"Set {key} as default {value}")
                defaults_switch = True
        
        if defaults_switch:
            self._nvs.commit()
            print("Settings initialized to default values")


    def reset_to_defaults(self):
        print("Resetting all settings to defaults")
        for key in self.defaults.keys():
            try:
                self._nvs.erase_key(key)
            except OSError:
                pass
        self._nvs.commit()
        self._init_defaults()
        self._load_settings()


    def _get_str(self, key):
        """Get a string value from NVS"""
        buf = bytearray(256)  # Max string length
        try:
            size = self._nvs.get_blob(key, buf)
            return buf[:size].decode()
        except OSError as e:
            # Re-raise the error to let _init_defaults handle it
            raise e

    def _set_str(self, key, value):
        """Set a string value in NVS"""
        if value is not None:
            self._nvs.set_blob(key, value.encode())

    def _load_settings(self):
        """Load all settings into cache"""
        self._load_pins()  # Load pin settings
        # Loads all settings

        for key, (value, type_) in self.defaults.items():
            if type_ == 'i32':
                self.settings[key] = self._nvs.get_i32(key)
            elif type_ == 'float':
                self.settings[key] = self._nvs.get_i32(f"{key}") / 100
            elif type_ == 'json':
                self.settings[key] = json.loads(self._get_str(key) or '{}')
            else:
                self.settings[key] = self._get_str(key)

        build_date = os.uname()[3].split(' ')[-1]  # Use build date from uname
        self.settings['build_date'] = build_date
        self.settings['version'] = version.version
        self.settings['branch'] = version.branch
        self.settings['esp_model'] = self.esp_model


    def _load_networks(self):
        """Load WiFi networks from NVS"""
        try:
            buf = bytearray(4096)  # Larger buffer for networks
            size = self._wifis.get_blob('networks', buf)
            networks_json = buf[:size].decode()
            return json.loads(networks_json)
        except OSError:
            print("No networks found")
            return {}

    def save_network(self, ssid, password):
        """Save a WiFi network"""
        try:
            networks = self._load_networks()
            networks[ssid] = password
            self._wifis.set_blob('networks', json.dumps(networks).encode())
            self._wifis.commit()
            print(f"Saved network: {ssid}")
        except Exception as e:
            print(f"Error saving network: {e}")
            

    def remove_network(self, ssid):
        """Remove a WiFi network"""
        networks = self._load_networks()
        if ssid in networks:
            del networks[ssid]
            self._wifis.set_blob('networks', json.dumps(networks).encode())
            self._wifis.commit()
            print(f"Deleting network: {ssid}")

    def clear_networks(self):
        self._wifis.set_blob('networks', json.dumps({}).encode())
        self._wifis.commit()
        print(f"Deleted all saved networks")


    def save_item(self, key, value):
        """Save a single setting"""
        try:
            if key not in self.settings:
                raise KeyError(f"Key {key} not in settings. Aborting Saving")
            if isinstance(value, float):
                self._nvs.set_i32(key, int(value * 100))
            elif isinstance(value, (int, bool)):
                self._nvs.set_i32(key, int(value))
            else:
                self._set_str(key, str(value))
            self._nvs.commit() 
            self._load_settings()  # Reload cache
            print(f"Saved {key}: {value}")

            if self.ble:
                self.ble.send(f"set/{key}/{value}")
        except Exception as e:
            print(f"ERROR saving {key} as {value}: {e}")


    def toggle_mode(self, mode, setting=None):
        """Toggle a mode setting"""
        try:
            if not setting:
                current_setting = self._nvs.get_i32(mode)
                new_setting = 1 if current_setting == 0 else 0
            else:
                if setting not in [0, 1]:
                    raise ValueError("Setting must be 0 or 1")
                new_setting = setting
            self.save_item(mode, new_setting)
            print(f"{mode} changed to: {new_setting}")
            self.broadcast_state(mode, new_setting)
        except Exception as e:
            print(f"Error toggling {mode}: {e}") 

    def toggle_mode(self, mode, setting=None):
        """Toggle a mode setting"""
        try:
            if not setting:
                current_setting = self._nvs.get_i32(mode)
                new_setting = 1 if current_setting == 0 else 0
            else:
                if setting not in [0, 1]:
                    raise ValueError("Setting must be 0 or 1")
                new_setting = setting
            self.save_item(mode, new_setting)
            print(f"{mode} changed to: {new_setting}")
        except Exception as e:
            print(f"Error toggling {mode}: {e}")
        
    def broadcast_state(self, key, value):
        """Broadcast a state change to the config server"""
        if self.config_server:
            try:
                self.config_server.broadcast_state(key, value)
                # print(f"Broadcasted state: {key} = {value}")
            except Exception as e:
                print(f"Error broadcasting state: {e}")
        else:
            print("Config server not set, cannot broadcast state")

    def lsettings(self):
        """Print all settings"""
        for key, value in sorted(self.settings.items()):
            print(f"{key}: {value}")


    def populate_commands(self, module):
        for method_name in dir(module):
            if not (method_name.startswith("_")):
                method = getattr(module, method_name)
                if callable(method):
                    self.commands[method_name] = method
        # print(sorted(self.commands.keys()))

    def update_rtc(self, dateyear, month, day, weekday, hours, minutes, seconds, subsecondstime):
        """Update RTC time"""
        try:
            values = [int(round(v)) for v in (dateyear, month, day, weekday, hours, minutes, seconds, subsecondstime)]
            datetime_tuple = tuple(values)
            self.rtc.datetime(datetime_tuple)
            print(f"RTC updated to: {datetime_tuple}")
        except Exception as e:
            print(f"Error updating RTC: {e}")

    def shutdown(self):
        """Shutdown the device"""
        print("Shutting down device...")
        if self.settings["esp_model"] == "s3":
            if not self.settings["butt_pin"] == 21: # workaround for the unworking microphone set (Reggie)
                if not self.rot.grounded:
                    wake_level = esp32.WAKEUP_ANY_HIGH
                else:
                    wake_level = esp32.WAKEUP_ALL_LOW
                esp32.wake_on_ext0(self.rot.pin_btn, level=wake_level)
        deepsleep()


class Logger:
    CRITICAL = 50
    ERROR = 40
    WARNING = 30
    INFO = 20
    DEBUG = 10
    
    def __init__(self, name, rtc=None, file="log.txt", max_log_size=500*1024):
        self.name = name
        self.rtc = rtc
        self.file = file
        self.max_log_size = max_log_size  # in bytes

    def _log(self, level, msg, *args):

        # Format timestamp
        t = self.rtc.datetime()
        timestamp = "{:04d}-{:02d}-{:02d} {:02d}:{:02d}:{:02d}".format(
            t[0], t[1], t[2], t[4], t[5], t[6]
        )

        # Format message with args if provided
        if args:
            msg = msg % args

        level_str = self._level_to_str(level)
        # Create NDJSON log entry
        log_obj = f"{timestamp} - {level_str} - {msg}\n"

        # Write to file if specified
        if self.file:
            try:
                # Check file size and rotate if needed
                try:
                    size = os.stat(self.file)[6]
                except OSError:
                    size = 0
                if size >= self.max_log_size:
                    # Rotate: rename current log to .1, overwrite if exists
                    try:
                        os.remove(self.file + ".1")
                    except OSError:
                        pass
                    try:
                        os.rename(self.file, self.file + ".1")
                    except OSError:
                        pass
                with open(self.file, 'a') as f:
                    f.write(log_obj)
                print(f"Logged to {self.file}: {log_obj}")
            except Exception as e:
                print(f"Logger error: {e}")
        else:
            sys.stdout.write(log_obj)
    
    def _level_to_str(self, level):
        return {
            self.CRITICAL: 'CRITICAL',
            self.ERROR: 'ERROR',
            self.WARNING: 'WARN',
            self.INFO: 'INFO',
            self.DEBUG: 'DEBUG'
        }.get(level, str(level))
    
    def debug(self, msg, *args): self._log(self.DEBUG, msg, *args)
    def info(self, msg, *args): self._log(self.INFO, msg, *args)
    def warning(self, msg, *args): self._log(self.WARNING, msg, *args)
    def error(self, msg, *args): self._log(self.ERROR, msg, *args)
    def critical(self, msg, *args): self._log(self.CRITICAL, msg, *args)

    

            

if __name__ == "__main__":
    nvs = NVSManager()
    print(f"length of nvs.settings: {len(nvs.settings)}")
    print(f"length of nvs.networks: {len(nvs._load_networks())}")
     
    # for key, value in sorted(nvs.settings.items()):
    #     print(f'{key}: {value}')
    # # print(f"NVS Settings : {nvs.settings}")
    

    # def update_pins(pin_dict):
    #     for key, pin in pin_dict.items():
    #         nvs._pins.set_i32(key, pin)
    #         print(f"{key} set in _pins as {pin}")
    #     nvs._pins.commit()
    #     print("Pins set to new values")

    # update_pins(nvs.c3_pins)
    # add_home_networks()
    # nvs.save_item('group_mode', 0)
    # nvs.save_item('hostname', "SerenStar")
