import logging
import sys
import gc
from common import read_config
from initial_configuration import initial_configuration
from networking import LAN, WebServer

# Run Garbage Collection
gc.collect()

# Start File Logger
logging.init()

# Read Config
config = read_config()

# Initial Device Configuration
# - Host a LAN and wait for someone to connect to it
#       "Live Subway Map"
#       "lsmpassword"
# - Show web page with available networks
# - Save user selected SSID and Password after verifying connection
if not config.get('ssid'):
    logging.logger.info("No configuration found")
    initial_configuration()
    logging.logger.info("New configuration written. Restarting.")
    sys.exit()

# Normal Device Operation
else:
    logging.logger.info("Config: %s", config)
    lan = LAN(config.get('ssid'), config.get('password'))
    lan.connect()
    
    simplepage = WebServer()
    simplepage.wait_for_response()
    # After a response the operation will exit
