import virtualtimers, sys, machine, system
import esp32

requestedStandbyTime = 0
onSleepCallback = None

nvs = esp32.NVS("system")
timeout = 20000
try:
    timeout = nvs.get_i32("sleep")
except Exception as e:
    timeout = 20000

disabled = False
enabled = False

def disable():
    global disabled, enabled
    disabled = True
    enabled = False
    kill()

def enable():
    global disabled, enabled, timeout
    if timeout < 1: #Disabled by setting
        return
    disabled = False
    enabled = True
    feed()

def resume():
    global disabled, enabled
    if disabled and enabled:
        enable()

def state():
    if disabled:
        return -1
    return timeout

def usb_attached():
    try:
        import voltages
        return voltages.usb() > 4500
    except:
        return False

def pm_task():
    ''' The power management task [internal function] '''
    global requestedStandbyTime
    global timeout
    global disabled

    if disabled:
        return timeout

    idleTime = virtualtimers.idle_time()

    usbStayAwake = False
    try:
        import voltages
        usbStayAwake = voltages.usb() > 2500 and machine.nvs_get_u8('system', 'usb_stay_awake')
    except:
        pass

    if idleTime > 30000 and (not usbStayAwake):
        global onSleepCallback
        if not onSleepCallback==None:
            print("[Power management] Running onSleepCallback...")
            try:
                onSleepCallback(idleTime)
            except BaseException as e:
                print("[ERROR] An error occurred in the on sleep callback.")
                sys.print_exception(e)
        system.sleep(idleTime, False)

    return timeout

def feed(start=True):
    ''' Resets the timer '''
    global timeout, enabled
    if start:
        enabled = True
    if enabled:
        if timeout < 1: #Disabled by setting
            return
        if not virtualtimers.update(timeout, pm_task):
            virtualtimers.new(timeout, pm_task, True)

def kill():
    ''' Kills the power management task '''
    virtualtimers.delete(pm_task)
    global disabled, enabled
    disabled = True
    enabled = False

def callback(cb):
    ''' Set a callback which is run before sleeping '''
    global onSleepCallback
    onSleepCallback = cb

def set_timeout(t):
    ''' Set timeout '''
    global timeout
    timeout = t

try:
    # If present, include the service for undervoltage detection
    __import__('tasks/undervoltagemon')
except:
    pass
