#include <WiFi.h>
#include <WiFiUdp.h>
#include <coap-simple.h>

const char* ssid     = "Your_SSID";
const char* password = "Your_Password";

WiFiUDP udp;
Coap coap(udp);

bool LEDstate;
int msgid;

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // start coap server/client
  coap.start();

  Serial.println("Send Request");
  int msgid = coap.get(IPAddress(192,168,0,43), 5683, "time");  
}

void loop() {
  Serial.println("Send Request");
  if (LEDstate) 
    msgid = coap.put(IPAddress(192,168,0,43), 5683, "light", "1");
  else
    msgid = coap.put(IPAddress(192,168,0,43), 5683, "light", "0");
  LEDstate=!LEDstate;
  delay(1000);
}
