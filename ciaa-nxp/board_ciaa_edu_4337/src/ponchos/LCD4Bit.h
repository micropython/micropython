#ifndef LCD4Bit_h
#define LCD4Bit_h



  void LCD4Bit_commandWrite(int value);
  void LCD4Bit_init(int num_lines,int dotFormat);
  void LCD4Bit_print(int value);
  void LCD4Bit_printIn(const char value[]);
  void LCD4Bit_clear();
  void LCD4Bit_configureCursor(int onOff,int blinkOnOff);
  //non-core---------------
  void LCD4Bit_cursorTo(int line_num, int x);
  void LCD4Bit_leftScroll(int chars, int delay_time);
  void LCD4Bit_rightScroll(int num_chars, int delay_time);  
  //end of non-core--------

  //4bit only, therefore ideally private but may be needed by user
  void LCD4Bit_commandWriteNibble(int nibble);
  void LCD4Bit_pulseEnablePin();
  void LCD4Bit_pushNibble(int nibble);
  void LCD4Bit_pushByte(int value);

#define LOW 0
#define HIGH 1
#define DOT_FORMAT_5x7	0
#define DOT_FORMAT_5x10	1


#endif
