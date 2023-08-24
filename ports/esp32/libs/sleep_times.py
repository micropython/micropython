import json
import time

# transmit_time the actual time at which to awake
# measurement_time the time between measurements
# warning_level boolean showing if in a high warning level state
sleep_times = {}
with open('configs/sleep_times.json','r',encoding='utf8') as file:
    data = file.read()
    print('PRitning sleep times')
    print(data)
    sleep_times = json.loads(data)

def process_warnings(warning : bool):
    from libs.configs import settings
    global sleep_times
    if warning:
        print('warning_measurement_sleep_time_s')
        sleep_times['measurement_time'] = settings['warning_measurement_sleep_time_s']
        if sleep_times['transmit_time'] > int(time.time()) + settings['warning_message_sleep_time_s']:
            sleep_times['transmit_time'] = int(time.time()) + settings['warning_message_sleep_time_s']
        sleep_times['warning_level'] = True
    else:
        sleep_times['measurement_time'] = settings['measurement_sleep_time_s']
        sleep_times['warning_level'] = False

    with open('configs/sleep_times.json','w',encoding='utf8') as sleep_times_file:
        sleep_times_file.write(json.dumps(sleep_times))

def set_measurements_sleep_time():
    from libs.configs import settings
    # global sleep_times
    sleep_times['measurement_time'] = settings['message_sleep_time']
    with open('configs/sleep_times.json','w',encoding='utf8') as sleep_times_file:
        sleep_times_file.write(json.dumps(sleep_times))

def set_transmit_time():
    from libs.configs import settings
    # global sleep_times
    print('writing new transmit time', settings['message_sleep_time_s'] + time.time())
    sleep_times['transmit_time'] = int(settings['message_sleep_time_s'] + time.time())
    with open('configs/sleep_times.json','w',encoding='utf8') as sleep_times_file:
        sleep_times_file.write(json.dumps(sleep_times))
    print('saved sleep time')
