/*
LCD4Bit v0.1 16/Oct/2006 neillzero http://abstractplain.net

What is this?
An arduino library for comms with HD44780-compatible LCD, in 4-bit mode (saves pins)

Sources:
- The original "LiquidCrystal" 8-bit library and tutorial
    http://www.arduino.cc/en/uploads/Tutorial/LiquidCrystal.zip
    http://www.arduino.cc/en/Tutorial/LCDLibrary
- DEM 16216 datasheet http://www.maplin.co.uk/Media/PDFs/N27AZ.pdf
- Massimo's suggested 4-bit code (I took initialization from here) http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1144924220/8
See also:
- glasspusher's code (probably more correct): http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1160586800/0#0

Tested only with a DEM 16216 (maplin "N27AZ" - http://www.maplin.co.uk/Search.aspx?criteria=N27AZ)
If you use this successfully, consider feeding back to the arduino wiki with a note of which LCD it worked on.

Usage:
see the examples folder of this library distribution.

*/

#include "LCD4Bit.h"


//command bytes for LCD
#define CMD_CLR 0x01
#define CMD_RIGHT 0x1C
#define CMD_LEFT 0x18
#define CMD_CURSOR_LEFT 0x10
#define CMD_CURSOR_RIGHT 0x14
#define CMD_CONFIG_CURSOR 0x0C
#define CMD_HOME 0x02

#define LCD_LINE_1_ADDRESS 0x00
#define LCD_LINE_2_ADDRESS 0x40
#define LCD_LINE_3_ADDRESS 0x14
#define LCD_LINE_4_ADDRESS 0x54

// --------- PINS -------------------------------------
//is the RW pin of the LCD under our control?  If we're only ever going to write to the LCD, we can use one less microcontroller pin, and just tie the LCD pin to the necessary signal, high or low.
//this stops us sending signals to the RW pin if it isn't being used.
int USING_RW = 0;

//RS, RW and Enable can be set to whatever you like
int RS = 4;
int Enable = 5;
//DB should be an unseparated group of pins  - because of lazy coding in pushNibble()
int DB[] = {0, 1, 2, 3};  //wire these to DB4~7 on LCD.
int RW = 6;

static int currentLineNumber;
extern void LCD4Bit_digitalWrite(int pin,int state);
//--------------------------------------------------------

// --------- Delays --------------------------------------
extern void LCD4Bit_delayMicroseconds(int t);
extern void LCD4Bit_delay(int t);
//--------------------------------------------------------

//how many lines has the LCD? (don't change here - specify on calling constructor)
int g_num_lines = 2;

//pulse the Enable pin high (for a microsecond).
//This clocks whatever command or data is in DB4~7 into the LCD controller.
void LCD4Bit_pulseEnablePin(){
  LCD4Bit_digitalWrite(Enable,LOW);
  LCD4Bit_delayMicroseconds(1);
  // send a pulse to enable
  LCD4Bit_digitalWrite(Enable,HIGH);
  LCD4Bit_delayMicroseconds(1);
  LCD4Bit_digitalWrite(Enable,LOW);
  LCD4Bit_delay(1);  // pause 1 ms.  TODO: what delay, if any, is necessary here?
}

//push a nibble of data through the the LCD's DB4~7 pins, clocking with the Enable pin.
//We don't care what RS and RW are, here.
void LCD4Bit_pushNibble(int value){
  int val_nibble= value & 0x0F;  //clean the value.  (unnecessary)

  LCD4Bit_digitalWrite(DB[0],val_nibble & 01);
  val_nibble >>= 1;
  LCD4Bit_digitalWrite(DB[1],val_nibble & 01);
  val_nibble >>= 1;
  LCD4Bit_digitalWrite(DB[2],val_nibble & 01);
  val_nibble >>= 1;
  LCD4Bit_digitalWrite(DB[3],val_nibble & 01);
  val_nibble >>= 1;
  
  LCD4Bit_pulseEnablePin();
}

//push a byte of data through the LCD's DB4~7 pins, in two steps, clocking each with the enable pin.
void LCD4Bit_pushByte(int value){
  int val_lower = value & 0x0F;
  int val_upper = value >> 4;
  LCD4Bit_pushNibble(val_upper);
  LCD4Bit_pushNibble(val_lower);
}


//stuff the library user might call---------------------------------

void LCD4Bit_commandWriteNibble(int nibble) {
  LCD4Bit_digitalWrite(RS, LOW);
  if (USING_RW) { LCD4Bit_digitalWrite(RW, LOW); }
  LCD4Bit_pushNibble(nibble);
}


void LCD4Bit_commandWrite(int value) {
  LCD4Bit_digitalWrite(RS, LOW);
  if (USING_RW) { LCD4Bit_digitalWrite(RW, LOW); }
  LCD4Bit_pushByte(value);
  //TODO: perhaps better to add a delay after EVERY command, here.  many need a delay, apparently.
}




//print the given character at the current cursor position. overwrites, doesn't insert.
void LCD4Bit_print(int value) {
  //set the RS and RW pins to show we're writing data
  LCD4Bit_digitalWrite(RS, HIGH);
  if (USING_RW) { LCD4Bit_digitalWrite(RW, LOW); }

  //let pushByte worry about the intricacies of Enable, nibble order.
  switch(value)
  {
	  case '\n': 
		currentLineNumber++;
		LCD4Bit_cursorTo(currentLineNumber, 0);
		break;
	  case '\t': 
		LCD4Bit_commandWrite(CMD_CURSOR_RIGHT);
		LCD4Bit_commandWrite(CMD_CURSOR_RIGHT);
		LCD4Bit_commandWrite(CMD_CURSOR_RIGHT);
		LCD4Bit_commandWrite(CMD_CURSOR_RIGHT);
		break;
				
	 default:
		LCD4Bit_pushByte(value);
  }
  
}


//print the given string to the LCD at the current cursor position.  overwrites, doesn't insert.
//While I don't understand why this was named printIn (PRINT IN?) in the original LiquidCrystal library, I've preserved it here to maintain the interchangeability of the two libraries.
void LCD4Bit_printIn(const char* buffer) {

    while(*buffer != '\0'){
	     LCD4Bit_print(*buffer++);
    }
    
}


//send the clear screen command to the LCD
void LCD4Bit_clear(){
  LCD4Bit_commandWrite(CMD_CLR);
  currentLineNumber=0;
  LCD4Bit_delay(1);
}


// initiatize lcd after a short pause
//while there are hard-coded details here of lines, cursor and blink settings, you can override these original settings after calling .init()
void LCD4Bit_init (int num_lines,int dotFormat) {

  g_num_lines = num_lines;
  if (g_num_lines < 1 || g_num_lines > 4)
  {
    g_num_lines = 1;
  }
  
  LCD4Bit_delay(50);

  //The first 4 nibbles and timings are not in my DEM16217 SYH datasheet, but apparently are HD44780 standard...
  LCD4Bit_commandWriteNibble(0x03);
  LCD4Bit_delay(5);
  LCD4Bit_commandWriteNibble(0x03);
  LCD4Bit_delayMicroseconds(100);
  LCD4Bit_commandWriteNibble(0x03);
  LCD4Bit_delay(5);

  // needed by the LCDs controller
  //this being 2 sets up 4-bit mode.
  LCD4Bit_commandWriteNibble(0x02);
  LCD4Bit_commandWriteNibble(0x02);
  //todo: make configurable by the user of this library.
  //NFXX where
  //N = num lines (0=1 line or 1=2 lines).
  //F= format (number of dots (0=5x7 or 1=5x10)).
  //X=don't care


  int num_lines_ptn;
  if (g_num_lines==1)
	num_lines_ptn = (0) << 3;
  else
  	num_lines_ptn = (1) << 3;
  	
  int dot_format_ptn = 0x00;      //5x7 dots.  0x04 is 5x10
  if(dotFormat==DOT_FORMAT_5x10)
	dot_format_ptn = 0x04;

  LCD4Bit_commandWriteNibble(num_lines_ptn | dot_format_ptn);
  LCD4Bit_delayMicroseconds(60);

  //The rest of the init is not specific to 4-bit mode.
  //NOTE: we're writing full bytes now, not nibbles.

  // display control:
  // turn display on, cursor off, no blinking
  LCD4Bit_commandWrite(0x0C);
  LCD4Bit_delayMicroseconds(60);

  //clear display
  LCD4Bit_commandWrite(0x01);
  LCD4Bit_delay(3);

  // entry mode set: 06
  // increment automatically, display shift, entire shift off
  LCD4Bit_commandWrite(0x06);

  currentLineNumber=0;

  LCD4Bit_delay(1);//TODO: remove unnecessary delays
}


//non-core stuff --------------------------------------
//move the cursor to the given absolute position.  line numbers start at 0.
void LCD4Bit_cursorTo(int line_num, int x){

	int addr;
	switch(line_num){
		case 0:addr = LCD_LINE_1_ADDRESS;break;
		case 1:addr = LCD_LINE_2_ADDRESS;break;
		case 2:addr = LCD_LINE_3_ADDRESS;break;
		case 3:addr = LCD_LINE_4_ADDRESS;break;
		default:addr = LCD_LINE_1_ADDRESS;break;
	}
	currentLineNumber = line_num;
	if(line_num>=4)
		currentLineNumber=0;
		
	addr+=x;

	LCD4Bit_commandWrite(0x80|addr);

}

//scroll whole display to left
void LCD4Bit_leftScroll(int num_chars, int delay_time){
  for (int i=0; i<num_chars; i++) {
    LCD4Bit_commandWrite(CMD_LEFT);
    LCD4Bit_delay(delay_time);
  }
}

//scroll whole display to right
void LCD4Bit_rightScroll(int num_chars, int delay_time){
  for (int i=0; i<num_chars; i++) {
    LCD4Bit_commandWrite(CMD_LEFT);
    LCD4Bit_delay(delay_time);
  }
}
  
// set cursor
void LCD4Bit_configureCursor(int onOff,int blinkOnOff)
{
	LCD4Bit_commandWrite(CMD_CONFIG_CURSOR | onOff<<1 | blinkOnOff);
	LCD4Bit_delay(1);
}


