## Gen Pin instructions ##

# Input File
pins.csv          : contain pin name, port number ,pin number
pin_functions.csv : contain list of pin support for peripheral
make_pins.py      : python script to gen pin


# Run make_pins.py
Copy above input file to folder boards/brd2601b/
Run CMD:
        $ cd boards/brd2601b/
        $ python make_pins.py -s pins.c -e pin_functions.h pins.csv pin_functions.csv

        -s: name/directory of output source file
        -e: name/directory of output header file

# Output
pins.c          : register pin to board_module_globals_table
				  generate array contains supported function of pin
pin_functions.h : define index of functions
	Example: pin_pa0_functions[FN_EUSART0_RX] == 1 //   Can assign pin pa0 for EUSART0_RX
             pin_pa0_functions[FN_EUSART0_RX] == 255 // Can't assign pin pa0 for EUSART0_RX


# Read pin define on REPL
import board
dir(board)
