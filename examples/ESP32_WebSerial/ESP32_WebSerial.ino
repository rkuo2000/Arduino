#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>

AsyncWebServer server(80);

const char* ssid = "Kuo";
const char* password = "0972211921";

const char* PARAM_MESSAGE = "message";

void recvMsg(uint8_t *data, size_t len){
  Serial.println("Received Data...");
  String d = "";
  for(int i=0; i < len; i++){
    d += char(data[i]);
  }
  Serial.println(d);
}

void setup() {   
  Serial.begin(115200); 
  WiFi.mode(WIFI_STA); 
  WiFi.begin(ssid, password);
     
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Could not initialize WiFi");
    return;
  }     
  Serial.println(WiFi.localIP());
  
  // WebSerial is accessible at "<IP Address>/webserial" in browser  
  WebSerial.begin(&server);
  WebSerial.msgCallback(recvMsg);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Hello, world");
  });

  // Send a GET request to <IP>/get?message=<message>
  // Ex. https://github.com/rkuo2000/homebot/blob/master/sendHTTPget.py
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String message;
    if (request->hasParam(PARAM_MESSAGE)) {
      message = request->getParam(PARAM_MESSAGE)->value();
    } else {
      message = "No message sent";
    }
      request->send(200, "text/plain", "Hello, GET: " + message);
  });  

  // Send a POST request to <IP>/post with a form field message set to <message>
  // Ex. https://github.com/rkuo2000/homebot/blob/master/sendHTTPpost.py
  server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request){
    String message;
    if (request->hasParam(PARAM_MESSAGE, true)) {
      message = request->getParam(PARAM_MESSAGE, true)->value();
    } else {
      message = "No message sent";
    }
    request->send(200, "text/plain", "Hello, POST: " + message);
  });  
  
  server.begin();
}
 
void loop() {
  WebSerial.println("Hello World");
  delay(1000);
}
