import json

# server_url
# setup_resource
# update_resource

with open('configs/settings.json','r',encoding='utf8') as file:
    settings = json.loads(file.read())

# device_id
# device_secret
# encryption_key
secrets = {}
with open('configs/secrets.json','r',encoding='utf8') as file:
    secrets = json.loads(file.read())

def save_settings():
    with open('configs/settings.json','w',encoding='utf8') as file:
        file.write(json.dumps(settings))

def save_new_settings(new_settings):
    global settings
    with open('configs/settings.json','w',encoding='utf8') as file:
        settings['measurement_sleep_time_s'] = new_settings["mst"]
        settings['message_sleep_time_s'] = new_settings["mwt"]
        settings['warning_measurement_sleep_time_s'] = new_settings["wmst"]
        settings['warning_message_sleep_time_s'] = new_settings["wmwt"]
        settings['warning_distance_mm'] = new_settings["wd"]
        # don't write code version
        file.write(json.dumps(settings))

def save_code_version(code_version : int):
    global settings
    with open('settings.json','w',encoding='utf8') as file:
        settings['code_version'] = code_version
        file.write(json.dumps(settings))
