### Prerequisites

To facilitate the installation of the MicroPython PSoC™ Edge port, the `mpy-pse.py` Python script is
provided. It is compatible with Windows, Linux and macOS.

Before downloading and running the script, it is recommended to create a new folder to keep all the
related files together. For example:

```bash
mkdir mp-install
cd mp-install
```

You can easily download the script from the terminal using the following command:

```bash
curl -s -L https://raw.githubusercontent.com/micropython/micropython/master/tools/psoc-edge/mpy-pse.py > mpy-pse.py
```

Ensure you have a recent version of [Python 3.x](https://www.python.org/downloads/) installed and the
[pip](https://pip.pypa.io/en/stable/installation/) package installer. Then install the following packages:

```bash
pip install requests
```

### Flashing

This can be done using the
`device-setup` command. Specify the board and the path and name of the `.hex` file using the `-f` flag
as shown below:

```bash
python mpy-pse.py device-setup -b KIT_PSE84_AI -f pathtodir/mpy-psoc-edge_KIT_PSE84_AI.hex
```

#### Multiple connected devices

If you have multiple PSoC™ Edge boards connected to your computer, you can distinguish them by their
serial number, and flash them one by one. To do so, use the `-n` flag to provide the serial number of
the target board as shown below:

```bash
python mpy-pse.py device-setup -b KIT_PSE84_AI -n 181F0D5A01212300
```

Alternatively, you can deploy a firmware file to all connected boards if you have a device YAML file
following the [etdevs](https://pypi.org/project/etdevs/) specification.

> **Note:** This option is only supported on **Linux** and **macOS** systems. etdevs is not currently
> enabled for Windows.

First, ensure that the etdevs package is installed:

```bash
pip install etdevs
```

Then, create a YAML file (e.g., *devs.yaml*) with the board `name` and the `uid` (debugger serial
number) for all the connected available boards:

```yaml
- name: KIT_PSE84_AI
  uid: 181F0D5A01212300

- name: KIT_PSE84_AI
  uid: 181F0D5A01212400
```

Finally, run the `device-setup` command providing the YAML file with the `-d` flag:

```bash
python mpy-pse.py device-setup -b KIT_PSE84_AI -d devs.yml
```
