//Set up 128x64 screen
#include <U8g2lib.h>
#include "Config_Setup.h"

U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, /* clock=*/ LCD_PINS_D4, /* data=*/ LCD_PINS_ENABLE, /* CS=*/ LCD_PINS_RS);



void LCD_IN_COIN(String vmessage, byte bar){
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
  u8g2.drawStr(38,13,"DanoGrab");  // write something to the internal memory
  u8g2.setFont(u8g2_font_6x10_mr);
  u8g2.drawStr(36,23,"Crane Game");  // write something to the internal memory
  u8g2.setFont(u8g2_font_unifont_t_symbols);
  u8g2.drawGlyph(14, 18, 0x2654);
  u8g2.drawGlyph(107, 18, 0x2655);
  //u8g2.setFont(u8g2_font_luIS12_tf  );
  u8g2.setFont(u8g2_font_lubBI14_tr );
  u8g2.setCursor(0, 45);
  u8g2.print(vmessage);  // write something to the internal memory
  u8g2.drawFrame(0,56,128,8);
  u8g2.drawBox(0,56,map(bar,0,100,0,128),8);
  // u8g2.setDrawColor(0);
  // u8g2.drawBox(1,57,126,6);
  // u8g2.setDrawColor(1);
  u8g2.sendBuffer();          // transfer internal memory to the display
}

void LCD_Command(String vmessage){
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
  u8g2.drawStr(38,13,"DanoGrab");  // write something to the internal memory
  u8g2.setFont(u8g2_font_6x10_mr);
  u8g2.drawStr(36,23,"Crane Game");  // write something to the internal memory
  u8g2.setFont(u8g2_font_unifont_t_symbols);
  u8g2.drawGlyph(14, 18, 0x2654);
  u8g2.drawGlyph(107, 18, 0x2655);
  //u8g2.setFont(u8g2_font_luIS12_tf  );
  u8g2.setCursor(0, 45);
  u8g2.print(vmessage);  // write something to the internal memory
  u8g2.sendBuffer();          // transfer internal memory to the display
}
