# TODO: Split these into separate directories with their own manifests.
options.defaults(lcd160cr=False, ssd1306=False, test=False)

if options.lcd160cr:
    module("lcd160cr.py", opt=3)

    if options.test:
        module("lcd160cr_test.py", opt=3)

if options.ssd1306:
    module("ssd1306.py", opt=3)
