# NUCLEO_STMF091RC
The port of micropython for the `STM32F091RC` allows an easy way to use `micropython` for experimenting.

With the `ST-morpho` connectors or the `Arduino` connectors one can hook up his/her favourite interfaces.

Please refer to document: [UM1724](https://www.st.com/content/ccc/resource/technical/document/user_manual/98/2e/fa/4b/e0/82/43/b7/DM00105823.pdf/files/DM00105823.pdf/jcr:content/translations/en.DM00105823.pdf)

## Arduino Serial Port
If one would like to use the Arduino connector to connect to a shield, you will run into trouble when the serial interface needs to be used.

The `NUCLEO_F091RC` PCB is not completely configured to work using the serial port; the `D0/D1` pins for UART communication are simply **NOT** connected!

### So what to do
According to the manual (paragraph 6.8, page 25) you can remove the zero ohm resistors on the back to connect the UART2 to the pins `D0(PA3)` and `D1(PA2)`.

Jumper | State | instruction
---|---| ---
Default connected UART2|| VCOM port of ST-LINK
SB13 | ON | 0 Ohm/ solder joint
SB14 | ON | 0 Ohm/ solder joint
SB62 | OFF | open
SB63 | OFF | open
Connect UART2 to D0/D1 Arduino || VCOM not used
SB13 | OFF | open
SB14 | OFF | open
SB62 | ON | 0 Ohm/ solder joint
SB63 | ON | 0 Ohm/ solder joint

However it requires to reroute another UART to the VCOM of the ST-LINK and you need to change the `REPL` port in `mpconfigboard.h` to make it work as before.

Much simpler is run two cross wires from `CN7-1` to `CN10-35` and `CN7-2` to `CN10-37` to connect `UART3` instead and leave everything as it is. If you want the patch can be made even simpler by running two wires on the back of the PCB from `CN7-1` to `CN10-36` and `CN7-2` to `CN10-38`. Then just install two jumpers between `CN10` `35-36` and `37-38`.
