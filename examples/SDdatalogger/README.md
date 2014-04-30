This is a SDdatalogger, to log data from the accelerometer to the SD-card. It also functions as card reader, so you can easily get the data on your PC.

To run, put the boot.py, cardreader.py and datalogger.py files on either the flash or the SD-card of your pyboard.
Upon reset, the datalogger script is run and logs the data. If you press the user button after reset and hold it until the orange LED goes out, you enter the cardreader mode and the filesystem is mounted to your PC.
