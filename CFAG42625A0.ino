//=============================================================================
//
//  written for Seeeduino v4.2 runnung at 3.3v
//
//  CRYSTALFONTZ CFAG4265A0 42x65 Graphic display in 4W-SPI mode
//
//  ref: https://www.crystalfontz.com/product/CFAG4265A0
//
//  2017-10-03 Max / Crystalfontz
//=============================================================================
/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/
//=============================================================================
#include <Arduino.h>
#include <SPI.h>
#include "LTC2630.h"
#include "ST7565P.h"
//=============================================================================
// Function Prototypes
//-----------------------------------------------------------------------------
void spi_master_send_byte(uint8_t data);

//=============================================================================
LTC2630 dac;
ST7565P lcd;
//=============================================================================
void setup(void)
{
  Serial.println(F("Initializing the CFAG4265A0"));
  
  //debug console
  Serial.begin(115200);
  Serial.println(F("Entering setup()"));

  //Thump the reset
  // Set the LCD_RST low for 10 uS and then high again and return
  // after 10 uS. According to the ST7565P datasheet only 1 uS is
  // needed at VDD = 3.3VDC but it won't hurt to be conservative.
  CLR_RESET;
  _delay_us(10);
  SET_RESET;
  _delay_us(10);

  // Set the ports to the appropriate I/O modes
  DDRB |= 0x47;
  DDRD |= 0x80;

  // Set the pins to a reasonable starting state.
  SET_LCD_CS;
  SET_DAC_CS;
  CLR_LCD_A0;
  SET_RESET;

  // Initialize SPI. By default the clock is 4MHz.
  SPI.begin();
  //Bump the clock to 8MHz. Appears to be the maximum.
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  
  dac.init();
  
  lcd.init();

  // Turn on the display and its backlight
  lcd.display_on();
  dac.backlight_set(BACKLIGHT_FULL_ON);
}
//=============================================================================
void loop(void)
{
  Serial.println(F("Loop()"));

  /* Display a fill pattern then clear
  */
  lcd.fill(0x3c, 3, 0);
  lcd.fill(0x00, 3, 0);
  delay(500);
  
  /* Display a fill pattern then clear
  */
  lcd.fill(0x03c, 0, 255);
  lcd.fill(0x00, 0, 255);
  delay(500);
  
  lcd.show_bitmap();

  delay(1000);
  lcd.clear();
}
//=============================================================================
