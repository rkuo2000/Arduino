/*
 * uMQTTBroker demo for Arduino
 * 
 * Minimal Demo: the program simply starts a broker and waits for any client to connect.
 */
 
#include "FirebaseESP8266.h" //FirebaseESP8266.h must be included before ESP8266WiFi.h
#include <ESP8266WiFi.h>
#include "uMQTTBroker.h"

uMQTTBroker myBroker;

#define FIREBASE_HOST "https://ntou-aiot.firebaseio.com" //Without http:// or https://
#define FIREBASE_AUTH "NkvyTe1HJVVEoa97mUBU1ACnbfQc0sToj2n9qI3L"
#define WIFI_SSID "Kuo"
#define WIFI_PASSWORD "0972211921"
//const char* mqtt_server = "broker.mqtt-dashboard.com";
const char* mqtt_server = "192.168.1.5";
const char* outTopic = "NTOU-AIOT";
const char* inTopic  = "NTOU-AIOT";

//Define FirebaseESP8266 data object
FirebaseData firebaseData;
FirebaseJson json;

void startWiFiClient()
{
  Serial.println("Connecting to "+(String)WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
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
  WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("AP started");
  Serial.println("IP address: " + WiFi.softAPIP().toString());
}

void setup()
{
  Serial.begin(115200);
  Serial.println();
  
  startWiFiClient(); // Connect to a WiFi network
  //startWiFiAP(); // Or start the ESP as AP
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(firebaseData, "tiny");
  
  // Start the broker
  Serial.println("Starting MQTT broker");
  myBroker.init();
  myBroker.subscribe("#"); // Subscribe to anything  
}

void loop()
{  
/*
  if (Firebase.pushString(firebaseData, path+"/Temperature/"+tag, temp)
  {
    Serial.println("Write Firebase OK !");
    Serial.println("------------------------------------");    
  } else {
    Serial.println("Write Firebase FAILED! ");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
  }
  

  delay(10000);
*/  
}
