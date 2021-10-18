//
// NodeMCU RoboCar with WebUI control
//
// To connect network SSID = your_ssid (password = your_passwd)
// Then open browser at 192.168.4.1
//
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <SparkFun_TB6612.h>

// TB6612FNG pin connection
#define STBY D0
#define PWMA D3
#define AIN2 D4
#define AIN1 D5
#define BIN1 D6
#define BIN2 D7
#define PWMB D8

// PWM Full Speed value
// PWM freq : NodeMCU = 1KHz, UNO = 500Hz
// PWM duty   NodeMCU = 1023 (10-bit PWM), UNO = 255 (8-bit PWM)

const int FULLSPEED = 1023;

// value 1 or -1 for motor spining default
const int offsetA = 1;
const int offsetB = 1;

Motor motorR = Motor(AIN1, AIN2, PWMA, offsetA, STBY);
Motor motorL = Motor(BIN1, BIN2, PWMB, offsetB, STBY);

/* Set these to your desired credentials. */
const char *ssid = "ESP8266";
const char *password = "1234567890";

ESP8266WebServer server(80);

const String HTTP_HEAD   = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>{v}</title>";
const String HTTP_STYLE  = "<style>.c{text-align: center;} div,input{padding:5px;font-size:1em;}  input{width:90%;}  body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.6rem;background-color:#1fb3ec;color:#fdd;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;} .button2 {background-color: #008CBA;} .button3 {background-color: #f44336;} .button4 {background-color: #e7e7e7; color: black;} .button5 {background-color: #555555;} .button6 {background-color: #4CAF50;} </style>";
const String HTTP_SCRIPT = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>";
const String HTTP_HEAD_END= "</head><body><div style='text-align:left;display:inline-block;min-width:260px;'>";

const String HOMEPAGE = "<form action=\"/cmd1\" method=\"get\"><button class=\"button1\">Forward</button></form><br/><form action=\"/cmd2\" method=\"get\"><button class=\"button2\">Backward</button></form><br/><form action=\"/cmd3\" method=\"get\"><button class=\"button3\">Right</button></form><br/><form action=\"/cmd4\" method=\"get\"><button class=\"button4\">Left</button></form><br/><form action=\"/cmd5\" method=\"get\"><button class=\"button5\">Stop</button></form><br/>    ";

void handleRoot() {
 String s =HTTP_HEAD;
      s += HTTP_STYLE;
      s += HTTP_SCRIPT;  
      s += HTTP_HEAD_END;
      s+=HOMEPAGE;
  server.send(200, "text/html", s);
}

void cmd1() {
 String s =HTTP_HEAD;
      s += HTTP_STYLE;
      s += HTTP_SCRIPT;  
      s += HTTP_HEAD_END;
      s+=HOMEPAGE;
  server.send(200, "text/html", s);
  forward(motorR, motorL, FULLSPEED);
}
void cmd2() {
 String s =HTTP_HEAD;
      s += HTTP_STYLE;
      s += HTTP_SCRIPT;  
      s += HTTP_HEAD_END;
      s+=HOMEPAGE;
  server.send(200, "text/html", s);
  backward(motorR, motorL, FULLSPEED);
}
void cmd3() {
 String s =HTTP_HEAD;
      s += HTTP_STYLE;
      s += HTTP_SCRIPT;  
      s += HTTP_HEAD_END;
      s+=HOMEPAGE;
  server.send(200, "text/html", s);
  right(motorR, motorL, FULLSPEED);;
}
void cmd4() {
 String s =HTTP_HEAD;
      s += HTTP_STYLE;
      s += HTTP_SCRIPT;  
      s += HTTP_HEAD_END;
      s+=HOMEPAGE;
  server.send(200, "text/html", s);
  left(motorR, motorL, FULLSPEED);    
}
void cmd5() {
 String s =HTTP_HEAD;
      s += HTTP_STYLE;
      s += HTTP_SCRIPT;  
      s += HTTP_HEAD_END;
      s+=HOMEPAGE;
  server.send(200, "text/html", s);
  brake(motorR, motorL);
}

void setup() {
  Serial.begin(115200);  
  Serial.println();
  Serial.print("Configuring access point..."); 
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();  
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.on("/cmd1", cmd1);
  server.on("/cmd2", cmd2);
  server.on("/cmd3", cmd3);
  server.on("/cmd4", cmd4);
  server.on("/cmd5", cmd5);
  
  Serial.println("HTTP server started");
  server.begin();
  brake(motorR, motorL);
}

void loop() {
  server.handleClient();
}
