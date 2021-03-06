//  Author: avishorp@gmail.com
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA



#if(ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

/*///extern "C" {
  #include <stdlib.h>
  #include <string.h>
  #include <inttypes.h>
//}
*/
#include <Arduino.h>
#include "TM1637Display.h"

#define TM1637_I2C_COMM1    0x40
#define TM1637_I2C_COMM2    0xC0
#define TM1637_I2C_COMM3    0x80

uint8_t ack=0;


static uint8_t m_pinClk=0;
static uint8_t m_pinDIO=0;
static uint8_t m_brightness;



////////////// Public Methods ////////////////////////////////////////

const uint8_t digitToSegment[] = {
 // XGFEDCBA
  0b00111111,    // 0
  0b00000110,    // 1
  0b01011011,    // 2
  0b01001111,    // 3
  0b01100110,    // 4
  0b01101101,    // 5
  0b01111101,    // 6
  0b00000111,    // 7
  0b01111111,    // 8
  0b01101111,    // 9
  0b01110111,    // A
  0b01111100,    // b
  0b00111001,    // C
  0b01011110,    // d
  0b01111001,    // E
  0b01110001     // F
  };



void TM1637_Display(uint8_t pinClk, uint8_t pinDIO)
{
	// Copy the pin numbers
	m_pinClk = pinClk;
	m_pinDIO = pinDIO;

	// Set the pin direction and default value.
	// Both pins are set as inputs, allowing the pull-up resistors to pull them up
    pinMode(m_pinClk, INPUT);
    pinMode(m_pinDIO,INPUT);
	digitalWrite(m_pinClk, LOW);
	digitalWrite(m_pinDIO, LOW);
}

void TM1637_setBrightness(uint8_t brightness, bool on)
{
	m_brightness = (brightness & 0x7) | (on? 0x08 : 0x00);
}




void TM1637_start()
{
  pinMode(m_pinDIO, OUTPUT);
  TM1637_bitDelay();
}

void TM1637_stop()
{
	pinMode(m_pinDIO, OUTPUT);
	TM1637_bitDelay();
	pinMode(m_pinClk, INPUT);
	TM1637_bitDelay();
	pinMode(m_pinDIO, INPUT);
	TM1637_bitDelay();
}

bool TM1637_writeByte(uint8_t b)
{
  uint8_t data = b;

  // 8 Data Bits
  for(uint8_t i = 0; i < 8; i++) {
    // CLK low
    pinMode(m_pinClk, OUTPUT);
    TM1637_bitDelay();

	// Set data bit
    if (data & 0x01)
      pinMode(m_pinDIO, INPUT);
    else
      pinMode(m_pinDIO, OUTPUT);

    TM1637_bitDelay();

	// CLK high
    pinMode(m_pinClk, INPUT);
    TM1637_bitDelay();
    data = data >> 1;
  }

  // Wait for acknowledge
  // CLK to zero
  pinMode(m_pinClk, OUTPUT);
  pinMode(m_pinDIO, INPUT);
  TM1637_bitDelay();

  // CLK to high
  pinMode(m_pinClk, INPUT);
  TM1637_bitDelay();
   ack = digitalRead(m_pinDIO);
  if (ack == 0)
    pinMode(m_pinDIO, OUTPUT);


  TM1637_bitDelay();
  pinMode(m_pinClk, OUTPUT);
  TM1637_bitDelay();

  return ack;
}


void TM1637_setSegments(const uint8_t segments[], uint8_t length, uint8_t pos)
{
    // Write COMM1
	TM1637_start();
	TM1637_writeByte(TM1637_I2C_COMM1);
	TM1637_stop();

	// Write COMM2 + first digit address
	TM1637_start();
	TM1637_writeByte(TM1637_I2C_COMM2 + (pos & 0x03));

	// Write the data bytes
	for (uint8_t k=0; k < length; k++)
	  TM1637_writeByte(segments[k]);

	TM1637_stop();

	// Write COMM3 + brightness
	TM1637_start();
	TM1637_writeByte(TM1637_I2C_COMM3 + (m_brightness & 0x0f));
	TM1637_stop();
}

void TM1637_showNumberDec(int num, bool leading_zero, uint8_t length, uint8_t pos)
{
  TM1637_showNumberDecEx(num, 0, leading_zero, length, pos);
}

void TM1637_showNumberDecEx(int num, uint8_t dots, bool leading_zero,
                                    uint8_t length, uint8_t pos)
{
  uint8_t digits[4];
	const static int divisors[] = { 1, 10, 100, 1000 };
	bool leading = true;

	for(int8_t k = 0; k < 4; k++) {
	    int divisor = divisors[4 - 1 - k];
		int d = num / divisor;
    uint8_t digit = 0;

		if (d == 0) {
		  if (leading_zero || !leading || (k == 3))
		      digit = TM1637_encodeDigit(d);
	      else
		      digit = 0;
		}
		else {
			digit = TM1637_encodeDigit(d);
			num -= d * divisor;
			leading = false;
		}
    
    // Add the decimal point/colon to the digit
    digit |= (dots & 0x80); 
    dots <<= 1;
    
    digits[k] = digit;
	}

	TM1637_setSegments(digits + (4 - length), length, pos);
}

void TM1637_bitDelay()
{
	delayMicroseconds(50);
}


uint8_t TM1637_encodeDigit(uint8_t digit)
{
	return digitToSegment[digit & 0x0f];
}


