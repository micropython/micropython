.. _esp32_mem32:

Accessing peripherals using mem32 command
=========================================

.. code-block:: python3

	from micropython import const
	from machine import mem32

	DR_REG_DPORT_BASE = const(0x3FF00000)
	DPORT_PERIP_CLK_EN_REG = const(DR_REG_DPORT_BASE + 0x0C0)
	DPORT_PERIP_RST_EN_REG = const(DR_REG_DPORT_BASE + 0x0C4)
	DPORT_PWM0_CLK_EN = const(1 << 17)
	MCPWM0 = const(0x3FF5E000)
	MCPWM1 = const(0x3FF6C000)

	# Enable CLK and disable RST.
	print(hex(mem32[DPORT_PERIP_CLK_EN_REG] & 0xffffffff))
	print(hex(mem32[DPORT_PERIP_RST_EN_REG] & 0xffffffff))
	mem32[DPORT_PERIP_CLK_EN_REG] |= DPORT_PWM0_CLK_EN
	mem32[DPORT_PERIP_RST_EN_REG] &= ~DPORT_PWM0_CLK_EN
	print(hex(mem32[DPORT_PERIP_CLK_EN_REG] & 0xffffffff))
	print(hex(mem32[DPORT_PERIP_RST_EN_REG] & 0xffffffff))

	print(hex(mem32[MCPWM0])) # read PWM_CLK_CFG_REG (reset value = 0)
	mem32[MCPWM0] = 0x55      # try to change PWM_CLK_PRESCALE
	print(hex(mem32[MCPWM0])) # read PWM_CLK_CFG_REG

The commands have the following functionality - 

* DPORT_PERI_CLK_EN_REG: enables the hardware accelerator clock.

* DPORT_PERI_RST_EN_REG: resets the accelerator.

* DPORT_PERIP_CLK_EN_REG=1: enables the peripheral clock.

* DPORT_PERIP_RST_EN_REG: resets peripherals.

* DPORT_WIFI_CLK_EN_REG: used for Wi-Fi and BT clock gating.

* DPORT_WIFI_RST_EN_REG: used for Wi-Fi and BT reset.
