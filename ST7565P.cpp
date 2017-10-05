#include <Arduino.h>
#include <SPI.h>
#include "ST7565P.h"
#include "CFAG4265A0_logo.h"

//-----------------------------------------------------------------------------
void ST7565P::init(void)
{
  bias_set(LCD_BIAS_1_7);
  adc_select(NORMAL);
  common_output_mode_select(NORMAL);
  vo_regulator_resistor_set(LCD_RATIO_5_0);
  //electronic_volume_set(36); // Originally 36
  electronic_volume_set(28);
  booster_ratio_set(LCD_BOOSTER_4X);
  power_controller_set(BOOST_ON, VR_ON, VF_ON);
  start_line_set(0);
  clear();
}
//-----------------------------------------------------------------------------
void ST7565P::adc_select(uint8_t direction)
{
  write_command(0b10100000 | (direction & 0b1));
}
//-----------------------------------------------------------------------------
void ST7565P::all_points(uint8_t on)
{
  write_command(0b10100100 | (on & 0b1));
}
//-----------------------------------------------------------------------------
void ST7565P::bias_set(uint8_t bias)
{
  write_command(0b10100010 | (bias & 0b1));
}
//-----------------------------------------------------------------------------
void ST7565P::booster_ratio_set(uint8_t ratio)
{
  write_command(0b11111000);
  write_command(ratio & 0b11);
}
//-----------------------------------------------------------------------------
void ST7565P::clear(void)
{
  uint8_t page, col;

  for (page = 0; page < 9; ++page)
  {
    page_address_set(page);
    column_address_set(0);
    for (col = 0; col < 42; ++col)
    {
      write_data(0);
    }
  }
}
//-----------------------------------------------------------------------------
void ST7565P::column_address_set(uint8_t column)
{
  write_command(0b00010000 | (column >> 4));
  write_command(0b00000000 | (column & 0b1111));
}
//-----------------------------------------------------------------------------
void ST7565P::common_output_mode_select(uint8_t reverse)
{
  write_command(0b11000000 | ((reverse & 0b1) << 3));
}
//-----------------------------------------------------------------------------
void ST7565P::cmd_reset(void)
{
  write_command(0b11100010);
}
//-----------------------------------------------------------------------------
void ST7565P::display_on(void)
{
  write_command(0b10101111);
}
//-----------------------------------------------------------------------------
void ST7565P::display_off(void)
{
  write_command(0b10101110);
}
//-----------------------------------------------------------------------------
void ST7565P::display_reverse(uint8_t reverse)
{
  write_command(0b101000110 | (reverse & 0b1));
}
//-----------------------------------------------------------------------------
void ST7565P::electronic_volume_set(uint8_t contrast)
{
  write_command(0b10000001);
  write_command(contrast & 0b00111111);
}
//-----------------------------------------------------------------------------
void ST7565P::fill(uint8_t data, uint16_t delay_ms_col, uint16_t delay_ms_page)
{
  uint8_t page, col;

  for (page = 0; page < 9; ++page)
  {
    page_address_set(page);
    column_address_set(0);
    for (col = 0; col < 42; ++col)
    {
      write_data(data);
      delay(delay_ms_col);
    }
    delay(delay_ms_page);
  }
}
//-----------------------------------------------------------------------------
void ST7565P::page_address_set(uint8_t page)
{
  write_command(0b10110000 | (page & 0b1111));
}
//-----------------------------------------------------------------------------
void ST7565P::power_controller_set(uint8_t boost, uint8_t vreg, uint8_t vfollower)
{
  write_command(0b00101000 | ((boost & 0b1) << 2) | ((vreg & 0b1) << 1) | (vfollower & 0b1));
}
//-----------------------------------------------------------------------------
void ST7565P::show_bitmap()
{
  // Display the Crystalfontz CFAG4265 bitmap image.
  //
  // The bitmap2lcd utility generated the data with the image
  // shifted up 7 pixels from the bottom of the LCD. This is
  // handled prior to display of each byte by shifting the
  // byte from the current page down 7 pixels and ORing it
  // with the byte from the next page that is shifted up 1
  // pixel.
  uint8_t page;
  uint16_t i = 0;

  for (i = 0, page = 0; page < 9; ++page)
  {
    page_address_set(page);
    column_address_set(0);
    for (uint8_t col = 0; col < 42; ++col)
    {
      uint8_t data1 = pgm_read_byte(&(cfag4265_logo[i]));
      uint8_t data2 = pgm_read_byte(&(cfag4265_logo[i + 42]));
      write_data((data1 >> 7) | (data2 << 1));
      ++i;
    }
  }
}
//-----------------------------------------------------------------------------
void ST7565P::start_line_set(uint8_t line)
{
  write_command(0b01000000 | (line & 0b111111));
}
//-----------------------------------------------------------------------------
void ST7565P::vo_regulator_resistor_set(uint8_t ratio)
{
  write_command(0b00100000 | (ratio & 0b111));
}
//-----------------------------------------------------------------------------
void ST7565P::write_command(uint8_t data)
{
  // Set LCD_CS low and LCD_A0 low to indicate command to LCD Send
  //  command byte to LCD
  CLR_LCD_CS;
  CLR_LCD_A0;
  SPI.transfer(data);

  // Set LCD_CS high high again
  SET_LCD_CS;
}
//-----------------------------------------------------------------------------
void ST7565P::write_data(uint8_t data)
{
  //Set LCD_CS low and LCD_A0 high to indicate data to LCD
  CLR_LCD_CS;
  SET_LCD_A0;
  //Send data byte to LCD
  SPI.transfer(data);

  // Set LCD_CS high again
  SET_LCD_CS;
}
