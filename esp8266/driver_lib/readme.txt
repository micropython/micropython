STEP 1: Copy driver and include folders to your project sub-folder, such as app folder. Unused drivers can be removed in your project.

STEP 2: Modify Makefile in app folder.
    1). Search SUBDIRS, add driver as subdir:
        SUBDIRS=    \
	    user    \
	    driver

    2). Search COMPONENTS_eagle.app.v6, add libdriver.a:
        COMPONENTS_eagle.app.v6 = \
	   user/libuser.a  \
	   driver/libdriver.a