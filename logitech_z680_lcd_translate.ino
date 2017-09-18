// Written by Varun Mehta
// September 2017
// Based on SPI Slave Code by Nick Gammon
// February 2011

#include <SPI.h>
#include <LiquidCrystal.h>
#include "ringbuf.h"

char buf [384];
volatile byte pos;
volatile boolean process_it;
// initialize the library with the numbers of the interface pins
// RS, E, D4, D5, D6, D7
LiquidCrystal lcd(2, 3, 8, 7, 6, 5);
RING_BUFFER fifo;

void setup ()
{
  Ringbuf_Init(&fifo, buf, 3, 128);
  Serial.begin (115200);   // debugging

  // have to send on master in, *slave out*
  pinMode(MOSI, INPUT);
  pinMode(SCK, INPUT);
  pinMode(SS, INPUT);
  pinMode(MISO, OUTPUT);
  
  // turn on SPI in slave mode
  SPCR |= _BV(SPE);
  SPCR |= _BV(DORD);
  SPCR |= _BV(CPOL); //clock defaults high
  SPCR |= _BV(CPHA); // setup on leading edge, sample on trailing
  // Serial.println(SPCR, HEX);
  SPI.setClockDivider(SPI_CLOCK_DIV128);
  // Serial.println(SPCR, HEX);
  // Serial.print("\e[?25l");
  // get ready for an interrupt 
  pos = 0;   // buffer empty
  process_it = false;

  // now turn on interrupts
  SPI.attachInterrupt();

  // set up the LCD's number of columns and rows:
   lcd.begin(20, 4);
  // Print a message to the LCD.
  // lcd.print("hello, world!");
}  // end of setup

byte receiveBuf[3];
// SPI interrupt routine
ISR (SPI_STC_vect) {
  byte c = SPDR;  // grab byte from SPI Data Register

  // add to buffer if room
  receiveBuf[pos++] = c;
  if (pos %3 == 0) {
    pos = 0;
    process_it = true;
    Ringbuf_Put(&fifo, receiveBuf);
  }
}  // end of interrupt routine SPI_STC_vect

// main loop - wait for flag set in interrupt routine
void loop (void) {
  byte *sendBuf;
  if(!Ringbuf_Empty(&fifo)) {
    int count = 0;
    for(count = 0; !Ringbuf_Empty(&fifo); count++) {
      sendBuf = (byte*)Ringbuf_Pop_Front(&fifo);
      byte payload = (sendBuf[1] & 0x0F) | ((sendBuf[2] & 0x0F) << 4);
      if (sendBuf[0] == 0x1F) {
        Serial.print("CMD: ");
        Serial.println(payload, HEX);
        switch (payload) {
          case 0x01:
            // lcd.command(payload);
            Serial.print("\e[2J");
            lcd.clear();
            break;
          case 0x02:
            // lcd.command(payload);
            Serial.print("\e[1;1f");
            lcd.home();
            break;
          case 0x40:
            Serial.print("\e[2J");
            lcd.command(payload);
            break;
          case 0x80: //serial LCD uses this for CGRAM, hd44780 does not
            lcd.command(0x40);
            // delayMicroseconds(20);
            break;
          case 0x0c: //turns on LCD, disables cursor
            break; //lcd.noCursor();
          case 0x00: //does nothing
            break;
          default:
            if (payload & B10000000) { // move cursor
              byte position = payload & B01111111;
              byte row = position/20;
              byte column = position - (row)*20;
              // if (row == 2) row = 3;
              // lcd.command(payload);
              Serial.print("\e[");
              Serial.print(row, DEC);
              Serial.print(";");
              Serial.print(column, DEC);
              Serial.print("f");
              lcd.setCursor(column, row);
            }
            break;
        }
      }
      else { //data for screen
        char character = payload;
        if (payload <= 0x08) // special chars we can't show on terminal
          character = payload+0x2C;
          Serial.print(character);
          // Serial.print("DAT: ");
          // Serial.println(payload, HEX);

          lcd.write(payload);
      }
      // Serial.print(sendBuf[0], HEX);Serial.print(" ");Serial.print(sendBuf[1], HEX);Serial.print(" ");Serial.println(sendBuf[2], HEX);
    }
  }      
}

