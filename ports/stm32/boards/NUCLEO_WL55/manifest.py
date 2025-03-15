# Don't include default frozen modules because MCU is tight on flash space.

# Only install the sync version of the LoRa driver because this board doesn't
# have asyncio by default.
require("lora-sync")
require("lora-stm32wl5")
