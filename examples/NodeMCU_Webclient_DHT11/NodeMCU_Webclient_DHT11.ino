// Webclient to read DHT11 and send data to Webserver
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <DHT.h>

#define DHTPIN 12     // NodeMCU pin D6 connected to DHT11 pin Data

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11 
//define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE, 15);

const char* ssid     = "Kuo";
const char* password = "0972211921";
String      webserverIP = "http://192.168.1.5";

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
  
  String url = webserverIP + "/dht11?";
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
