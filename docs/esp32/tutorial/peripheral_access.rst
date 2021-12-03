Accessing peripherals directly via registers
============================================

The ESP32's peripherals can be controlled via direct register reads and writes.
This requires reading the datasheet to know what registers to use and what
values to write to them.  The following example shows how to turn on and change
the prescaler of the MCPWM0 peripheral.

.. code-block:: python3

    from micropython import const
    from machine import mem32

    # Define the register addresses that will be used.
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

    # Change the MCPWM0 prescaler.
    print(hex(mem32[MCPWM0])) # read PWM_CLK_CFG_REG (reset value = 0)
    mem32[MCPWM0] = 0x55      # change PWM_CLK_PRESCALE
    print(hex(mem32[MCPWM0])) # read PWM_CLK_CFG_REG

Note that before a peripheral can be used its clock must be enabled and it must
be taken out of reset.  In the above example the following registers are used
for this:

- ``DPORT_PERI_CLK_EN_REG``: used to enable a peripheral clock

- ``DPORT_PERI_RST_EN_REG``: used to reset (or take out of reset) a peripheral

The MCPWM0 peripheral is in bit position 17 of the above two registers, hence
the value of ``DPORT_PWM0_CLK_EN``.
