#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <functional>
#include "switch.h"
#include "UpnpBroadcastResponder.h"
#include "CallbackFunction.h"

// prototypes
boolean connectWifi();

//on/off callbacks 
void officeLightsOn();
void officeLightsOff();
void kitchenLightsOn();
void kitchenLightsOff();
void livingroomLightsOn();
void livingroomLightsOff();

// Change this before you flash
const char* ssid = "Kuo";
const char* password = "0972211921";

boolean wifiConnected = false;

UpnpBroadcastResponder upnpBroadcastResponder;

Switch *office = NULL;
Switch *kitchen = NULL;
Switch *livingroom = NULL;
int Switch1Pin = D1; // Switch1 Pin = D1
int Switch2Pin = D5; // Switch2 Pin = D5
int Switch3Pin = D6; // Switch3 Pin = D6

void setup()
{
  Serial.begin(9600);
  pinMode(Switch1Pin, OUTPUT);
  pinMode(Switch2Pin, OUTPUT);
  pinMode(Switch3Pin, OUTPUT);
   
  // Initialise wifi connection
  wifiConnected = connectWifi();
  
  if(wifiConnected){
    upnpBroadcastResponder.beginUdpMulticast();
    
    // Define your switches here. Max 14
    // Format: Alexa invocation name, local port no, on callback, off callback
    office = new Switch("office lights", 80, officeLightsOn, officeLightsOff);
    kitchen = new Switch("kitchen lights", 81, kitchenLightsOn, kitchenLightsOff);
    livingroom = new Switch("livingroom lights", 82, livingroomLightsOn, livingroomLightsOff);
    
    Serial.println("Adding switches upnp broadcast responder");
    upnpBroadcastResponder.addDevice(*office);
    upnpBroadcastResponder.addDevice(*kitchen);
    upnpBroadcastResponder.addDevice(*livingroom);    
  }
}
 
void loop()
{
	 if(wifiConnected){
      upnpBroadcastResponder.serverLoop();

      livingroom->serverLoop();
      kitchen->serverLoop();
      office->serverLoop();
	 }
}

void officeLightsOn() {
    digitalWrite(Switch1Pin, HIGH); 
    Serial.print("Switch 1 turn on ...");
}

void officeLightsOff() {
    digitalWrite(Switch1Pin, LOW);   
    Serial.print("Switch 1 turn off ...");
}

void kitchenLightsOn() {
    digitalWrite(Switch2Pin, HIGH);  
    Serial.print("Switch 2 turn on ...");
}

void kitchenLightsOff() {
    digitalWrite(Switch2Pin, LOW);  
    Serial.print("Switch 2 turn off ...");
}


void livingroomLightsOn() {
    digitalWrite(Switch3Pin, HIGH);  
    Serial.print("Switch 3 turn on ...");
}

void livingroomLightsOff() {
    digitalWrite(Switch3Pin, LOW);  
    Serial.print("Switch 3 turn off ...");
}

// connect to wifi – returns true if successful or false if not
boolean connectWifi(){
  boolean state = true;
  int i = 0;
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting ...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 10){
      state = false;
      break;
    }
    i++;
  }
  
  if (state){
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("");
    Serial.println("Connection failed.");
  }
  
  return state;
}
