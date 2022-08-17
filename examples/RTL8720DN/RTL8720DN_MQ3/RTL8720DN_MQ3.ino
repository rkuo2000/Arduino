// MQ-3: Alcohol Sensor 
// MQ-2: Gas Sensor

#define ADC_pin A2 // NodeMCU = A0 RTL8720DN = A2
#define ADC_level 1000
#define sampleDelay 200
unsigned long initTime;

void setup() {
  Serial.begin(115200); 
//  analogSetPinAttenuation(ADC_pin, ADC_11db);  
//  adcAttachPin(ADC_pin);
//  adcStart(ADC_pin);
  
  // warm-up MQ-3
  initTime = millis();
  while(analogRead(ADC_pin) > ADC_level) {
    Serial.print(analogRead(ADC_pin)); 
    Serial.print("\t");
    Serial.println(millis()-initTime);
  }
  Serial.println(".....reading ADC......");
}

void loop() {
  int adc_value;
  adc_value = analogRead(ADC_pin);
  Serial.println(adc_value);
  delay(sampleDelay); 
}
