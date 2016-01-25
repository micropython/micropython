/*
    Poncho UI for EDU-CIAA library.
    Copyright (C) 2015  Ernesto gigliotti. ernestogigliotti@gmail.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
     
*/


/**
 * @brief       Enable Kayboard scanning
 * @param       rows : Number of rows (1, 2, 3 or 4)
 * @param       columns : Number of columns (1, 2, 3 or 4)
 * @return      void
 */
void Board_KEYBOARD_Init(uint8_t rows, uint8_t columns);


/**
 * @brief       Read matrix value of coordinates row and column
 * @param       row : row number (0,1,2 or 3)
 * @param       row : column number (0,1,2 or 3)
 * @return      1 if switch on specified position is pressed, 0 otherwise
 */
int Board_KEYBOARD_readMatrix(uint8_t row,uint8_t col);


/**
 * @brief       This function must be called every 1ms for Keyboard management
 * @return      void
 */
void Board_KEYBOARD_tick_ms(void);


/**
 * @brief       Enable keyboard scan in tick interrupt
 * @return      void
 */
void Board_KEYBOARD_enable(void);

/**
 * @brief       Disable keyboard scan in tick interrupt
 * @return      void
 */
void Board_KEYBOARD_disable(void);





/**
 * @brief       Initializes LCD 
 * @param       lines: number of lines (1, 2 3 or 4)
 * @param       dotFormat: 0:5x7 - 1:5x10
 * @return      void
 */
void Board_LCD_Init(uint8_t lines,uint8_t dotFormat);

/**
 * @brief       Clear LCD screen and sets cursor to (0;0) position
 * @return      void
 */
void Board_LCD_clear(void);

/**
 * @brief       Print a text from current cursor position
 * @param       text: string with null terminator
 * @return      void
 */
void Board_LCD_printString(const char* text);


/**
 * @brief       Sets cursor position to specified value
 * @param       x: number of row (starts from 0)
 * @param       y: number of column (starts from 0)
 * @return      void
 */
void Board_LCD_gotoXY(uint8_t x,uint8_t y);


/**
 * @brief       Configures cursor status
 * @param       onOff: 0: cursor is hide - 1:cursor is visible
 * @param       blinkOnOff: 0:static cursor - 1:blinking cursor
 * @return      void
 */
void Board_LCD_configCursor(uint8_t onOff,uint8_t blinkOnOff);

