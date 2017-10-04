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
#include <SPI.h>

#define ARDUINO_SPI 1

#include "CFAG4265A0_logo.h"
//-----------------------------------------------------------------------------

// Backlight full on at DAC output = 1.5V
// Backlight off at DAC output < 50mV
#define BACKLIGHT_FULL_ON 232
#define BACKLIGHT_FULL_OFF 0

typedef enum {
  DAC_REG_WRITE = 0b00000000,
  DAC_REG_UPDATE = 0b00010000,
  DAC_REG_WRITE_UPDATE = 0b00110000,
  DAC_POWER_DOWN = 0b01000000,
  DAC_INTERNAL_REF = 0b01100000,
  DAC_VCC_REF = 0b01110000,
} dac_cmd_t;

// LTC2630-8 DAC Commands

// ST7565P LCD Controller Definitions
#define LCD_BIAS_1_7 1
#define LCD_BIAS_1_9 0
#define NORMAL 0
#define REVERSE 1

#define LCD_RATIO_3_0 0b000
#define LCD_RATIO_3_5 0b001
#define LCD_RATIO_4_0 0b010
#define LCD_RATIO_4_5 0b011
#define LCD_RATIO_5_0 0b100
#define LCD_RATIO_5_5 0b101
#define LCD_RATIO_6_0 0b110
#define LCD_RATIO_6_5 0b111

#define LCD_BOOSTER_2X 0
#define LCD_BOOSTER_3X 0
#define LCD_BOOSTER_4X 0
#define LCD_BOOSTER_5X 0b01
#define LCD_BOOSTER_6X 0b11

#define BOOST_ON 1
#define BOOST_OFF 0
#define VR_ON 1
#define VR_OFF 0
#define VF_ON 1
#define VF_OFF 0

//=============================================================================
// LCD SPI & control lines
//   ARD      | Port | LCD
// -----------+------+-------------------------
//  #8/D8     |  PB0 | LCD_A0
//  #9/D9     |  PB1 | LCD_CS
// #10/D10    |  PB2 | DAC_CS
// #11/D11    |  PB3 | HW_MOSI    (hardware SPI)
// #12/D12    |  PB4 | not used   (hardware MISO)
// #13/D13    |  PB5 | HW_SCK     (hardware SPI)

// A0 = LCD_A0
#define LCD_A0 0x01
#define CLR_LCD_A0    (PORTB &= ~(LCD_A0))
#define SET_LCD_A0    (PORTB |= (LCD_A0))

// CS1 = LCD_CS
#define LCD_CS 0x02
#define CLR_LCD_CS    (PORTB &= ~(LCD_CS))
#define SET_LCD_CS    (PORTB |= (LCD_CS))

// CS2 = DAC_CS
#define DAC_CS 0x04
#define CLR_DAC_CS    (PORTB &= ~(DAC_CS))
#define SET_DAC_CS    (PORTB |= (DAC_CS))

// MOSI
// #define HW_MOSI 0x08
// #define CLR_MOSI    (PORTB &= ~(HW_MOSI))
// #define SET_MOSI    (PORTB |= (HW_MOSI))

// MISO
// #define HW_MISO 0x10
// #define CLR_MISO    (PORTB &= ~(HW_MISO))
// #define SET_MISO    (PORTB |= (HW_MISO))

// SCK
// #define HW_SCK 0x20
// #define CLR_SCK    (PORTB &= ~(HW_SCK))
// #define SET_SCK    (PORTB |= (HW_SCK))

// HW_RESET
#define HW_RESET 0x80
#define CLR_RESET    (PORTD &= ~(HW_RESET))
#define SET_RESET    (PORTD |= (HW_RESET))

#define NULL_DATA 0
//=============================================================================
// Function Prototypes
//-----------------------------------------------------------------------------
void dac_init(void);
void dac_cmd_send(dac_cmd_t cmd, uint8_t data);

void gpio_init(void);

void hw_reset(void);

void lcd_backlight_cycle(void);
void lcd_backlight_set(uint8_t light);
void lcd_clear(void);
void lcd_column_address_set(uint8_t column);
void lcd_init(void);
void lcd_page_address_set(uint8_t page);
void lcd_cmd_send(uint8_t data);

void lcd_fill(uint8_t data, uint16_t delay_ms_col, uint16_t delay_ms_page);
void lcd_display_on(void);
void lcd_display_off(void);
void lcd_start_line_set(uint8_t line);
void lcd_adc_select(uint8_t direction);
void lcd_display_reverse(uint8_t reverse);
void lcd_all_points(uint8_t on);
void lcd_bias_set(uint8_t bias);
void lcd_cmd_reset(void);
void lcd_common_output_mode_select(uint8_t reverse);
void lcd_power_controller_set(uint8_t boost, uint8_t vreg, uint8_t vfollower);
void lcd_electronic_volume_set(uint8_t contrast);
void lcd_vo_regulator_resistor_set(uint8_t ratio);
void lcd_booster_ratio_set(uint8_t ratio);

void spi_init(void);
void spi_master_send_byte(uint8_t data);

//=============================================================================
void gpio_init(void)
{
  //SEE #defines above
  DDRB |= 0x47;
  DDRD |= 0x80;

  //Drive the ports to a reasonable starting state.
  SET_LCD_CS;
  SET_DAC_CS;
  CLR_LCD_A0;
  SET_RESET;
}
//=============================================================================
#if ARDUINO_SPI == 0

void spi_init(void)
{
  /* Initialize SPI as master sck = fck/16, data clocked on rising
      * edge. The SPI clock may be set as high as 20MHz for the
      * ST7565P LCD controller on the CFAO4265.
      */
  SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

void spi_master_send_byte(uint8_t data)
{
  /* Start data transmission
      */
  SPDR = data;

  /* Wait for transmission complete
      */
  while (!(SPSR & (1 << SPIF)));
}
#endif

void lcd_cmd_send(uint8_t data)
{
  // Set LCD_CS low and LCD_A0 low to indicate command to LCD Send
  //  command byte to LCD
  CLR_LCD_CS;
  CLR_LCD_A0;
#if ARDUINO_SPI == 1
  SPI.transfer(data);
#else
  spi_master_send_byte(data);
#endif
  delay(1);

  // Set LCD_CS high and LCD_A0 high again
  SET_LCD_CS;
//  SET_LCD_A0;
}

void lcd_data_send(uint8_t data)
{
  //Set LCD_CS low and LCD_A0 high to indicate data to LCD
  CLR_LCD_CS;
  SET_LCD_A0;
  //Send data byte to LCD
#if ARDUINO_SPI == 1
  SPI.transfer(data);
#else
  spi_master_send_byte(data);
#endif
  delay(1);

  // Set LCD_CS high and LCD_A0 high again
  SET_LCD_CS;
}

void lcd_init(void)
{
  lcd_bias_set(LCD_BIAS_1_7);
  delay(1);
  lcd_adc_select(NORMAL);
  delay(1);
  lcd_common_output_mode_select(NORMAL);
  delay(1);
  lcd_vo_regulator_resistor_set(LCD_RATIO_5_0);
  delay(1);
  //lcd_electronic_volume_set(36); // Originally 36
  lcd_electronic_volume_set(28);
  delay(1);
  lcd_booster_ratio_set(LCD_BOOSTER_4X);
  delay(1);
  lcd_power_controller_set(BOOST_ON, VR_ON, VF_ON);
  delay(1);
  lcd_start_line_set(0);
  delay(1);
  lcd_clear();
  delay(1);
}

void lcd_display_on(void)
{
  lcd_cmd_send(0b10101111);
}

void lcd_display_off(void)
{
  lcd_cmd_send(0b10101110);
}

void lcd_start_line_set(uint8_t line)
{
  lcd_cmd_send(0b01000000 | (line & 0b111111));
}

void lcd_page_address_set(uint8_t page)
{
  lcd_cmd_send(0b10110000 | (page & 0b1111));
}

void lcd_column_address_set(uint8_t column)
{
  lcd_cmd_send(0b00010000 | (column >> 4));
  lcd_cmd_send(0b00000000 | (column & 0b1111));
}

void lcd_adc_select(uint8_t direction)
{
  lcd_cmd_send(0b10100000 | (direction & 0b1));
}

void lcd_display_reverse(uint8_t reverse)
{
  lcd_cmd_send(0b101000110 | (reverse & 0b1));
}

void lcd_all_points(uint8_t on)
{
  lcd_cmd_send(0b10100100 | (on & 0b1));
}

void lcd_bias_set(uint8_t bias)
{
  lcd_cmd_send(0b10100010 | (bias & 0b1));
}

void lcd_cmd_reset(void)
{
  lcd_cmd_send(0b11100010);
}

void lcd_common_output_mode_select(uint8_t reverse)
{
  lcd_cmd_send(0b11000000 | ((reverse & 0b1) << 3));
}

void lcd_power_controller_set(uint8_t boost, uint8_t vreg, uint8_t vfollower)
{
  lcd_cmd_send(0b00101000 | ((boost & 0b1) << 2) | ((vreg & 0b1) << 1) | (vfollower & 0b1));
}

void lcd_electronic_volume_set(uint8_t contrast)
{
  lcd_cmd_send(0b10000001);
  lcd_cmd_send(contrast & 0b00111111);
}

void lcd_vo_regulator_resistor_set(uint8_t ratio)
{
  lcd_cmd_send(0b00100000 | (ratio & 0b111));
}

void lcd_booster_ratio_set(uint8_t ratio)
{
  lcd_cmd_send(0b11111000);
  lcd_cmd_send(ratio & 0b11);
}

void lcd_clear(void)
{
  uint8_t page, col;

  for (page = 0; page < 9; ++page)
  {
    lcd_page_address_set(page);
    lcd_column_address_set(0);
    for (col = 0; col < 42; ++col)
    {
      lcd_data_send(0);
    }
  }
}

void lcd_fill(uint8_t data, uint16_t delay_ms_col, uint16_t delay_ms_page)
{
  uint8_t page, col;

  for (page = 0; page < 9; ++page)
  {
    lcd_page_address_set(page);
    lcd_column_address_set(0);
    for (col = 0; col < 42; ++col)
    {
      lcd_data_send(data);
      delay(delay_ms_col);
    }
    delay(delay_ms_page);
  }
}

void lcd_backlight_set(uint8_t light)
{
  dac_cmd_send(DAC_REG_WRITE_UPDATE, light);
}

void lcd_backlight_cycle(void)
{
  uint8_t light_val;

  /* Decrease backlight from full brightness to almost off
      */
  for (light_val = BACKLIGHT_FULL_ON; light_val > BACKLIGHT_FULL_OFF; --light_val)
  {
    lcd_backlight_set(light_val);
    _delay_ms(40);
  }

  /* Leave backlight off for 1S
	  */
  delay(1000);

  /* Increase backlight from off to almost full brightness
      */
  for (light_val = BACKLIGHT_FULL_OFF; light_val < BACKLIGHT_FULL_ON; ++light_val)
  {
    lcd_backlight_set(light_val);
    _delay_ms(40);
  }
}
//=============================================================================
void dac_init(void)
{
  // Initialize the LTC2630-8 DAC. Set its reference to VCC
  // (+3.3VDC)
//  Serial.println(F("cmd1"));
  dac_cmd_send(DAC_POWER_DOWN, NULL_DATA);
//  Serial.println(F("cmd2"));
  dac_cmd_send(DAC_VCC_REF, NULL_DATA);
}
//=============================================================================
void dac_cmd_send(dac_cmd_t cmd, uint8_t data)
{
  /* Set DAC_CS low. Send DAC command, data byte followed by don't
      * care byte to DAC
      */
//  Serial.println(F("entering dac_cmd_send()"));
      
  CLR_DAC_CS;
#if ARDUINO_SPI == 1
  SPI.transfer(cmd);
  delay(1);
  SPI.transfer(data);
  delay(1);
  SPI.transfer(NULL_DATA);
  delay(1);
#else
  spi_master_send_byte(cmd);
  delay(1);
  spi_master_send_byte(data);
  delay(1);
  spi_master_send_byte(NULL_DATA);
  delay(1);
  #endif  

  /* Set DAC_CS high
      */
  SET_DAC_CS;
//  Serial.println(F("exiting dac_cmd_send()"));
}
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
void hw_reset(void)
{
  /* Set the LCD_RST low for 10 uS and then high again and return
      * after 10 uS. According to the ST7565P datasheet only 1 uS is
      * needed at VDD = 3.3VDC but it won't hurt to be conservative.
      */
  CLR_RESET;
  _delay_us(10);
  SET_RESET;
  _delay_us(10);
}
//=============================================================================
void setup(void)
{
  Serial.println(F("Initializing the CFAG4265A0"));

  //debug console
  Serial.begin(115200);
  Serial.println(F("Entering setup()"));

  //Thump the reset
  hw_reset();

  // Initialize the individual pieces
  Serial.println(F("gpio_init()"));
  gpio_init();
#if ARDUINO_SPI == 1
  // // Initialize SPI. By default the clock is 4MHz.
  SPI.begin();
  // //Bump the clock to 8MHz. Appears to be the maximum.
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
#else
  Serial.println(F("spi_init()"));
  spi_init();
#endif
  Serial.println(F("dac_init()"));
  dac_init();
  Serial.println(F("lcd_init()"));
  lcd_init();

  // Turn on the display and its backlight
  lcd_display_on();
  lcd_backlight_set(BACKLIGHT_FULL_ON);
}
//=============================================================================
void loop(void)
{
  uint8_t page;
  uint16_t i = 0;

  Serial.println(F("Loop()"));

  /* Display a fill pattern then clear
  */
  lcd_fill(0x3c, 3, 0);
  lcd_fill(0x00, 3, 0);
  delay(500);
  
  /* Display a fill pattern then clear
  */
  lcd_fill(0x03c, 0, 255);
  lcd_fill(0x00, 0, 255);
  delay(500);
  
  // Display the Crystalfontz CFAG4265 bitmap image.
  //
  // The bitmap2lcd utility generated the data with the image
  // shifted up 7 pixels from the bottom of the LCD. This is
  // handled prior to display of each byte by shifting the
  // byte from the current page down 7 pixels and ORing it
  // with the byte from the next page that is shifted up 1
  // pixel.

  for (i = 0, page = 0; page < 9; ++page)
  {
    lcd_page_address_set(page);
    lcd_column_address_set(0);
    for (uint8_t col = 0; col < 42; ++col)
    {
      uint8_t data1 = pgm_read_byte(&(cfag4265_logo[i]));
      uint8_t data2 = pgm_read_byte(&(cfag4265_logo[i + 42]));
      lcd_data_send((data1 >> 7) | (data2 << 1));
      ++i;
    }
  }

  /* Delay 4S, cycle the backlight
  * Delay 1.5S then clear the LCD
  */
  // delay(1000);
  // lcd_backlight_cycle();
  delay(1000);
  lcd_clear();
}
//=============================================================================
