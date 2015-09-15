To build (*):

    $ cd nios2
    $./buildlib
	
The built micropythonlib.a exposes via
micropythonlib.h the following API to enable running
any python script passed in:

void run_pystr(const char *src);
