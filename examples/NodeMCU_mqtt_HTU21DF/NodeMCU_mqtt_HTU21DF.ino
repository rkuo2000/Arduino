// Connect Vin to 3-5VDC
// Connect GND to ground
// Connect SCL to I2C clock pin (UNO: A5, NodeMCU: D1, ESP32: D22)
// Connect SDA to I2C data pin  (UNO: A4, NodeMCU: D2, ESP32: D21)
#include <Wire.h>
#include <Adafruit_HTU21DF.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

Adafruit_HTU21DF htu = Adafruit_HTU21DF();

//const char* mqtt_server = "broker.mqtt-dashboard.com";
const char* mqtt_server = "192.168.1.5";
const char* outTopic = "NTOU-AIOT";
const char* inTopic  = "NTOU-AIOT";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

/*
 * Your WiFi config here
 */
const char* ssid = "Kuo";
const char* password = "0972211921";
/*
 * WiFi init stuff
 */
void startWiFiClient()
{
  Serial.println("Connecting to "+(String)ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  
  randomSeed(micros());
    
  Serial.println("WiFi connected");
  Serial.println("IP address: " + WiFi.localIP().toString());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(outTopic, "MQTT-ESP8266 publishing");
      // ... and resubscribe
      client.subscribe(inTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  startWiFiClient();
  delay(1000);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);  
  client.subscribe("#"); // subscribe anything  
  delay(1000);
  htu.begin();
  delay(10);
}

void loop() { 
  
  float h = htu.readHumidity();
  float t = htu.readTemperature();
  char h_str[6];
  char t_str[6];
  char* msg;
        
  sprintf(msg, "=%s", dtostrf(t, 4, 2, t_str)); 
//  outTopic = "NTOU-AIOT/CC3302/DHT22/Temperature";
  Serial.print(outTopic);     
  Serial.println(msg);

  sprintf(msg, "=%s", dtostrf(h, 4, 2,h_str)); 
//  outTopic = "NTOU-AIOT/CC3302/DHT22/Humidity";
  Serial.print(outTopic);     
  Serial.println(msg);  

  delay(1*3*1000);
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  client.publish(outTopic, msg);
  client.publish(outTopic, msg);  
  // go to deepsleep for 1 minutes
  //system_deep_sleep_set_option(0);
  //system_deep_sleep(1 * 60 * 1000000);
  delay(1*60*1000);
}
