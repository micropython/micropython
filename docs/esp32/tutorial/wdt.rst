
.. _esp32_wdt:

WDT
===

When the WDT timeout is short, a problem occurs when updating the software:
you don't have enough time to copy the updates to the device.
ESP32 allows reinitialise the watchdog with a longer timeout - like an hour.

More comprehansive example usage::

    # Save as 'main.py' and it will run automatically after 'boot.py'

    import sys
    from time import sleep
    from machine import WDT, reset

    try:
        print('Press Ctrl-C to break the program.')
        sleep(5)
        print('Enable the WDT with a timeout of 5s.')
        wdt = WDT(timeout=5000)

        seconds = 1
        while True:  # infinite work loop
            print('Do something useful in less than 5 seconds.')
            print(f'Sleep {seconds} seconds.')
            if seconds >= 5:
                print(f'WDT will reboot the board.')
            sleep(seconds)
            seconds += 1
            wdt.feed()

            # 1/0  # uncomment this line to raise an runtime exception, then WDT reboot

            # sys.exit()  # uncomment this line for planned program exit

    except SystemExit:
        wdt = WDT(timeout=1000 * 60 * 60)  # 1 hour before WDT reboot
        print('Now you have 1 hour to update program on the board.')
        print('When ready, type reset() on the REPL/WebREPL command line to reboot.')

