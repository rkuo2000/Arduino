//
// MQ3: Alcohol Sensor
//
//int TIME_UNTIL_WARMUP = 900;
//unsigned long time;

void setup() {
  Serial.begin(115200);
  analogSetPinAttenuation(A0,ADC_11db);
  adcAttachPin(A0);
//  adcStart(A0);
  
  // warm-up MQ-3 for 15 minutes
  //time = millis()/1000;
  //while(time<=TIME_UNTIL_WARMUP);
  
  // warm-up MQ-3 till reading value <600
  Serial.println("Warming up MQ Sensor ...");
  while (analogRead(A0)>600){
    Serial.println(analogRead(A0));
    delay(200);   
  }
}

void loop() {
  int alco;
  alco = analogRead(A0);
  Serial.println(alco);
}
