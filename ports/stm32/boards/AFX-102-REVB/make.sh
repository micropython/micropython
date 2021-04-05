#make -j4 BOARD=AFX-102-REVB OPENOCD_CONFIG=boards/openocd_stm32f7.cfg  deploy-openocd
make -j4 BOARD=AFX-102-REVB OPENOCD_CONFIG=boards/openocd_stm32f7.cfg  $1 $2 $3
