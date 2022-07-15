options.defaults(ds18x20=False)

module("onewire.py", opt=3)

if options.ds18x20:
    module("ds18x20.py", opt=3)
