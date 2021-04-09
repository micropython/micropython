#make -j4 BOARD=AFX-102-REVB OPENOCD_CONFIG=boards/openocd_stm32f7.cfg  deploy-openocd
make BOARD=STM32F469DISC $1 $2 $3
