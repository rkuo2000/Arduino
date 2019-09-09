// Webclient to read DHT22 and send data to Webserver
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <DHT.h>

#define DHTPIN D6     // NodeMCU pin D6 connected to DHT22 Data pin
DHT dht(DHTPIN, DHT22, 15); 

const char* ssid     = "Your_SSID";
const char* password = "Your_Password";
String      webserverIP = "http://192.168.1.5"; // Your Webserver IP address

void setup() {
  Serial.begin(115200);
 
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
}

void loop() {
  delay(5000);

  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();
  Serial.print(temp);
  Serial.print(" ");
  Serial.print(humidity);
  Serial.println();
  
  String url = webserverIP + "/dht22?";
  url += "T=";
  url += String(temp);
  url += "&H=";
  url += String(humidity);
  
  if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status

     WiFiClient client;

     HTTPClient http;    //Declare object of class HTTPClient
 
     http.begin(client, url);    //Specify request destination
     http.addHeader("Content-Type", "text/plain");  //Specify content-type header
 
     int httpCode = http.POST("Message from ESP8266");   //Send the request
     String payload = http.getString();                  //Get the response payload
 
     Serial.println(httpCode);   //Print HTTP return code
     Serial.println(payload);    //Print request response payload

     http.end();  //Close connection
 
   }else{
      Serial.println("Error in WiFi connection");   
   }
  
  Serial.println();
  Serial.println("closing connection. going to sleep...");
  // go to deepsleep for 1 minutes
  //system_deep_sleep_set_option(0);
  //system_deep_sleep(1 * 60 * 1000000);
  delay(1*60*1000);
}
