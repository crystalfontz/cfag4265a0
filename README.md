# CFAG4265A0-TFK

This is Arduino [(Seeeduino)](https://www.crystalfontz.com/product/cfapn15062-seeeduino-arduino-clone-microprocessor) sample code is for the [CFAG4265A0-TFK](https://www.crystalfontz.com/product/cfag4265a0tfk-42x65-graphic-one-inch-lcd). This display includes the [ST7565R LCD controller](https://www.crystalfontz.com/controllers/Sitronix/ST7565R).


This display is also available as on a [breakout board that handles the backlight driving](https://www.crystalfontz.com/product/cfag4265a0tfke11-carrier-board-with-lcd) or as a [fully built Seeeduino kit](https://www.crystalfontz.com/product/cfag4265a0tfke12).


This code expects that you are using our breakout board or an equivalent. The ST7565P files set up functions and variables for communicating with the display controller on the display. The LTC2630 files set up functions and variables for communicating with the DAC on the breakout board for controlling the backlight. 

## Connections

```
//=============================================================================
// LCD SPI & control lines
//-----------------------------------------------------------------------------
//   ARD      | Port | LCD                      | Wire Color
// -----------+------+--------------------------+-----------
//  #8/D8     |  PB0 | LCD_A0                   | Green
//  #9/D9     |  PB1 | LCD_CS                   | Purple
// #10/D10    |  PB2 | DAC_CS   (backlight)     | Red
// #11/D11    |  PB3 | HW_MOSI  (hardware SPI)  | Orange
// #12/D12    |  PB4 | LCD_RST  (hardware Reset)| Blue
// #13/D13    |  PB5 | HW_SCK   (hardware SPI)  | Yellow
```

![Assembly](https://user-images.githubusercontent.com/55416371/200682186-506a80d0-2f4e-41df-8acb-e5b488174d5c.JPG)
