//
// Webserver to receive data from Webclients
// To use a web browser to open IP address of this webserver 
//
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

const char* ssid     = "Home_SSID";
const char* password = "Home_Password";

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

void cmd1() {\ 
 // read sensor1 data
 // ...to-be-coded
 String s =HTTP_HEAD;
      s += HTTP_STYLE;
      s += HTTP_SCRIPT;  
      s += HTTP_HEAD_END;
      s+=HOMEPAGE;
  server.send(200, "text/html", s);

}
void cmd2() {
 // read sensor2 data
 // ...to-be-coded 
 String s =HTTP_HEAD;
      s += HTTP_STYLE;
      s += HTTP_SCRIPT;  
      s += HTTP_HEAD_END;
      s+=HOMEPAGE;
  server.send(200, "text/html", s);

}

void cmd3() {
 // read sensor3 data
 // ...to-be-coded 
 String s =HTTP_HEAD;
      s += HTTP_STYLE;
      s += HTTP_SCRIPT;  
      s += HTTP_HEAD_END;
      s+=HOMEPAGE;
  server.send(200, "text/html", s);
}

void setup() {
  Serial.begin(115200);

  // disable all output to save power
  turnOff(0);
  turnOff(2);
  turnOff(4);
  turnOff(5);
  turnOff(12);
  turnOff(13);
  turnOff(14);
  turnOff(15);

  dht.begin();
  delay(10);
  

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/cmd1", cmd1);
  server.on("/cmd2", cmd2);
  server.on("/cmd3", cmd3);
  
  Serial.println("HTTP server started");
  server.begin();
}

void loop() {
  server.handleClient();
}
