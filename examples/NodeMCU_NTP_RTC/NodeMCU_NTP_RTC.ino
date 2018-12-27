/// Import Arduino Library : Time.zip & TimeAlarms.zip 
//
// NodeMCU Network Time RTC
//
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Time.h>
#include <TimeAlarms.h>

char* ssid="Kuo";          // WiFi SSID
char* passwd="0972211921"; // WiFi Password

unsigned int localPort=2390; //local port to listen for UDP packets
IPAddress timeServerIP;      // time.nist.gov NTP server address
const char* ntpServerName="time.nist.gov"; // NTP Server host name
const int NTP_PACKET_SIZE=48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE]; // buffer to hold incoming and outgoing packets
WiFiUDP udp; // UDP instance to let us send and receive packets over UDP
int count=0;

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  // Connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid,passwd);
  while (WiFi.status()!=WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  // Start UDP
  Serial.println("Starting UDP");
  udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());
  sync_clock();
}

void loop()
{
  String d = getDate();
  String t = getTime();
  String w = getWeek();
  int Hm=hour()*100 + minute();   //時:分整數, 0~2359
  Serial.print(d + " " + t + " " + w + " ");
  Serial.println(Hm);   
  ++count;
  if (count >= 40) {  //每 40 次迴圈與 NTP 同步一次
      sync_clock();
      count=0;
  }
  delay(1000);
}

void sync_clock()
{
  unsigned long GMT=getUnixTime();
  if (GMT != 0) {   //有得到 NTP 回應才更新 ESP8266 內建 RTC
    setTime(GMT + 28800L);  //以台灣時間更新內部時鐘
    } 
}

String getDate() 
{
  String d=(String)year() + "-";
  byte M=month();
  if (M < 10) {d.concat('0');}
  d.concat(M);
  d.concat('-');
  byte D=day();
  if (D < 10) {d.concat('0');}
  d.concat(D);
  return d;
}

String getTime() 
{
  String t="";
  byte h=hour();
  if (h < 10) {t.concat('0');}
  t.concat(h);
  t.concat(':');
  byte m=minute();
  if (m < 10) {t.concat('0');}
  t.concat(m);
  t.concat(':');
  byte s=second();
  if (s < 10) {t.concat('0');}
  t.concat(s);
  return t;
}

String getWeek() 
{
  String w[]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
  return w[weekday()-1];
}

unsigned long getUnixTime() 
{
    WiFi.hostByName(ntpServerName, timeServerIP);  //get a random server from the pool
    sendNTPpacket(timeServerIP);                   //send an NTP packet to a time server
    delay(1000);                                   // wait to see if a reply is available

    int cb=udp.parsePacket();                      //return bytes received
    unsigned long unix_time=0;
    if (!cb) {Serial.println("no packet yet");}
    else {  //received a packet, read the data from the buffer
        Serial.print("packet received, length=");
        Serial.println(cb);                        //=48 
        udp.read(packetBuffer, NTP_PACKET_SIZE);  //read the packet into the buffer

        //the timestamp starts at byte 40 of the received packet and is four bytes,
        //or two words, long. First, esxtract the two words:
        unsigned long highWord=word(packetBuffer[40], packetBuffer[41]);
        unsigned long lowWord=word(packetBuffer[42], packetBuffer[43]);
        //combine the four bytes (two words) into a long integer
        //this is NTP time (seconds since Jan 1 1900):
        unsigned long secsSince1900=highWord << 16 | lowWord;
        Serial.print("Seconds since Jan 1 1900=" );
        Serial.println(secsSince1900);
        Serial.print("Unix time=");
        //Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
        unix_time=secsSince1900 - 2208988800UL;
        Serial.print(F("Unix time stamp (seconds since 1970-01-01)="));
        Serial.println(unix_time); //print Unix time
        }   
    return unix_time; //return seconds since 1970-01-01
}

unsigned long sendNTPpacket(IPAddress& address) 
{
    Serial.println("sending NTP packet...");
    // set all bytes in the buffer to 0
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    //Initialize values needed to form NTP request
    //(see URL above for details on the packets)
    packetBuffer[0]=0b11100011;   // LI, Version, Mode
    packetBuffer[1]=0;     // Stratum, or type of clock
    packetBuffer[2]=6;     // Polling Interval
    packetBuffer[3]=0xEC;  // Peer Clock Precision
    //8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12]=49;
    packetBuffer[13]=0x4E;
    packetBuffer[14]=49;
    packetBuffer[15]=52;
    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:
    udp.beginPacket(address, 123); //NTP requests are to port 123
    udp.write(packetBuffer, NTP_PACKET_SIZE);
    udp.endPacket();
}
