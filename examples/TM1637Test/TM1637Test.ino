#include <TM1637Display.h>
#define CLK 13
#define DIO 14


#define TEST_DELAY   2000

const uint8_t SEG_DONE[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
  SEG_C | SEG_E | SEG_G,                           // n
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
  };

void setup() {
  // put your setup code here, to run once:
TM1637_Display(CLK, DIO);

}

void loop() {
  // put your main code here, to run repeatedly:
int k;
  bool lz = false;
  uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };
  TM1637_setBrightness(0x0f,1);
   // All segments on
  
  TM1637_setSegments(data,4,0);
  delay(TEST_DELAY);

  data[0] = 0b01001001;
  data[1] = TM1637_encodeDigit(1);
  data[2] = TM1637_encodeDigit(2);
  data[3] = TM1637_encodeDigit(3);

  for(k = 3; k >= 0; k--) {
  TM1637_setSegments(data, 1, k);
  delay(TEST_DELAY);
  }

  TM1637_setSegments(data+2, 2, 2);
  delay(TEST_DELAY);

  TM1637_setSegments(data+2, 2, 1);
  delay(TEST_DELAY);

  TM1637_setSegments(data+1, 3, 1);
  delay(TEST_DELAY);

lz=false;
  for (uint8_t z = 0; z < 2; z++) {
  for(k = 0; k < 10000; k += k*4 + 7) {
   // TM1637_showNumberDec(k,0,lz,uint8_t length = 4,uint8_t pos = 0);
    TM1637_showNumberDec(k, lz, 4,0);
    delay(TEST_DELAY);
  }
  lz = true;
  }
for(k = 0; k < 4; k++)
  data[k] = 0;
  TM1637_setSegments(data,4,0);

  // Run through all the dots
  for(k=0; k <= 4; k++) {
    TM1637_showNumberDecEx(0, (0x80 >> k), true,4,0);
    delay(TEST_DELAY);
  }
TM1637_showNumberDec(153, false, 3, 1);
  delay(TEST_DELAY);
  TM1637_showNumberDec(22, false, 2, 2);
  delay(TEST_DELAY);
  TM1637_showNumberDec(0, true, 1, 3);
  delay(TEST_DELAY);
  TM1637_showNumberDec(0, true, 1, 2);
  delay(TEST_DELAY);
  TM1637_showNumberDec(0, true, 1, 1);
  delay(TEST_DELAY);
  TM1637_showNumberDec(0, true, 1, 0);
  delay(TEST_DELAY);

  // Brightness Test
  for(k = 0; k < 4; k++)
  data[k] = 0xff;
  for(k = 0; k < 7; k++) {
   TM1637_setBrightness(k,1);
    TM1637_setSegments(data,4,0);
    delay(TEST_DELAY);
  }

   for(k = 0; k < 4; k++) {
    TM1637_setBrightness(7, false);  // Turn off
    TM1637_setSegments(data,4,0);
    delay(TEST_DELAY);
    TM1637_setBrightness(7, true); // Turn on
    TM1637_setSegments(data,4,0);
    delay(TEST_DELAY);  

      // Done!
 TM1637_setSegments(SEG_DONE,4,0);

  while(1);
  }

}
