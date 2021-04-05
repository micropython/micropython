#make -j4 BOARD=OPEN7XXI_C OPENOCD_CONFIG=boards/openocd_stm32f7.cfg  deploy-openocd
make -j4 BOARD=OPEN7XXI_C OPENOCD_CONFIG=boards/openocd_stm32f7.cfg $1
