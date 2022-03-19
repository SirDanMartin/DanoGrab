#ifndef LCD_MODULE_SIRDAN //Header Guard
  #define LCD_MODULE_SIRDAN

  #include <Arduino.h>
  #include <U8g2lib.h>


  void LCD_IN_COIN(String, byte);
  void LCD_Command(String);

  //setup LCD Variable
  extern U8G2_ST7920_128X64_F_SW_SPI u8g2;


#endif
