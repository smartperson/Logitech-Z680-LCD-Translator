#ifndef NHD_0220CW_AX3_h
#define NHD_0220CW_AX3_h

#include <stdint.h>
#include <stdbool.h>
#include <Arduino.h>
#define uint8_t byte
static const byte ROW_N = 2;                 // Number of display rows
static const byte COLUMN_N = 20;             // Number of display columns

static const byte DC = 8;                    // Arduino's pin assigned to the D/C line
//const byte RW = 13;                 // Arduino's pin assigned to the R/W line (optional, can be always low)
static const byte E = 7;                     // Arduino's pin assigned to the E line
//const byte CS = 12;                 // Arduino's pin assigned to the /CS line (optional, can be always low)
//const byte RES = 13;                // Arduino's pin assigned to the Reset line (optional, can be always high)
static const byte DATA_PINS[4] = {5, 4, 3, 2};                  // Arduino's pins assigned to the data bus

static const char TEXT[4][21] = {"1-Newhaven Display--", 
                          "2-------Test--------", 
                          "3-16/20-Characters--", 
                          "4!@#$%^&*()_+{}[]<>?"};         // Strings to be displayed

static byte new_line[4] = {0x80, 0xA0, 0xC0, 0xE0};               // DDRAM address for each line of the display
static byte nhd_rows = 0x08;                     // Display mode: 1/3 lines or 2/4 lines; default 2/4 (0x08)

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void nhd_setup(void);
void nhd_output(void);
void nhd_blocks(void);
void nhd_command(byte c);
void nhd_data(byte d);
void nhd_enableCycle(void);
void nhd_send4bit(byte value);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#include <inttypes.h>
#include "Print.h"

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

class NhdOledDisplay : public Print {
public:
  NhdOledDisplay(uint8_t rs, uint8_t enable,
		uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
		uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
  NhdOledDisplay(uint8_t rs, uint8_t rw, uint8_t enable,
		uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
		uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
  NhdOledDisplay(uint8_t rs, uint8_t rw, uint8_t enable,
		uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);
  NhdOledDisplay(uint8_t rs, uint8_t enable,
		uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);

  void init(uint8_t fourbitmode, uint8_t rs, uint8_t rw, uint8_t enable,
	    uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
	    uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
    
  void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);

  void clear();
  void home();

  void noDisplay();
  void display();
  void noBlink();
  void blink();
  void noCursor();
  void cursor();
  void scrollDisplayLeft();
  void scrollDisplayRight();
  void leftToRight();
  void rightToLeft();
  void autoscroll();
  void noAutoscroll();

  void setRowOffsets(int row1, int row2, int row3, int row4);
  void createChar(uint8_t, uint8_t[]);
  void setCursor(uint8_t, uint8_t); 
  virtual size_t write(uint8_t);
  void command(uint8_t);
  
  using Print::write;
private:
  void send(uint8_t, uint8_t);
  void write4bits(uint8_t);
  void write8bits(uint8_t);
  void pulseEnable();

  uint8_t _rs_pin; // LOW: command.  HIGH: character.
  uint8_t _rw_pin; // LOW: write to LCD.  HIGH: read from LCD.
  uint8_t _enable_pin; // activated by a HIGH pulse.
  uint8_t _data_pins[8];

  uint8_t _displayfunction;
  uint8_t _displaycontrol;
  uint8_t _displaymode;

  uint8_t _initialized;

  uint8_t _numlines;
  uint8_t _row_offsets[4];
};

#endif
