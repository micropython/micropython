# PSoC6 Port Tests

The PSoc6 tests are organized in the following way:

```
psoc6/
├─ board_ext_hw/        --> Tests require board with extended hardware config
│  ├─ diagrams/         --> Boards hardware extension diagrams
│  ├─ multi/            --> Tests require multiple boards instances
│  ├─ single/           --> Tests require a single board
├─ board_only_hw/       --> Tests only require the boards (no additional hardware)
│  ├─ multi/            --> Tests require multiple boards instances 
│  ├─ single/           --> Tests require a single board
├─ mp_custom/           --> Tests based on mpremote (and not based on the run-test.py utility)
├─ inputs/              --> Non-test files utilities required as inputs for the tests
├─ run_psoc6_tests.sh   --> Script handling PSoC6 tests
├─ README
```
## Running the tests

In order to run a particular test or group of tests make use of the `run_psoc6_tests.h` script.

Find the available test suites and options by running the help command:

```
./run_psoc6_tests.sh help
```

For example, to run the "PWM" tests (default device will be /dev/ttyACM0):

```
./run_psoc6_tests.sh -t pwm
```

If you need to specify the serial ports of the devices, for example, in a multiple instances test:

```
./run_psoc6_tests.sh -t bitstream --dev-test /dev/ttyACM0 --dev-stub /dev/ttyACM3
```

Or directly running all the tests of test under CI/CD for a given board type:

```
./run_psoc6_tests.sh --test-suite ci-tests --board CY8CPROTO-062-4343W --hil ifx-mpy-hil
```

In this case, the hardware device list connected to the hardware in the loop (HIL) needs to be provided.
 
## Extended Hardware Setup Tests

The tests located in the `board_ext_hw/` implement the validation of hardware peripherals features (GPIO, I2C, SPI, etc.) which require additional or extended hardware configuration to the evaluation boards.

The extended setup is meant to provide the necessary circuitry, electronic system or interface for the tests to be performed. For example, interconnecting some pins (with simple cables) and/or attach some external circuit, device, or discrete electronic components. 

### Boards Setup Diagram

Find below the hardware diagrams for each of the supported boards:

#### CY8CPROTO-062-4343W

![CY8CPROTO-062-4343W HIL setup](board_ext_hw/diagrams/cy8cproto-062-4343w-hil-test-diag.png)

#### CY8CPROTO-063-BLE

![CY8CPROTO-063-BLE HIL setup](board_ext_hw/diagrams/cy8cproto-063-ble-hil-test-diag.png)

#### CY8CKIT-062S2-AI

![CY8CKIT-062S2-AI HIL setup](board_ext_hw/diagrams/cy8ckit-062s2-ai-hil-test-diag.png)

### Developing Extended Hardware Setup Tests

The driving principle for designing these tests is to keep a **simple** (as simple as possible), **reproducible** and **scalable** hardware-in-the-loop setup, by aiming for **self-contained** board setups using minimal or no hardware other than the board itself. 

Most of the MCU (and boards) provide a given feature and its complementary feature. What do we mean by a feature and its *complementary* feature? Conceptually, many protocols and system topologies are based on a complementary or opposite role interaction: input-output, master-slave, server-client or controller-peripheral, sender-receiver. Its operation is effective when they are interacting together. Thus, the evaluation of one of them is hardly achievable without each other. 

In our case, both roles are likely to be available and implementable in a single (or worst case, with a couple of) evaluation board. 
For example, testing a GPIO input API functionality can be done by using a GPIO output API, and physically connecting the 2 ports together with a cable.
Another example would be to validate an ADC by connecting it to a DAC in the same board. Or connecting an instance of a I2C master to a I2C slave, in the same board, with a few cables and pull-up resistors.

If this is the case, it will often simplify the whole testing hardware infrastructure by removing the need of external hardware devices and complex interfaces. For example, measuring equipment that is not always available, potentially expensive, and harder to reproduce and scale.

Sometimes the *complementary* feature might not be available. In that case, it is worth to evaluate which is the most convenient approach. Let´s imagine we have a master SPI API which could be easily tested with a SPI slave in the same micocontroller. If it is not available it will require to be developed for MicroPython. Sure, it is an effort, but keep in mind that such SPI slave mode is not just a testing utility, but an usable enablement for MicroPython end (primary) users. Besides, how simple, reproducible, scalalable and automatable are the other options? 

Yes, also automatable. As the rests of the PSoC6 folder tests, these tests need to be able to **run automatically** under the MicroPython testing suite tools, without the requiring visual inspections and manual checks. That way they can be added to the Continuous Integration testing pipelines, and reassuring quality after every affected or relevant code change. 