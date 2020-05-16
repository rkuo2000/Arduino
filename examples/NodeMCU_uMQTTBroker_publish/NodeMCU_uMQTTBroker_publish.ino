/*
 * uMQTTBroker demo for Arduino
 * 
 * Minimal Demo: the program simply starts a broker and waits for any client to connect.
 */

#include <ESP8266WiFi.h>
#include "uMQTTBroker.h"

//const char* mqtt_server = "broker.mqtt-dashboard.com";
IPAddress mqtt_server(192, 168, 1, 5); // local MQTT broker
const char* outTopic = "NTOU-AIOT";
const char* inTopic  = "NTOU-AIOT";

long lastMsg = 0;
char msg[50];
int value = 0;

const char* ssid = "Kuo";
const char* password = "0972211921";

class myMQTTBroker: public uMQTTBroker
{
public:
    virtual bool onConnect(IPAddress addr, uint16_t client_count) {
      Serial.println(addr.toString()+" connected");
      return true;
    }
    
    virtual bool onAuth(String username, String password) {
      Serial.println("Username/Password: "+username+"/"+password);
      return true;
    }
    
    virtual void onData(String topic, const char *data, uint32_t length) {
      char data_str[length+1];
      os_memcpy(data_str, data, length);
      data_str[length] = '\0';
      
      Serial.println("received topic '"+topic+"' with data '"+(String)data_str+"'");
    }
};

myMQTTBroker myBroker;

void startWiFiClient()
{
  Serial.println("Connecting to "+(String)ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  
  Serial.println("WiFi connected");
  Serial.println("IP address: " + WiFi.localIP().toString());
}

void startWiFiAP()
{
  WiFi.softAP(ssid, password);
  Serial.println("AP started");
  Serial.println("IP address: " + WiFi.softAPIP().toString());
}

void setup()
{
  int client_count = 0;
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  // Connect to a WiFi network
  startWiFiClient();

  // Or start the ESP as AP
  //startWiFiAP();

  // Subscribe the broker
  Serial.println("MQTT broker subscribed...");
  myBroker.onConnect(mqtt_server, client_count);
  myBroker.subscribe(inTopic); // subscribe to anything 
}

void loop()
{ 
  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 75, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    myBroker.publish(outTopic, msg);
  }
}
