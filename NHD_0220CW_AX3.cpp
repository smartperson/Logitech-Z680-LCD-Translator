/*
 * Demo_NHD0420CW-Ax3_parallel_6800_4b.ino
 * 
 * Tutorial sketch for use of character OLED slim display family by Newhaven with Arduino Uno, without 
 * using any library.  Models: NHD0420CW-Ax3, NHD0220CW-Ax3, NHD0216CW-Ax3. Controller: US2066
 * In this example, the display is connected to Arduino via 8-bit 6800 parallel interface.
 *
 * Displays on the OLED alternately a 4-line message and a sequence of character "block".
 * This sketch assumes the use of a 4x20 display; if different, modify the values of the two variables 
 * ROW_N e COLUMN_N.
 * The sketch uses the minimum possible of Arduino's pins; if you intend to use also R/W, /CS or /RES 
 * lines, the related instructions are already present, it's sufficient to remove the comment markers.
 *
 * The circuit:
 * OLED pin 1 (Vss)    to Arduino pin ground
 * OLED pin 2 (VDD)    to Arduino pin 5V
 * OLED pin 3 (REGVDD) to Arduino pin 5V
 * OLED pin 4 (D/C)    to Arduino pin D2
 * OLED pin 5 (R/W)    to Vss ground (always write); to enable also read, connect to Arduino pin D13
 * OLED pin 6 (E)      to Arduino pin D3
 * OLED pin 7 (DB0)    to Vss ground
 * OLED pin 8 (DB1)    to Vss ground
 * OLED pin 9 (DB2)    to Vss ground
 * OLED pin 10 (DB3)   to Vss ground
 * OLED pin 11 (DB4)   to Arduino pin D8
 * OLED pin 12 (DB5)   to Arduino pin D9
 * OLED pin 13 (DB6)   to Arduino pin D10
 * OLED pin 14 (DB7)   to Arduino pin D11
 * OLED pin 15 (/CS)   to Vss ground  (or to Arduino pin D12, in case of use of more than one display)
 * OLED pin 16 (/RES)  to Arduino pin Reset or VDD 5V (or to Arduino pin D13, to control reset by sw)
 * OLED pin 17 (BS0)   to VDD 5V
 * OLED pin 18 (BS1)   to Vss ground
 * OLED pin 19 (BS2)   to VDD 5V
 * OLED pin 20 (Vss)   to Vss ground
 *
 * Original example created by Newhaven Display International Inc.
 * Modified and adapted to Arduino Uno 16 Mar 2015 by Pasquale D'Antini
 * Modified 19 May 2015 by Pasquale D'Antini
 *
 * This example code is in the public domain.
 */

#include <stdint.h>
#include <stdbool.h>
#include <Arduino.h>

#ifndef NHD_0220CW_AX3
#define NHD_0220CW_AX3
#include "NHD_0220CW_AX3.h"

// _______________________________________________________________________________________

void nhd_command(byte c)                  // SUBROUTINE: PREPARES THE TRANSMISSION OF A COMMAND
{
   digitalWrite(DC, LOW);             // Sets LOW the D/C line of the display -> command
//   digitalWrite(RW, LOW);           // Sets LOW the R/W line of the display (optional, can be always low)
   nhd_send4bit(c >> 4);                  // Sends the higher 4 bits on the data bus
   nhd_enableCycle();                     // Calls the enable signal cycle subroutine
   nhd_send4bit(c);                       // Sends the lower 4 bits on the data bus
   nhd_enableCycle();                     // Calls the enable signal cycle subroutine
}
// _______________________________________________________________________________________

void nhd_data(byte d)                     // SUBROUTINE: PREPARES THE TRANSMISSION OF A BYTE OF DATA
{
   digitalWrite(DC, HIGH);            // Sets HIGH the D/C line of the display -> data
//   digitalWrite(RW, LOW);           // Sets LOW the R/W line of the display (optional, can be always low)
   nhd_send4bit(d >> 4);                  // Sends the higher 4 bits on the data bus
   nhd_enableCycle();                     // Calls the enable signal cycle subroutine
   nhd_send4bit(d);                       // Sends the lower 4 bits on the data bus
   nhd_enableCycle();                     // Calls the enable signal cycle subroutine
}
// _______________________________________________________________________________________

void nhd_enableCycle(void)                // SUBROUTINE: EXECUTE THE ENABLE SIGNAL CYCLE (DATA LATCH)
{
   delayMicroseconds(1);              // Waits 1 us (required for timing purpose)
//   digitalWrite(CS, LOW);           // Sets LOW the /CS line of the display (optional, can be always low)
   digitalWrite(E, HIGH);             // Sets HIGH the E line of the display
   delayMicroseconds(1);              // Waits 1 us (required for timing purpose)
   digitalWrite(E, LOW);              // Sets LOW the E line of the display
//   digitalWrite(CS, HIGH);          // Sets HIGH the /CS line of the display (optional, can be always low)
   delayMicroseconds(1);              // Waits 1 us (required for timing purpose)
}
// _______________________________________________________________________________________

void nhd_send4bit(byte value)             // SUBROUTINE: SENDS HALF BYTE ON THE DATA BUS
{
   for (byte i = 0; i < 4; i++)                          // One bit at a time,
   {
      digitalWrite(DATA_PINS[i], (value >> i) & 0x01);   //  sets the four lines of the data bus, 
   }                                                     //  to send the half character to the display
}
// _______________________________________________________________________________________

void nhd_output(void)                     // SUBROUTINE: DISPLAYS THE FOUR STRINGS, THEN THE SAME IN REVERSE ORDER
{
   byte r = 0;                        // Row index
   byte c = 0;                        // Column index

   nhd_command(0x01);                     // Clears display (and cursor home)
   delay(2);                          // After a clear display, a minimum pause of 1-2 ms is required
   
   for (r=0; r<ROW_N; r++)            // One row at a time,
   {
      nhd_command(new_line[r]);           //  moves the cursor to the first column of that line
      for (c=0; c<COLUMN_N; c++)      // One character at a time, 
      {
         nhd_data(TEXT[r][c]);            //  displays the correspondig string
      }
   }

   delay(2000);                       // Waits, only for visual effect purpose
   
   for (r=0; r<ROW_N; r++)            // One row at a time,
   {
      nhd_command(new_line[r]);           //  moves the cursor to the first column of that line
      for (c=0; c<COLUMN_N; c++)      // One character at a time, 
      {
         nhd_data(TEXT[3-r][c]);          //  displays the correspondig string (in reverse order)
      }
   }
}
// _______________________________________________________________________________________

void nhd_blocks(void)                     // SUBROUTINE: FILLS THE ENTIRE DISPLAY WITH THE CHARACTER "BLOCK"
{
   byte r = 0;                        // Row index
   byte c = 0;                        // Column index

   nhd_command(0x01);                     // Clear display (and cursor home)
   delay(2);                          // After a clear display, a minimum pause of 1-2 ms is required

   for (r=0; r<ROW_N; r++)            // One row at a time,
   {
      nhd_command(new_line[r]);           //  moves the cursor to the first column of that line
      for (c=0; c<COLUMN_N; c++)      // One character at a time, 
      {
         nhd_data(0xDB);                  //  displays the character 0xDB (block)
         delay(50);                   // Waits, only for visual effect purpose
      }
      delay(500);                     // Waits, only for visual effect purpose
   }
}
// _______________________________________________________________________________________

#endif

// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LiquidCrystal constructor is called).

NhdOledDisplay::NhdOledDisplay(uint8_t rs, uint8_t rw, uint8_t enable,
			     uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
			     uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
  init(0, rs, rw, enable, d0, d1, d2, d3, d4, d5, d6, d7);
}

NhdOledDisplay::NhdOledDisplay(uint8_t rs, uint8_t enable,
			     uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
			     uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
  init(0, rs, 255, enable, d0, d1, d2, d3, d4, d5, d6, d7);
}

NhdOledDisplay::NhdOledDisplay(uint8_t rs, uint8_t rw, uint8_t enable,
			     uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
{
  init(1, rs, rw, enable, d0, d1, d2, d3, 0, 0, 0, 0);
}

NhdOledDisplay::NhdOledDisplay(uint8_t rs,  uint8_t enable,
			     uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
{
  init(1, rs, 255, enable, d0, d1, d2, d3, 0, 0, 0, 0);
}

void NhdOledDisplay::init(uint8_t fourbitmode, uint8_t rs, uint8_t rw, uint8_t enable,
			 uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
			 uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
  _rs_pin = rs;
  _rw_pin = rw;
  _enable_pin = enable;
  
  _data_pins[0] = d0;
  _data_pins[1] = d1;
  _data_pins[2] = d2;
  _data_pins[3] = d3; 
  _data_pins[4] = d4;
  _data_pins[5] = d5;
  _data_pins[6] = d6;
  _data_pins[7] = d7; 

  if (fourbitmode)
    _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
  else 
    _displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;

  begin(20, 2);  
}

void NhdOledDisplay::begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
  if ((dotsize != LCD_5x8DOTS) && (lines == 1)) {
    _displayfunction |= LCD_5x10DOTS;
  }

  pinMode(_rs_pin, OUTPUT);
  // we can save 1 pin by not using RW. Indicate by passing 255 instead of pin#
  if (_rw_pin != 255) { 
    pinMode(_rw_pin, OUTPUT);
  }
  pinMode(_enable_pin, OUTPUT);
  
  // Do these once, instead of every time a character is drawn for speed reasons.
  for (int i=0; i<((_displayfunction & LCD_8BITMODE) ? 8 : 4); ++i)
  {
    pinMode(_data_pins[i], OUTPUT);
  } 

  // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
  // according to datasheet, we need at least 40ms after power rises above 2.7V
  // before sending commands. Arduino can turn on way before 4.5V so we'll wait 50
  delayMicroseconds(50000); 
  // Now we pull both RS and R/W low to begin commands
  digitalWrite(_rs_pin, LOW);
  digitalWrite(_enable_pin, LOW);
  if (_rw_pin != 255) { 
    digitalWrite(_rw_pin, LOW);
  }

  _numlines = lines;
  byte rows;
  /* clean up and genericize this */
  if (lines == 2 || lines == 4)
    rows = 0x08;                    // Display mode: 2/4 lines
  else
    rows = 0x00;                    // Display mode: 1/3 lines
   
  command(0x22 | rows); // Function set: extended command set (RE=1), lines #
  command(0x71);        // Function selection A:
  write((byte)0x5C);    //  enable internal Vdd regulator at 5V I/O mode (def. value) (0x00 for disable, 2.8V I/O)
  command(0x20 | rows); // Function set: fundamental command set (RE=0) (exit from extended command set), lines #
  command(0x08);        // Display ON/OFF control: display off, cursor off, blink off (default values)
  command(0x22 | rows); // Function set: extended command set (RE=1), lines #
  command(0x79);        // OLED characterization: OLED command set enabled (SD=1)
  command(0xD5);        // Set display clock divide ratio/oscillator frequency:
  command(0x70);        //  divide ratio=1, frequency=7 (default values)
  command(0x78);        // OLED characterization: OLED command set disabled (SD=0) (exit from OLED command set)
  
  if (lines > 2)
    command(0x09);     // Extended function set (RE=1): 5-dot font, B/W inverting disabled (def. val.), 3/4 lines
  else
    command(0x08);     // Extended function set (RE=1): 5-dot font, B/W inverting disabled (def. val.), 1/2 lines
  
  command(0x06);        // Entry Mode set - COM/SEG direction: COM0->COM31, SEG99->SEG0 (BDC=1, BDS=0)
  command(0x72);        // Function selection B:
  write((byte)0x00);    //  ROM/CGRAM selection: ROM A, CGROM=240, CGRAM=8 (ROM=00, OPR=00)
  command(0x79);        // OLED characterization: OLED command set enabled (SD=1)
  command(0xDA);        // Set SEG pins hardware configuration:
  command(0x10);        //  alternative odd/even SEG pin, disable SEG left/right remap (default values)
  command(0xDC);        // Function selection C:
  command(0x00);        //  internal VSL, GPIO input disable
  command(0x81);        // Set contrast control:
  command(0x7F);        //  contrast=127 (default value)
  command(0xD9);        // Set phase length:
  command(0xF1);        //  phase2=15, phase1=1 (default: 0x78)
  command(0xDB);        // Set VCOMH deselect level:
  command(0x40);        //  VCOMH deselect level=1 x Vcc (default: 0x20=0,77 x Vcc)
  command(0x78);        // OLED characterization: OLED command set disabled (SD=0) (exit from OLED command set)
  command(0x20 | rows); // Function set: fundamental command set (RE=0) (exit from extended command set), lines #
  command(0x01);        // Clear display
  delay(2);             // After a clear display, a minimum pause of 1-2 ms is required
  command(0x80);        // Set DDRAM address 0x00 in address counter (cursor home) (default value)
  command(0x0C);        // Display ON/OFF control: display ON, cursor off, blink off
  delay(250);           // Waits 250 ms for stabilization purpose after display on
   
  if (lines == 2)
    new_line[1] = 0xC0;             // DDRAM address for each line of the display (only for 2-line mode)
  /* end of clean-up and genericize region */

  setRowOffsets(0x00, 0x40, 0x00 + cols, 0x40 + cols);  

  // finally, set # lines, font size, etc.
  // command(LCD_FUNCTIONSET | _displayfunction);  

  // turn the display on with no cursor or blinking default
  // _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;  
  /*display();

  // clear it off
  clear();

  // Initialize to default text direction (for romance languages)
  _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  // set the entry mode
  command(LCD_ENTRYMODESET | _displaymode);*/

}

void NhdOledDisplay::setRowOffsets(int row0, int row1, int row2, int row3)
{
  _row_offsets[0] = row0;
  _row_offsets[1] = row1;
  _row_offsets[2] = row2;
  _row_offsets[3] = row3;
}

/********** high level commands, for the user! */
void NhdOledDisplay::clear()
{
  command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
  delayMicroseconds(2000);  // this command takes a long time!
}

void NhdOledDisplay::home()
{
  command(LCD_RETURNHOME);  // set cursor position to zero
  delayMicroseconds(2000);  // this command takes a long time!
}

void NhdOledDisplay::setCursor(uint8_t col, uint8_t row)
{
  const size_t max_lines = sizeof(_row_offsets) / sizeof(*_row_offsets);
  if ( row >= max_lines ) {
    row = max_lines - 1;    // we count rows starting w/0
  }
  if ( row >= _numlines ) {
    row = _numlines - 1;    // we count rows starting w/0
  }
  
  command(LCD_SETDDRAMADDR | (col + _row_offsets[row]));
}

// Turn the display on/off (quickly)
void NhdOledDisplay::noDisplay() {
  _displaycontrol &= ~LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void NhdOledDisplay::display() {
  _displaycontrol |= LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void NhdOledDisplay::noCursor() {
  _displaycontrol &= ~LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void NhdOledDisplay::cursor() {
  _displaycontrol |= LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void NhdOledDisplay::noBlink() {
  _displaycontrol &= ~LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void NhdOledDisplay::blink() {
  _displaycontrol |= LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void NhdOledDisplay::scrollDisplayLeft(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void NhdOledDisplay::scrollDisplayRight(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void NhdOledDisplay::leftToRight(void) {
  _displaymode |= LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void NhdOledDisplay::rightToLeft(void) {
  _displaymode &= ~LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void NhdOledDisplay::autoscroll(void) {
  _displaymode |= LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void NhdOledDisplay::noAutoscroll(void) {
  _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void NhdOledDisplay::createChar(uint8_t location, uint8_t charmap[]) {
  location &= 0x7; // we only have 8 locations 0-7
  command(LCD_SETCGRAMADDR | (location << 3));
  for (int i=0; i<8; i++) {
    write(charmap[i]);
  }
}

/*********** mid level commands, for sending data/cmds */

inline void NhdOledDisplay::command(uint8_t value) {
  send(value, LOW);
}

inline size_t NhdOledDisplay::write(uint8_t value) {
  send(value, HIGH);
  return 1; // assume sucess
}

/************ low level data pushing commands **********/

// write either command or data, with automatic 4/8-bit selection
void NhdOledDisplay::send(uint8_t value, uint8_t mode) {
  digitalWrite(_rs_pin, mode);

  // if there is a RW pin indicated, set it low to Write
  if (_rw_pin != 255) { 
    digitalWrite(_rw_pin, LOW);
  }
  
  if (_displayfunction & LCD_8BITMODE) {
    write8bits(value); 
  } else {
    write4bits(value>>4);
    write4bits(value);
  }
}

void NhdOledDisplay::pulseEnable(void) {
  digitalWrite(_enable_pin, LOW);
  delayMicroseconds(1);    
  digitalWrite(_enable_pin, HIGH);
  delayMicroseconds(1);    // enable pulse must be >450ns
  digitalWrite(_enable_pin, LOW);
  delayMicroseconds(50);   // commands need > 37us to settle
}

void NhdOledDisplay::write4bits(uint8_t value) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(_data_pins[i], (value >> i) & 0x01);
  }

  pulseEnable();
}

void NhdOledDisplay::write8bits(uint8_t value) {
  for (int i = 0; i < 8; i++) {
    digitalWrite(_data_pins[i], (value >> i) & 0x01);
  }
  
  pulseEnable();
}
