import gc
from microdot import Microdot, Response, send_file, redirect
from microdot.websocket import with_websocket
from machine import reset
import json
import asyncio
from time import ticks_ms



class ConfigServer:
    def __init__(self, nvs, led, rot, atcp):
        self.app = Microdot()
        # self.redirect_app = Microdot()
        self.nvs = nvs
        self.led = led
        self.rot = rot
        self.atcp = atcp
        self.espnow = None
        self.IP = self.atcp.wlan.ifconfig()[0] if self.atcp.wlan else None
        self.ws_connections = set()
        self.page = 1
        self.ping_timeout = 30000 # timeout set to 60s
        self.ping_time = ticks_ms()

        # Message queue for websockets
        self.message_queue = []
        self.broadcast_flag = asyncio.ThreadSafeFlag()

        # Define buttons with categories
        self.buttons = {
            'Experiences': {
                'manual_mode': 'led.start_mode(led.a_xp_manual())',
                'Theta': 'led.xp_state(0)',
                'Alpha': 'led.xp_state(1)',
                'lo-Beta': 'led.xp_state(2)',
                'hi-Beta': 'led.xp_state(3)',
                'Gamma': 'led.xp_state(4)',
                'the_full_frequency_range': 'led.a_xp_custom()',
            },
            'Light Controls': {
                'bilateral_off' : 'led.change_bilateral(0)',
                'bilateral_v1': 'led.change_bilateral(1)',
                'bilateral_v2': 'led.change_bilateral(2)',
                'stop_lights': 'led.stop()',
            },
            'Colour Controls': {
                'set_red': 'led.update_active_col((255, 0, 0))',
                'set_green': 'led.update_active_col((0, 255, 0))',
                'set_blue': 'led.update_active_col((0, 0, 255))',
                'set_white': 'led.update_active_col((255, 255, 255))',
                'set_yellow': 'led.update_active_col((255, 255, 0))',
                'set_purple': 'led.update_active_col((128, 0, 128))',
                'set_cyan': 'led.update_active_col((0, 255, 255))',
            },
            'System Controls': {
                "group_mode": 'nvs.toggle_mode("group_mode")',
                "wifi_at_boot": 'nvs.toggle_mode("wifi_at_boot")',
                "on_at_boot": 'nvs.toggle_mode("on_at_boot")',
                'shutdown_device': 'nvs.shutdown()',
                'reboot_device': 'reset()',
                'switch_knob_direction': 'rot.toggle_direction()',
                'refresh_settings': 'asyncio.create_task(network_manager.server.send_initial_state())',
                'update_to_main_version_(stable) ': 'asyncio.create_task(network_manager.update_device(branch="main"))',
                'update_to_development_branch_(experimental) ': 'asyncio.create_task(network_manager.update_device(branch="dev"))',
                # 'update_to_BLE_branch_(highly_experimental) ': 'asyncio.create_task(network_manager.update_device(branch="BLE"))',
                'update_locally_(Alexx_only)': 'network_manager.update_locally()',
            }
        }

        self.button_options = {
            "bilateral": ["Off", "v1", "v2"]
        }

        # Description for rotary values
        self.rot_vals = {
            "m_bri": "Brightness",
            "freq": "Frequency of Light",
            "hue": "Colour Rainbow (0-255): 0=Red, 90=Green, 255=Violet",
            'duty' : "Duty Cycle",
        }

        self.custom_experience_limits = {
            "min_freq": (0.1, 40, 8, 0.1),
            "max_freq": (0.1, 40, 15, 0.1),
            "duration": (10, 1800, 300, 10),
        }


        self.slider_metadata = {
            "m_bri": {
                "label": "Brightness",
                "min": 0.01,
                "max": 1.0,
                "step": 0.01,
                "default": nvs.settings["led_m_bri"],
                "unit": "",
                "description": "Overall LED brightness"
            },
            "freq": {
                "label": "Frequency",
                "min": 3.0,
                "max": 40.0,
                "step": 0.1,
                "default": nvs.settings["led_freq"],
                "unit": "Hz",
                "description": "Light flicker frequency"
            },
            "duty": {
                "label": "Duty Cycle",
                "min": 0.1,
                "max": 0.9,
                "step": 0.1,
                "default": nvs.settings["led_duty"],
                "unit": "",
                "description": "On/off ratio"
            },
            "hue": {
                "label": "Color",
                "min": 1,
                "max": 255,
                "step": 1,
                "default": nvs.settings["led_hue"],
                "unit": "",
                "description": "0=Red, 90=Green, 255=Violet"
            }
        }

        self.setup_routes()

    def setup_routes(self):
        # """Register all routes with the Microdot app"""
        # @self.redirect_app.route('/<path:path>')
        # async def redirect_to_https(request, path):
        #     # Get the host from request or use AP IP
        #     host = request.headers.get('Host', f'{self.IP}').split(':')[0]
        #     https_url = f'https://{host}/{path}'
        #     # Return redirect response
        #     return redirect(https_url)

        @self.app.route('/')
        def index(request):
            return send_file('static/index.html')

        @self.app.route('/style.css')
        def css(request):
            return send_file('static/style.css')

        @self.app.route('/main.js')
        def js(request):
            return send_file('static/main.js')

        @self.app.route('/favicon.ico')
        def favicon(request):
            return ''
            # return send_file('static/favicon.ico')

        # Captive Portal routes for android
        @self.app.route('/generate_204')
        @self.app.route('/gen_204')
        @self.app.route('/hotspot-detect.html')
        @self.app.route('/connecttest.txt')
        def generate_204(request):
            # Return a 204 No Content response
            return redirect('http://192.168.4.1/')
        

        @self.app.route('/webrepl')
        def webrepl(request):
            if self.IP:
                url = f"http://{self.IP}:8266/"
                return Response(status_code=302, headers={'Location': url})
            else:
                return 'IP address not available'
        
        @self.app.route('/shutdown')
        def shutdown(request):
            request.self.app.shutdown()
            return 'The server is shutting down...'
    
            
        @self.app.route('/api/sliders')
        def get_sliders(request):
            return json.dumps(self.slider_metadata)

        @self.app.route('/ws')
        @with_websocket
        async def websocket(request, ws):
            """WebSocket endpoint for all communication"""
            self.ws_connections.add(ws)
            print(f"New WebSocket connection - {len(self.ws_connections)} total")
            # Handle WebSocket messages
            websocket_task = asyncio.create_task(self.message_processor(ws))
            try:
                # Send initial state
                asyncio.create_task(self.send_initial_state())
                while not ws.closed:
                    # print("Waiting for message...")
                    data = await ws.receive()
                    if data is None:
                        break
                    try:
                        message = json.loads(data)
                        # print(f"Received websocket message: {message}")
                        await self.handle_ws_message(message, ws)
                    except json.JSONDecodeError:
                        print(f"Invalid JSON: {data}")
                    except Exception as e:
                        print(f"Error handling message: {e}")
                    
                    await asyncio.sleep(0.2)  # Allow other tasks to run
            except Exception as e:
                print(f"WebSocket error: {e}")
            finally:
                try:
                    self.ws_connections.remove(ws)
                    print(f"WebSocket connection closed - {len(self.ws_connections)} remaining")
                    websocket_task.cancel()
                    websocket_task = None
                    if not self.ws_connections:
                        self.led.xp_on=False
                    gc.collect()
                except:
                    pass

        @self.app.route('/api/buttons')
        def api_buttons(request):
            # Return all buttons (existing behavior)
            return Response(
                body=json.dumps(self.buttons), 
                headers={'Content-Type': 'application/json'}
            )

        @self.app.route('/api/buttons/<category>')
        def api_buttons_category(request, category):
            # Map URL-friendly names to actual dictionary keys
            category_map = {
                'experiences': 'Experiences',
                'light_controls': 'Light Controls', 
                'system_controls': 'System Controls'
            }
            
            actual_category = category_map.get(category)
            
            if actual_category and actual_category in self.buttons:
                # Return single category wrapped in dict to maintain structure
                filtered_buttons = {actual_category: self.buttons[actual_category]}
                return Response(
                    body=json.dumps(filtered_buttons), 
                    headers={'Content-Type': 'application/json'}
                )
            else:
                return Response(
                    body=json.dumps({'error': 'Category not found'}), 
                    headers={'Content-Type': 'application/json'},
                    status_code=404
                )

    async def send_initial_state(self):
        """Send initial state only to the newly connected client"""
        try:
            # await asyncio.sleep(1)
            self.broadcast_message('nvs_settings', 'system', self.nvs.settings)
            await asyncio.sleep(0.1)
            self.populate_networks()
        except Exception as e:
            print(f"Error sending initial state: {e}")

    async def handle_ws_message(self, message, ws):
        """Process WebSocket messages from clients"""
        try:
            # print(f"Handling ws message: {message}")
            message_type = message.get('type', '')

            if message_type == 'log':
                # Handle log messages from the client
                log_message = message.get('message', '')
                print(f"Client Log: {log_message}")

            elif message_type == 'button':
                # Handle button press
                button_id = message.get('button')
                await self.process_button(button_id)
                
            elif message_type == 'slider':
                # Handle slider change
                key = message.get('key')
                value = message.get('value')
                if key and value is not None:
                    await self.process_slider(key, value)
                    
            elif message_type == 'custom_experience':
                # Handle custom experience form
                await self.start_custom_experience(message)
                
            elif message_type == 'wifi':
                # Handle WiFi operations
                operation = message.get('operation')
                if operation == 'add':
                    ssid = message.get('ssid')
                    password = message.get('password')
                    if ssid and password:
                        self.nvs.save_network(ssid, password)
                        self.broadcast_message('system', 'message', f'Added WiFi network: {ssid}')
                        self.populate_networks()
                elif operation == 'delete':
                    ssid = message.get('ssid')
                    if ssid:
                        self.nvs.remove_network(ssid)
                        self.broadcast_message('system', 'message', f'Deleted WiFi network: {ssid}')
                        self.populate_networks()

            elif message_type == 'nvs':
                # Handle NVS operations
                key = message.get('key')
                value = message.get('value')
                if key and value is not None:
                    # Try to convert to appropriate type
                    try:
                        value = int(value)
                    except ValueError:
                        try:
                            value = float(value)
                        except ValueError:
                            pass  # Keep as string
                    
                    self.nvs.save_item(key, value)
                    self.broadcast_message('system', 'message', f'Updated setting {key} to {value}')
                    
            elif message_type == 'command':
                # Handle direct command execution
                command = message.get('command')
                if command:
                    try:
                        exec(command)
                        self.broadcast_message('system', 'message', 'Command executed successfully')
                    except Exception as e:
                        await ws.send(json.dumps({
                            'type': 'error',
                            'message': f'Command execution failed: {str(e)}'
                        }))
                        

            elif message_type == 'ping':
                # Handle ping (keep-alive)
                self.ping_time = ticks_ms()
                print("Ping updated")
                self.led.xp_on=True
                await ws.send(json.dumps({
                    'type': 'pong',
                    'timestamp': message.get('timestamp', 0)
                }))
                
            elif message_type == 'client_audio_rms':
                rms = message.get('rms')
                if rms is not None:
                    self.process_client_audio_rms(rms)
                
            else:
                print(f"Unknown message type: {message_type}")
                
        except Exception as e:
            print(f"Error processing WebSocket message: {e}")
            await ws.send(json.dumps({
                'type': 'error',
                'message': f'Error processing request: {str(e)}'
            }))

    async def process_button(self, button_id):
        """Process a button press from WebSocket"""
        if not button_id:
            return
            
        # Find and execute the command for this button
        for category in self.buttons.values():
            for name, command in category.items():
                if name == button_id:
                    try:
                        if self.espnow:
                            self.espnow.broadcast(f"{command}")
                        exec(command)
                        self.atcp.message = f"{command}".encode()
                        self.atcp.flag.set()
                    except Exception as e:
                        print(f"Error executing button command: {e}")
                    break

    async def process_slider(self, key, value):
        """Process a slider change from WebSocket"""
        try:
            if key in self.rot.values:
                new_value = float(value)
                self.rot.u_vals(new_value, key)
        except ValueError:
            print(f"Invalid value for {key}: {value}")


    async def message_processor(self, ws):
        while not ws.closed:
            try:
                await self.broadcast_flag.wait()
                # Process all queued messages
                while self.message_queue:
                    # Get the next message
                    message = self.message_queue.pop(0)
                    try:
                        message = json.dumps(message)
                        await ws.send(message)
                        # print(f"Sent message to client: {message}")
                        # Yield to allow other tasks to run
                    except Exception as e:
                        print(f"Error sending to client: {e}")
                    
                    # Yield to let other tasks run
                    await asyncio.sleep(0.1)
                
                # Optional: yield before waiting for more messages
                await asyncio.sleep(0.1)
            except Exception as e:
                print(f"Error in message processor: {e}")
                # break


    def broadcast_message(self, message_type, key, value):
        message = {
            'type': message_type,
            'key': key,
            'value': value
        }
        # Check for duplicates (rebounce) and add to queue instead of sending immediately
        self.message_queue = [m for m in self.message_queue if not (m['key'] == key and m['value'] == value)]

        self.message_queue.append(message)
        self.broadcast_flag.set()
        # print(f"Added message to queue: {message}")


    def broadcast_update(self, key, value):
        """Broadcast slider updates to all clients"""
        self.broadcast_message('value_update', key, value)

    def broadcast_state(self, button_id, state):
        """Broadcast button state updates to all clients"""
        self.broadcast_message('state_update', button_id, state)


    async def start_custom_experience(self, params):
        """Start a custom LED experience with the provided parameters"""
        try:
            print(f"Starting custom experience: {params}")
            min_freq = float(params.get('min_freq', 5))
            max_freq = float(params.get('max_freq', 15))
            duration = int(params.get('duration', 300))
            col = int(params.get('col', 0))
            ran = int(params.get('ran', 5))
            bilateral = int(params.get('emdr', 0))
            await self.led.start_mode(self.led.a_xp_custom(min_freq=min_freq,
                                    max_freq=max_freq,
                                    duration=duration,
                                    col=col,
                                    ran=ran,
                                    bilateral=bilateral
                                              ))
        except Exception as e:
            print(f"Error starting custom experience: {e}")
            self.broadcast_message('error', 'message', f'Error starting custom experience: {str(e)}')

    def ping_timer(self):
        async def rolling_timer():
            while ticks_ms() < self.ping_timer + self.ping_timeout:
                print("Ping is up to date")
                await asyncio.sleep(30)
            connections = self.ws_connections.copy()
            closed_connections = set()
            for ws in connections:
                success = await ws.send(message)
                if not success:
                    closed_connections.add(ws)
            # Remove closed connections
            for ws in closed_connections:
                try:
                    self.ws_connections.remove(ws)
                except:
                    pass
        asyncio.create_task(rolling_timer())

    def populate_networks(self):
        """Populate the list of SSIDs from the loaded networks"""
        networks = self.nvs._load_networks()
        ssids = list(networks.keys()) if networks else []
        self.broadcast_message('networks', 'wifi', ssids)

    def generate_slider_metadata(self, slider_selection=None):
        # Use the rotary encoder's values as the source of truth
        metadata = {}
        if not slider_selection:
            slider_selection = self.rot.values.keys()
        for key in slider_selection:
            default, min_val, max_val, _, step, unit, description = self.rot.values.get(key, (None, None, None, None))
                # Only include numeric sliders (filter as needed)
            metadata[key] = {
                "label": description,
                "min": min_val,
                "max": max_val,
                "step": step,
                "default": default,
                "unit": unit,
                # "description": description
            }
        return metadata