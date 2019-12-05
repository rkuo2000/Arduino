// 1. create Firebase ntou-aiot
// 2. NodeMCU read DHT22, pushString to Firebase
//    NodeMCU go to DeepSleep mode for 30 seconds
//    NodeMCU wakes up by itself when GPIO 16 (D0 in NodeMCU board) is connected to the RESET pin
// 3. create Firebase functions (timestamp on /CC3302/Temperature triggered) 
//    mkdir esp8266
//    cd esp8266
//    firebase init
//    firebase deploy --only functions 
//    vi function/src/index.ts
//    -- export const formatData = functions.database.ref('/CC3302/DHT22/Temperature/{pushId}/')
// 4. plotly.js
//    https://github.com/olivierlourme/esp8266webapp
//    cd esp8266webapp/public
//    open index.html

#include <ESP8266WiFi.h>
#include "FirebaseESP8266.h"
#include "DHT.h"

#define DHTPIN D6     // NodeMCU pin D6 connected to DHT11 pin Data
DHT dht(DHTPIN, DHT22, 15);

#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define FIREBASE_HOST "YOUR_PROJECT_ID.firebaseio.com"
#define FIREBASE_AUTH "YOUR_FIREBASE_AUTH" // secret (40 chars)

const String path="/CC3302/DHT22";

//Define FirebaseESP8266 data object
FirebaseData firebaseData;
FirebaseJson json;
 
void setup() {
  Serial.begin(115200);
  dht.begin();
  delay(10);
  
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Let's connect to Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  //Set database read timeout to 1 minute (max 15 minutes)  
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).  
  Firebase.setwriteSizeLimit(firebaseData, "tiny");  //tiny, small, medium, large and unlimited

  // read DHT22
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  Serial.print("Humidity: ");  Serial.print(h);
  String fireHumid = String(h) + String("%");
  Serial.print(",  Temperature: ");  Serial.print(t);  Serial.println("°C");
  String fireTemp = String(t) + String("°C");

  Serial.println("Push String to Firebase.....");
  Firebase.pushString(firebaseData, path+"/Humidity", fireHumid);
  Firebase.pushString(firebaseData, path+"/Temperature", fireTemp);
  
  // Deep sleep mode, the ESP8266 wakes up by itself when GPIO 16 (D0 in NodeMCU board) is connected to the RESET pin  
  Serial.println("Deep Sleep Mode for 30 seconds!"); 
  //ESP.deepSleep(300e6, WAKE_RF_DEFAULT); // 5 minutes (1st parameter is in µs!)
  ESP.deepSleep(30e6, WAKE_RF_DEFAULT);    // 30 seconds   
}

void loop() {
}
