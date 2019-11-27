//#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "wbFontB12.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
//#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define OLED_RESET LED_BUILTIN
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

uint8_t bits[]={0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
uint8_t fbuf[18], _cx, _cy, i, j, count;
uint16_t _color1, _color2;

void showbits(uint8_t c, uint8_t n)
{
  for(int i=0; i<4; i++, _cx++) 
    if (c & bits[i+n]) oled.drawPixel(_cx, _cy, _color1);
    else oled.drawPixel(_cx, _cy, 0); // _color2);
}

void dispfont(uint16_t n)
{
  memcpy_P(fbuf, (uint8_t *)(&b5f12[n*18]), 18);
  count = 0;
  for(int i=0; i<18; i++) {
    showbits(fbuf[i], 0);
    if (++count % 3 == 0) { _cx-=12; _cy++; } 
    showbits(fbuf[i], 4);
    if (++count % 3 == 0) { _cx-=12; _cy++; } 
  }
}

void setup() {
  Serial.begin(115200);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for x78 on-board, 0x3D for x7A
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  oled.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  oled.clearDisplay();

  // Draw a single pixel in white
  oled.drawPixel(10, 10, WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  oled.display();
  delay(2000);
}

void loop() {
  _cx=(rand()%9)*14;
  _cy=(rand()%10)*16;
  _color1=(rand()%65536)|0x8410;
  //_color2=(rand()%65536)&0xfbef;
  dispfont(rand()%5401);
  oled.display();
  oled(2000);
}
