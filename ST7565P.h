#ifndef __ST7565P_H_
#define __ST7565P_H_
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
//-----------------------------------------------------------------------------
//   ARD      | Port | LCD                      | Wire Color
// -----------+------+--------------------------+-----------
//  #8/D8     |  PB0 | LCD_A0                   | Green
//  #9/D9     |  PB1 | LCD_CS                   | Purple
// #10/D10    |  PB2 | DAC_CS                   | Red
// #11/D11    |  PB3 | HW_MOSI  (hardware SPI)  | Orange
// #12/D12    |  PB4 | LCD_RST  (hardware MISO) | Blue
// #13/D13    |  PB5 | HW_SCK   (hardware SPI)  | Yellow

// A0 = LCD_A0 | GREEN
#define LCD_A0 0x01
#define CLR_LCD_A0    (PORTB &= ~(LCD_A0))
#define SET_LCD_A0    (PORTB |= (LCD_A0))

// CS1 = LCD_CS | PURPLE
#define LCD_CS 0x02
#define CLR_LCD_CS    (PORTB &= ~(LCD_CS))
#define SET_LCD_CS    (PORTB |= (LCD_CS))

// MOSI
// #define HW_MOSI 0x08
// #define CLR_MOSI    (PORTB &= ~(HW_MOSI))
// #define SET_MOSI    (PORTB |= (HW_MOSI))

// HW_RESET | BLUE
#define HW_RESET 0x10
#define CLR_RESET    (PORTB &= ~(HW_RESET))
#define SET_RESET    (PORTB |= (HW_RESET))

// MISO | UNUSED
// #define HW_MISO 0x10
// #define CLR_MISO    (PORTB &= ~(HW_MISO))
// #define SET_MISO    (PORTB |= (HW_MISO))

// SCK
// #define HW_SCK 0x20
// #define CLR_SCK    (PORTB &= ~(HW_SCK))
// #define SET_SCK    (PORTB |= (HW_SCK))

#define NULL_DATA 0

//=============================================================================
// ST7565P Class definition
//-----------------------------------------------------------------------------
class ST7565P
{
public:
  void init(void);
  void adc_select(uint8_t direction);
  void all_points(uint8_t on);
  void bias_set(uint8_t bias);
  void booster_ratio_set(uint8_t ratio);
  void clear(void);
  void cmd_reset(void);
  void column_address_set(uint8_t column);
  void common_output_mode_select(uint8_t reverse);
  void display_on(void);
  void display_off(void);
  void display_reverse(uint8_t reverse);
  void electronic_volume_set(uint8_t contrast);
  void fill(uint8_t data, uint16_t delay_ms_col, uint16_t delay_ms_page);
  void page_address_set(uint8_t page);
  void power_controller_set(uint8_t boost, uint8_t vreg, uint8_t vfollower);
  void show_bitmap(void);
  void start_line_set(uint8_t line);
  void vo_regulator_resistor_set(uint8_t ratio);
  void write_command(uint8_t data);
  void write_data(uint8_t data);
};

#endif //__ST7565P_H_