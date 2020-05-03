// GY953 : STMicro 9-axis AHRS navigation sensor
//         UART interface (D4, D3) to GY953 
#include <SoftwareSerial.h>
// NodeMCU pins
#define RX_PIN D4 // connect to TXD of module
#define TX_PIN D3 // connect to RXD of module (logic level 3.3v!)

int YPR[3];
unsigned char Re_buf[11], counter=0;
unsigned char sign =0;

SoftwareSerial swSer(RX_PIN, TX_PIN);

void setup() {  
  Serial.begin(115200);
  swSer.begin(115200);

  delay(2000);
  // GY953 commands
  swSer.write(0xA5); swSer.write(0x45); swSer.write(0xEA); //the Euler angles (default 50Hz)
  swSer.write(0xA5); swSer.write(0xAd); swSer.write(0x52);
}

void loop() {
  while(swSer.available() >0){
    Re_buf[counter]= swSer.read();
    if (counter==0 && Re_buf[0]!=0x5A) return;
    counter++;
    if (counter==11) {
      counter=0;
      sign=1;
    }
  }  
  
  if (sign){
    sign=0;
     if(Re_buf[0]==0x5A && Re_buf[1]==0x5A )    //检查帧头，帧尾
     {                 
       YPR[0]=(Re_buf[8]<<8|Re_buf[9])/100;   //合成数据，去掉小数点后2位
       YPR[1]=(Re_buf[6]<<8|Re_buf[7])/100;
       YPR[2]=(Re_buf[4]<<8|Re_buf[5])/100;
       if (YPR[0]>=475) YPR[0]-=295;
       if (YPR[1]>=475) YPR[1]-=295;
       if (YPR[2]>=475) YPR[2]-=295;              
       Serial.print("YPR:\t");
       Serial.print(YPR[0], DEC); Serial.print("\t"); //显示航向
       Serial.print(YPR[1], DEC); Serial.print("\t"); //显示俯仰角
       Serial.println(YPR[2], DEC);                   //显示横滚角 
       delay(10);
     }
  }

}
