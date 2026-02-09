import gc
# print(f"Free Memory pre IMPORT: {gc.mem_free()}")
import asyncio
from app.rotary import RotaryEncoder
from app.led import LED
from app.system import NVSManager
from app.network_manager import NetworkManager

gc.collect()

# print(f"Free Memory after IMPORT : {gc.mem_free()}")

nvs = NVSManager()
# nvs.lsettings()

loop = asyncio.get_event_loop()
rot = RotaryEncoder(nvs, loop=loop)
led = LED(nvs, loop=loop)
network_manager = NetworkManager(nvs, led, rot, loop)
ble = None

# Import and start BLE if enabled
if nvs.settings["ble_at_boot"]:
    try:
        from app.ble import BLE
        ble = BLE(nvs, loop=loop)
        ble.espnow = network_manager.espnow
        nvs.ble = ble
        nvs.populate_commands(ble)
        print("Booted in BLE mode")
    except ImportError as e:
        print("No BLE Libraries available: ", e)

# Set dependencies after all objects are created

nvs.led = led
nvs.rot = rot
rot.set_dependencies(led=led, network_manager=network_manager)

# Import and start mic if available
if nvs.settings["microphone"]:
    try:
        from app.mic import AsyncMicReader
        mic = AsyncMicReader(nvs, led=led, espnow=network_manager.espnow)
        nvs.mic = mic
        nvs.populate_commands(mic)
        print("Microphone running")
    except ImportError as e:
        print("No Microphone Libraries available: ", e)

#Populate commands from LED and Rot for BLE
for module in [led, rot, nvs, network_manager]:
    nvs.populate_commands(module)
# print(nvs.commands)

gc.collect()
print(" class instances Initiatied - Memory: ", gc.mem_free())


def broadcast(message):
    if network_manager.espnow:
        network_manager.espnow.broadcast(message)

async def startup():
    if nvs.settings["led_duty"] == 1.0:
        nvs.save_item("led_duty", 0.4)
        led.duty = 0.4
    # Universal starting scripts
    loop.create_task(rot._butt_results())
    await led.a_battery_display()
    await network_manager.connect_wifi_on_boot()
    # Device dependant starting scripts
    if nvs.settings["microphone"]:
        mic.toggle_mic_task(led.b_f_c)
        return
    if not nvs.settings["group_mode"]:
        print(f"Starting in Solo Mode")
        led.start_mode(led.a_xp_manual())
        return
    else:
        print(f"Starting in Group Mode")
        rot.u_vals(1.0, "m_bri")
        led.xp_on = True
        await network_manager.start_espnow()
        return
    
            
async def ota_rollback():
    try:
        await asyncio.sleep(10)
        import ota.rollback
        ota.rollback.cancel()
        print("OTA Rollback Canceled. New version is saved")
    except Exception as e:
        print("No ota.rollback modules: ", e)


""""""""""""""""""""""""""""""""""""""""""""""""""""" MAIN LOOP """"""""""""""""""""""""""""""""""""""""""""""""""" 
async def main():
    await startup()
    gc.collect()
    shutdown_counter = 0
    print("Startup Complete - Memory: ", gc.mem_free())
    nvs.log.info(f"Device started. Version: {nvs.settings['version']}, Build Date: {nvs.settings['build_date']}")
    loop.create_task(ota_rollback())
    while True:
        gc.collect()
        await asyncio.sleep(30)
        if not led.xp_on:
            shutdown_counter += 1
            if shutdown_counter > (nvs.settings["shutdown_timer"] * 2) and nvs.settings["auto_off"]:
                await rot.a_shutdown()
            elif shutdown_counter > 3:
                loop.create_task(led.a_battery_display())
        else:
            shutdown_counter = 0

try:
    loop.run_until_complete(main())
except KeyboardInterrupt:
    led.stop_flick()

