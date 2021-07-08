// ESP32 control LED5050 strip : Blue-Red-Green-Black(12V)
#define pinBLUE  27
#define pinRED   14
#define pinGREEN 12

// PWM channel no.  
#define chRED   0
#define chGREEN 1
#define chBLUE  2

#define   PWMFREQ 1000
#define PWMRESOLUTION 8 // 8-bit resolution
#define PWMMAXPOWER 255

#define ledTime 20
#define colorTime 2000

void setup() {
  Serial.begin(115200);
  
  // setup PWM pin and its channel
  pinMode(pinRED,   OUTPUT);
  pinMode(pinGREEN, OUTPUT);
  pinMode(pinBLUE,  OUTPUT);
  ledcSetup(chRED,  PWMFREQ, PWMRESOLUTION);
  ledcSetup(chGREEN,PWMFREQ, PWMRESOLUTION); 
  ledcSetup(chBLUE, PWMFREQ, PWMRESOLUTION); 
  ledcAttachPin(pinRED,   chRED); 
  ledcAttachPin(pinGREEN, chGREEN); 
  ledcAttachPin(pinBLUE,  chBLUE);
}

void loop() {
  uint8_t i =0;
  uint8_t r, g, b;
  String S;
  char buf[11];
  /*
  onRed();   // Red demo
  delay(500);
  onGreen(); // Green demo
  delay(500);
  onBlue();  // Blue demo
  delay(500);
  */
  Serial.print("Red");
  r=255; g=0; b=0;
  printColor(r, g, b);
  setColor(r, g, b); 
  delay(colorTime);

  Serial.print("Green");
  r=0; g=255; b=0;
  printColor(r, g, b);
  setColor(r, g, b); 
  delay(colorTime);
  
  Serial.print("Blue");
  r=0; g=0; b=255;
  printColor(r, g, b);
  setColor(r, g, b); 
  delay(colorTime);
  
  r=0; g=128; b=128;
  printColor(r, g, b);
  setColor(r, g, b);  
  delay(colorTime);
  
  r=128; g=0; b=128;
  printColor(r, g, b);  
  setColor(r, g, b);  
  delay(colorTime);

  r=128; g=128; b=0;
  printColor(r, g, b);   
  setColor(r, g, b);  
  delay(colorTime);
      
  Serial.print("Orange");
  r=255; g=128; b=0;
  printColor(r, g, b);   
  setColor(r, g, b);  
  delay(colorTime);

  Serial.print("Lime");
  r=128; g=255; b=0;
  printColor(r, g, b);   
  setColor(r, g, b);  
  delay(colorTime);  
  
  Serial.print("Lite Green");
  r=0; g=255; b=128;
  printColor(r, g, b);  
  setColor(r, g, b);  
  delay(colorTime);  

  Serial.print("Sea Blue");
  r=0; g=128; b=255;
  printColor(r, g, b);   
  setColor(r, g, b);  
  delay(colorTime);

  Serial.print("Pink");
  r=255; g=0; b=128;
  printColor(r, g, b);   
  setColor(r, g, b);  
  delay(colorTime);  

  Serial.print("Purple");
  r=128; g=0; b=255;
  printColor(r, g, b);   
  setColor(r, g, b);  
  delay(colorTime);

  Serial.print("Yellow");
  r=255; g=255; b=45;
  printColor(r, g, b);   
  setColor(r, g, b);  
  delay(colorTime);

  Serial.print("Brown");
  r=128; g=64; b=0;
  printColor(r, g, b);   
  setColor(r, g, b);  
  delay(colorTime);  

  Serial.print("Grey");
  r=128; g=128; b=128;
  printColor(r, g, b);   
  setColor(r, g, b);  
  delay(colorTime);  

  Serial.print("Black");    
  r=0; g=0; b=0;
  printColor(r, g, b);   
  setColor(r, g, b); 
  delay(colorTime);
}

void printColor(uint8_t r, uint8_t g, uint8_t b) {
  Serial.print(" r="); Serial.print(r);
  Serial.print(" g="); Serial.print(g);
  Serial.print(" b="); Serial.println(b);    
}

void turnOff() {
  ledcWrite(chRED, 0);
  ledcWrite(chGREEN,0);
  ledcWrite(chBLUE, 0);
}

void setColor(uint8_t r, uint8_t g, uint8_t b) {
  ledcWrite(chRED,   r);
  ledcWrite(chGREEN, g);
  ledcWrite(chBLUE,  b);
  delay(ledTime);
}

void onWhite() {
  for (int i=0; i<256; i++) {
      ledcWrite(chRED,   i);
      ledcWrite(chGREEN, i);
      ledcWrite(chBLUE,  i);            
      delay(ledTime);
  }
  ledcWrite(chRED,   0);
  ledcWrite(chGREEN, 0);
  ledcWrite(chBLUE,  0);  
}

void onRed() {
  for (int i=0; i<256; i++) {
      ledcWrite(chRED, i);
      delay(ledTime);
  }
  ledcWrite(chRED, 0);
}

void onGreen() {
  for (int i=0; i<256; i++) {
      ledcWrite(chGREEN, i);
      delay(ledTime);
  }
  ledcWrite(chGREEN, 0);  
}

void onBlue() {
  for (int i=0; i<256; i++) {
      ledcWrite(chBLUE, i);
      delay(ledTime);
  }
  ledcWrite(chBLUE, 0);  
}
