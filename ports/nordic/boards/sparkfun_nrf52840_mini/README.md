# SparkFun Pro nRF52840 Mini Breakout

The [SparkFun Pro nRF52840 Mini](https://www.sparkfun.com/products/15025) small breakout board for Raytac's MDBT50Q-P1M module, which features an nRF52840. It breaks out as many pins as it can in an Arduino Pro Mini footprint. Also included on the board are a qwiic (I<sup>2</sup>C) connector, LiPo battery charger, and on/off switch.

Note: the SparkFun Pro nRF52840 Mini Breakout does not include a QSPI external flash. Any Python code will need to be stored on the internal flash filesystem.

## CircuitPython Pin Defs

CircuitPython pin definitions try to follow those of the [Arduino Pro Mini](https://www.sparkfun.com/products/11113), which the footprint is based on.

This can be somewhat confusing, especially around the analog pins. Here's a quick pin-map:

<table>
<tr><th>Board pin label</th><th>Digital Pin Reference</th><th>Additional Pin Capabilities</th><th>Pin/Port Reference</th></tr>
<tr><td>17</td><td>D1</td><td>TX</td><td>P0_17</td></tr>
<tr><td>15</td><td>D0</td><td>RX</td><td>P0_15</td></tr>
<tr><td>8</td><td></td><td>SDA</td><td>P0_08</td></tr>
<tr><td>11</td><td></td><td>SCL</td><td>P0_11</td></tr>
<tr><td>19</td><td>D3</td><td></td><td>P0_19</td></tr>
<tr><td>20</td><td>D4</td><td></td><td>P0_20</td></tr>
<tr><td>21</td><td>D5</td><td></td><td>P0_21</td></tr>
<tr><td>22</td><td>D6</td><td></td><td>P0_22</td></tr>
<tr><td>23</td><td>D7</td><td></td><td>P0_23</td></tr>
<tr><td>9</td><td>D8</td><td></td><td>P0_09</td></tr>
<tr><td>10</td><td>D9</td><td></td><td>P0_10</td></tr>
<tr><td>2</td><td>D10</td><td>A0</td><td>P0_02</td></tr>
<tr><td>3</td><td>D11</td><td>MOSI, A1</td><td>P0_03</td></tr>
<tr><td>31</td><td>D12</td><td>MISO, A7</td><td>P0_31</td></tr>
<tr><td>30</td><td>D13</td><td>SCK, A6</td><td>P0_31</td></tr>
<tr><td>29</td><td></td><td>A5</td><td>P0_29</td></tr>
<tr><td>28</td><td></td><td>A4</td><td>P0_28</td></tr>
<tr><td>5</td><td></td><td>A3</td><td>P0_05</td></tr>
<tr><td>4</td><td></td><td>A2</td><td>P0_04</td></tr>
</table>

If a pin isn't defined as D0, D1, etc., standard port/pin references should work -- e.g. `P0_17` is pin 17, `P0_02` is pin 2, etc.

## Bootloader Notes

The nRF52840 Mini ships with a slightly modified (i.e pin defs and USB defs) version of the Adafruit nRF52 bootloader, which supports UF2 and CDC bootloading.

## Hardware Reference

The nRF52840 Mini hardware layout is open source:

* [Schematic](https://cdn.sparkfun.com/assets/learn_tutorials/8/2/0/nrf52840-breakout-mdbt50q-v10.pdf)
* [Eagle Files](https://cdn.sparkfun.com/assets/learn_tutorials/8/2/0/nrf52840-breakout-mdbt50q-v10.zip)
* [Hookup Guide](https://learn.sparkfun.com/tutorials/sparkfun-pro-nrf52840-mini-hookup-guide)
