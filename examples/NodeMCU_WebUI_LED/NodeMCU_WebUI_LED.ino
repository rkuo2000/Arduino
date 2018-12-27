//
// NodeMCU WebUI to control LEDs
//
// To connect network SSID = NodeMCU (password = 1234)
// Then open browser at 192.168.4.1
//
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

/* Set these to your desired credentials. */
const char *ssid = "NodeMCU";
const char *password = "1234";

ESP8266WebServer server(80);

const String HTTP_HEAD   = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>{v}</title>";
const String HTTP_STYLE  = "<style>.c{text-align: center;} div,input{padding:5px;font-size:1em;}  input{width:90%;}  body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.6rem;background-color:#1fb3ec;color:#fdd;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;} .button2 {background-color: #008CBA;} .button3 {background-color: #f44336;} .button4 {background-color: #e7e7e7; color: black;} .button5 {background-color: #555555;} .button6 {background-color: #4CAF50;} </style>";
const String HTTP_SCRIPT = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>";
const String HTTP_HEAD_END= "</head><body><div style='text-align:left;display:inline-block;min-width:260px;'>";

const String HOMEPAGE = "<form action=\"/cmd1\" method=\"get\"><button class=\"button3\">Red</button></form><br/><form action=\"/cmd2\" method=\"get\"><button class=\"button6\">Green</button></form><br/> <form action=\"/cmd3\" method=\"get\"><button class=\"button2\">Blue</button></form><br/><form action=\"/cmd4\" method=\"get\"><button class=\"button4\">Off</button></form><br/>    ";

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 */
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
   digitalWrite(D0,HIGH);
}
void cmd2() {
 String s =HTTP_HEAD;
      s += HTTP_STYLE;
      s += HTTP_SCRIPT;  
      s += HTTP_HEAD_END;
      s+=HOMEPAGE;
  server.send(200, "text/html", s);
    digitalWrite(D1,HIGH);
}
void cmd3() {
 String s =HTTP_HEAD;
      s += HTTP_STYLE;
      s += HTTP_SCRIPT;  
      s += HTTP_HEAD_END;
      s+=HOMEPAGE;
  server.send(200, "text/html", s);
   digitalWrite(D2,HIGH);
}
void cmd4() {
 String s =HTTP_HEAD;
      s += HTTP_STYLE;
      s += HTTP_SCRIPT;  
      s += HTTP_HEAD_END;
      s+=HOMEPAGE;
  server.send(200, "text/html", s);
    digitalWrite(D0,LOW);
    digitalWrite(D1,LOW);
    digitalWrite(D2,LOW);
     
}

void setup() {
  delay(1000);
  pinMode(D0,OUTPUT);
  pinMode(D1,OUTPUT);
  pinMode(D2,OUTPUT);
 
  digitalWrite(D0,LOW);
    
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.on("/cmd1", cmd1);
  server.on("/cmd2", cmd2);
  server.on("/cmd3", cmd3);
  server.on("/cmd4", cmd4);
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
