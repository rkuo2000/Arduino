// A bicycle robot self balancing reaction wheel control.Using Arduino nano+ MPU6050+ kalman filter.
// https://youtu.be/WlqR8xeOhow
//
// Variable Feedback
// 1. Angle from IMU and using Kalman filter estimate angle.
// 2.Angular Velocity from IMU.
// 3.Wheel Position Using Encoder.
// 4.Wheel velocity differentiate from encoder.

#include <SoftwareSerial.h>
/* กำหนด Bluetooth Serial TXD เป็นขา DIO 11 หรือตรงกับขา MOSI บน ICSP Connector   */
#define BT_SERIAL_TX_DIO 11 
/* กำหนด Bluetooth Serial RXD เป็นขา DIO 12 หรือตรงกับขา MISO บน ICSP Connector  */
#define BT_SERIAL_RX_DIO 12
/* สร้าง software serial port สำหรับเชื่อมต่อกับ Bluetooth*/
SoftwareSerial BluetoothSerial(BT_SERIAL_TX_DIO, BT_SERIAL_RX_DIO);

#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050.h>
MPU6050 read_sensor;
#include <Encoder.h>
#include <EEPROM.h>
#include <EEPROMAnything.h>
//#include <MsTimer2.h>
#include <ServoTimer2.h>
ServoTimer2 rotation;
#include "Nonlinear.h"
//#include <Servo.h>  
//Servo myservo;  // create servo object to control a servo 
//int potpin = 1;  // analog pin used to connect the potentiometer
//int val_servo;    // variable to read the value from the analog pin 
//int ms_servo = 0;

#define Ts 0.004;
Encoder Disk(2,4);
EEPROMAnythingClass Control_gain;

byte noElem = 7;
unsigned int baseAddr = 0;
unsigned int num = 0;
double val[10];
/*
 * FS_SEL | Full Scale Range   | LSB Sensitivity
 * -------+--------------------+----------------
 * 0      | +/- 250 degrees/s  | 131 LSB/deg/s
 * 1      | +/- 500 degrees/s  | 65.5 LSB/deg/s
 * 2      | +/- 1000 degrees/s | 32.8 LSB/deg/s
 * 3      | +/- 2000 degrees/s | 16.4 LSB/deg/s
 */

/*
 * AFS_SEL | Full Scale Range | LSB Sensitivity
 * --------+------------------+----------------
 * 0       | +/- 2g           | 8192 LSB/mg
 * 1       | +/- 4g           | 4096 LSB/mg
 * 2       | +/- 8g           | 2048 LSB/mg
 * 3       | +/- 16g          | 1024 LSB/mg
 
  *          |   ACCELEROMETER    |           GYROSCOPE
 * DLPF_CFG | Bandwidth | Delay  | Bandwidth | Delay  | Sample Rate
 * ---------+-----------+--------+-----------+--------+-------------
 * 0        | 260Hz     | 0ms    | 256Hz     | 0.98ms | 8kHz
 * 1        | 184Hz     | 2.0ms  | 188Hz     | 1.9ms  | 1kHz
 * 2        | 94Hz      | 3.0ms  | 98Hz      | 2.8ms  | 1kHz
 * 3        | 44Hz      | 4.9ms  | 42Hz      | 4.8ms  | 1kHz
 * 4        | 21Hz      | 8.5ms  | 20Hz      | 8.3ms  | 1kHz
 * 5        | 10Hz      | 13.8ms | 10Hz      | 13.4ms | 1kHz
 * 6        | 5Hz       | 19.0ms | 5Hz       | 18.6ms | 1kHz
 * 7        |   -- Reserved --   |   -- Reserved --   | Reserved
 
 
 */



void setup() {

  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(3,OUTPUT); // servo
  pinMode(5,OUTPUT); // dir wheel
  pinMode(6,OUTPUT); // PWM wheel
  digitalWrite(6,HIGH);
  pinMode(9,OUTPUT); // PWM disk
  pinMode(10,INPUT);
  pinMode(13,OUTPUT);
  Wire.begin();
  Serial.begin(9600);
  //Serial.println("Initializing I2C devices..."); 
//  rotation.attach(9); 
  
  read_sensor.initialize();
  read_sensor.setFullScaleGyroRange(0); 
  read_sensor.setDLPFMode(5);
 // read_sensor.setMasterClockSpeed(9); //I2C Master Clock Speed 500kHz

 BluetoothSerial.begin(9600);

rotation.attach(3);

  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;

  TCNT1 = 65536-16000000/256*Ts;            // preload timer 65536-16MHz/(256/2Hz)
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
  interrupts();             // enable all interrupts


//    myservo.attach(3);  // attaches the servo on pin 9 to the servo object 
  //MsTimer2::set(0.1, flash);
  //MsTimer2::start();
}

//
//int servo_angle = 512;
//int servo_count = 0;
//
//void flash()
//{
//  servo_count += 1;
//  if (servo_count <= ms_servo)
//    digitalWrite(3,HIGH);
//  else
//    digitalWrite(3,LOW);
//  if (servo_count == 20)
//    servo_count = 0;
//}



char T[10],T1[10],Cl[10];
float w1,w2,w3[7],sign=1,ref_v;
int i=0,j=0,l=0,num_w=0,n=0;

void Input()
{

  
  if (Serial.available()) {
   
  
    char ch=Serial.read() ;
   
    if(ch==0x2E)//Check . 
    {
      j=1;
      l=1;
      ch=0x20;
      i=0;
    }
    else if(ch==0x2D)// Check -
    {
      sign=-1;        
    }
    else if((ch!=0x2E)&&(j==0)) // ch!=. and j=0
    {   
      T[i]=ch;
      i++;      
    }
    else if((ch!=0x2E)&&(l==1)) // ch!=. and l=1
    {   
      T1[i]=ch;
      i++;            
    }

    // เก็บข้อมูล ///
    /* Caculate data */
    if((ch==0x2C)&&(num_w!=7))
    {
      w1=atof(T);
      w2=atof(T1);
      w3[num_w]=w1+(w2/10000);
      w3[num_w]=w3[num_w]*sign;

      i=0;
      j=0;
      l=0;

      sign=1;
      strcpy(T,"new");
      strcpy(T1,"new");
      num_w++; 
    }
  
    else if(num_w==7)
    {
      
      num_w=0;
      n=0;
      T[0]=' ';
      T1[0]=' ';  // เราเพิ่มมาเอง
      
      for (int i=0; i <=  noElem -1; i++){
   num =Control_gain.writeAnything( (i*4)+baseAddr, w3[i]);
  }

    
    }
    
  }
  
}   


float x[2][1]={{0.0},{0.0}};
float P[2][2]={{0},{0}};
float K[2][1];

float ay,az=.02;
float gx;
float theta,disk,disk_dot,disk_old;
float zk;
float uk;
float gyro;
float dt=Ts;
float Q_angle;
float Q_bias;
float R_measure;
float theta_d,gyro_offset;

void Read_encoder()
{
  disk=(float)Disk.read()*2*PI/400;
  disk_dot=(disk-disk_old)/Ts;
  disk_old=disk;
}

void KalmanSensorFusion(){
float Q_angle=0.001;
float Q_bias=0.003; 
float R_measure=0.03;

  gyro=(float)gx*PI/(131*180);
 
 /* Predict Project the state ahead */
 
  uk=gyro;
  x[0][0]=x[0][0]+dt*uk-dt*x[1][0];
  x[1][0]=x[1][0];

  /* P=A*P*P'+Q */

  
  P[0][0]=P[0][0]+dt*(dt*P[1][1]-P[0][1]-P[1][0]+Q_angle);
  P[0][1]=P[0][1]-dt*P[1][1];
  P[1][0]=P[1][0]-dt*P[1][1];
  P[1][1]=P[1][1]+Q_bias*dt;
 
 /* measure*/
 
  theta=atan(ay/az); /* Acclerometer*/
  zk=theta;

  K[0][0]=P[0][0]/(P[0][0]+R_measure);
  K[1][0]=P[1][0]/(P[0][0]+R_measure);

  x[0][0]+=K[0][0]*(zk-x[0][0]);
  x[1][0]+=K[1][0]*(zk-x[0][0]);

  P[0][0]=(1-K[0][0])*P[0][0];
  P[0][1]=(1-K[0][0])*P[0][1];
  P[1][0]=-K[1][0]*P[0][0]+P[1][0];
  P[1][1]=-K[1][0]*P[0][1]+P[1][1];
 

}




/* float sat_fnc=0;

float sat(float u,float MAX,float MIN){
  // function นี้เราเขียนเอง Saturation function เราเอามาจาก Khalil(Nonlinear system)แล้วก็มาเขียน C Code
  // สุดท้ายก็คือ เราต้องการใส่ Sat ของต้วแปรไดเราก็เพียงพิมพ์ sat(ใส่สิ่งที่ต้องการ)
  if(abs(u)<=MAX){
    sat_fnc=u;
  }
  if(abs(u)>MAX){
    if(u>0){
      sat_fnc=MAX;
    }  
    if(u==0){
      sat_fnc=0;
    }
    if(u<0){
      sat_fnc=MIN;
    }
  }

  return(sat_fnc); //คืนค่า
}
 */

float uf;
long in;
float kp,kd,kd_disk,kp_disk=0.00187,disk_desing;

void PID()
{
  

Nonlinear Function;


float err,err_dot,err1;  

err=x[0][0]-theta_d;
err1=disk-disk_desing;
err_dot=gyro-gyro_offset;

if(digitalRead(10)==0 and abs(x[0][0])<0.3){

Function.Saturation(255*(kp*err+kd*err_dot+kd_disk*disk_dot+kp_disk*err1),255,-255); //RUN SATURATE FUNCTUIN

uf=Function.Sat_Fnc; // CONTROL LAW DESIGN





bluetooth();
}
else 
{
  uf=0;
  Disk.write(0); // Clear Encoder 
   digitalWrite(6,HIGH);
   digitalWrite(7,LOW);
   digitalWrite(8,LOW);
}





} 

void SentPID2Drive()
{
  
  in=(int)uf;
   analogWrite(5,abs(in));
   
     if(uf>0)
  {
    digitalWrite(7,LOW);
    digitalWrite(8,HIGH);
  }

  else if(uf<0)
  {
     digitalWrite(7,HIGH);
     digitalWrite(8,LOW);
   }
   
   


}





ISR(TIMER1_OVF_vect) 
{  
  TCNT1 = 65536-16000000/256*Ts;
  Read_encoder();
  KalmanSensorFusion();
  PID();
  SentPID2Drive();
  
}

void loop() {
  Input();
//  servo;
  
  if(digitalRead(10)==0){
  LED();
  }
  else
  {
   Disk.write(0);
   disk_desing=disk;
   gyro_offset=gyro;
   digitalWrite(7,LOW);
   digitalWrite(8,LOW);
   digitalWrite(13,LOW);
  }
  ay=(float)read_sensor.getAccelerationY()/8192;
  az=(float)read_sensor.getAccelerationZ()/8192;
  gx=(float)read_sensor.getRotationX();
  
  
  for (int i=0; i <= noElem -1; i++){
    
    int addr = (i*4)+baseAddr;
    num = Control_gain.readAnything( addr,val[i]);
    
  }
  
kp=val[1];
kd=val[2];
kd_disk=val[3];
/* R_measure=val[4];
Q_angle=val[5];
Q_bias=val[6]; 
  */

//  Serial.print(kp);
//  Serial.print("\t");
//  Serial.print(kd);
//  Serial.print("\t");
//  Serial.print(kd_disk);
//  Serial.print("\t");
//  Serial.print(R_measure,4);
//  Serial.print("\t");
//  Serial.print(Q_angle,4);
//  Serial.print("\t");
//  Serial.print(Q_bias,4);
//  Serial.print("\t");
//  Serial.print(num_w);
//  Serial.println();
 servo();
  Serial.print(theta);
  Serial.print("\t");
  Serial.print(x[0][0]);
  Serial.print("\t");
  /* Serial.print(gyro);
  Serial.print("\t");
  Serial.print(disk);
  Serial.print("\t");
  Serial.print(disk_dot);
  Serial.print("\t");
  Serial.print(uf);
  Serial.print("\t");
  Serial.print(num_w); */
  Serial.println();

//  val_servo = analogRead(potpin);            // reads the value of the potentiometer (value between 0 and 1023) 
//  ms_servo = map(analogRead(1), 0, 1023, 0, 20);     // scale it to use it with the servo (value between 0 and 180) 
  //Serial.println(analogRead(1),DEC);
//  myservo.write(val_servo);                  // sets the servo position according to the scaled value 
}


void bluetooth()
{
  if (BluetoothSerial.available())
  {
    int a = BluetoothSerial.read();
    Serial.println(a,DEC);
    if (a >= 12)
    { 
      analogWrite(6,((22-a)*25.5));
      digitalWrite(9,LOW);
    }
    else if (a <= 10)
    {
      analogWrite(6,((10-a)*25.5));
      digitalWrite(9,HIGH);
    }
    else analogWrite(6,255);
  }
}


int ledState = LOW;             
long previousMillis = 0;     
long interval = 100; 

void LED()
{
  unsigned long currentMillis = millis();
 const int ledPin =  13;      // the number of the LED pin

    
  if(currentMillis - previousMillis > interval) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;   

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW)
      ledState = HIGH;
    else
      ledState = LOW;

    // set the LED with the ledState of the variable:
    digitalWrite(ledPin, ledState);
  }
}

void servo()
{
   int val;
  
  // val = incPulse(analogRead(1), 8);
   val = map(analogRead(1), 0, 1023, 1020,1770);     // 
   rotation.write(val);
    
}
