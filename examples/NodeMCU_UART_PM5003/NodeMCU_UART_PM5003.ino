//
// software Serial to read PM2.5 sensor (PM5003)
//

// NodeMCU user software Serial to support Serial1
#include <SoftwareSerial.h>
#define RX_PIN D7 // connect to PM5003-TXD
#define TX_PIN D8 // connect to PM5003-RXD
SoftwareSerial swSerial(RX_PIN, TX_PIN, false, 128);

/*
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
//#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define OLED_RESET LED_BUILTIN
Adafruit_SSD1306 lcd(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
*/
long pmcf10=0;
long pmcf25=0;
long pmcf100=0;
long pmat10=0;
long pmat25=0;
long pmat100=0;
char buf[50];

void setup() {
  Serial.begin(115200);
  swSerial.begin(9600); // for PM5003 PM2.5 sensor
}

void loop() {
  // put your main code here, to run repeatedly:
  int count = 0;
  unsigned char c;
  unsigned char high;
  Serial.println();
  Serial.println("reading PM5003..."); 
  while (swSerial.available()) {
    c = swSerial.read();
    if((count==0 && c!=0x42) || (count==1 && c!=0x4d)){
      Serial.println("check failed");
//      lcd.setCursor(0,0);
//      lcd.print(String("check failed "));
      break;
    }

    if(count > 15){
      Serial.println("complete");
      break;
    }
    else if(count == 4 || count == 6 || count == 8 || count == 10 || count == 12 || count == 14) {
      high = c;
    }
    else if(count == 5){
      pmcf10 = 256*high + c;
      Serial.print("CF=1, PM1.0=");
      Serial.print(pmcf10);
      Serial.println(" ug/m3");
//      lcd.clear();
//      lcd.setCursor(0,0);
//      lcd.print(String("CF ")+pmcf10 + String(","));
    }
    else if(count == 7){
      pmcf25 = 256*high + c;
      Serial.print("CF=1, PM2.5=");
      Serial.print(pmcf25);
      Serial.println(" ug/m3");
//      lcd.setCursor(8,0);
//      lcd.print(pmcf25 + String(","));
    }
    else if(count == 9){
      pmcf100 = 256*high + c;
      Serial.print("CF=1, PM10=");
      Serial.print(pmcf100);
      Serial.println(" ug/m3");
//      lcd.setCursor(12,0);
//      lcd.print(pmcf100);
    }
    else if(count == 11){
      pmat10 = 256*high + c;
      Serial.print("atmosphere, PM1.0=");
      Serial.print(pmat10);
      Serial.println(" ug/m3");
//      lcd.setCursor(0,1);
//      lcd.print(String("At ")+pmat10 + String(","));
    }
    else if(count == 13){
      pmat25 = 256*high + c;
      Serial.print("atmosphere, PM2.5=");
      Serial.print(pmat25);
      Serial.println(" ug/m3");
//      lcd.setCursor(8,1);
//      lcd.print(pmat25 + String(","));
    }
    else if(count == 15){
      pmat100 = 256*high + c;
      Serial.print("atmosphere, PM10=");
      Serial.print(pmat100);
      Serial.println(" ug/m3");
//      lcd.setCursor(12,1);
//      lcd.print(pmat100);
    }
    count++;
  }
  
  while(swSerial.available()) swSerial.read();
  delay(5000); 
}
