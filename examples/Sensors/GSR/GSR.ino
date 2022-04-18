// GSR - Galvanic Skin Response sensor (analog input)
// print GSR data to Serial port
// use Putty connect to Serial port, then save print data as .csv
// Excel can read .csv to show waveform
const int BUZZER = D0; // output to buzzer
const int GSR = A2;    // analog input from GSR sensor
int threshold = 0;
int sensorValue;

void setup(){
  long sum=0;
	
  Serial.begin(115200);
  pinMode(BUZZER,OUTPUT);
  digitalWrite(BUZZER,LOW);
  delay(1000);
  
  # read 500 times to avergage as the threshold
  for(int i=0;i<500;i++){
    sensorValue=analogRead(GSR);
    sum += sensorValue;
    delay(5);
  }
  threshold = sum/500;
  Serial.print("threshold =");
  Serial.println(threshold);
}

void loop(){
  int temp;
  sensorValue=analogRead(GSR);
  Serial.print("sensorValue=");
  Serial.println(sensorValue);
  temp = threshold - sensorValue;
  if(abs(temp)>50)
  {
    sensorValue=analogRead(GSR);
    temp = threshold - sensorValue; 
    if(abs(temp)>50){               
      digitalWrite(BUZZER,HIGH);
      Serial.println("YES!");
      delay(3000);
      digitalWrite(BUZZER,LOW);
      delay(1000);
    }
  }
}
