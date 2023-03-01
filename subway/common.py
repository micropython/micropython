import logging
import json

DEFAULT_PATH = "config.json"

def touch_config(path=DEFAULT_PATH):
    with open(path, 'w') as f:
        json.dump({}, f)

def write_config(config, path=DEFAULT_PATH):
    with open(path, 'w') as f:
        json.dump(config, f)

def read_config(path=DEFAULT_PATH):
    try:
        with open(path) as f:
            config = json.load(f)

    # File doesn't exist yet
    except OSError:
        touch_config()
        return {}
    
    return config
