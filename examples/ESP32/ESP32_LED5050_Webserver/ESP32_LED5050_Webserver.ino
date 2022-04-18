// ESP32 LED5050 Webserver

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

#define pinBLUE  27
#define pinRED   14
#define pinGREEN 12

// PWM channel no.  
#define chRED   0
#define chGREEN 1
#define chBLUE  2

#define PWMFREQ 1000
#define PWMRESOLUTION 8 // 8-bit resolution
#define PWMMAXPOWER 255

#define ledTime 20

const char* ssid     = "Your_SSID";
const char* password = "Your_Password";

WebServer server(80);

// Color Picker webpage
const String HTTP_PAGE_HEAD = "<!DOCTYPE html><html><body>";
const String HTTP_PAGE_TITLE = "<h1>LED5050  Color Picker</h1>";
const String HTTP_PAGE_FORM = "<form action=\"/action.php\"><label for=\"selcolor\">Select your color:</label><input type=\"color\" id=\"selcolor\" name=\"selcolor\" value=\"#ff0000\"><br><br><input type=\"submit\"></form>";
const String HTTP_PAGE_END = "</div></body></html>";
const String HTTP_WEBPAGE = HTTP_PAGE_HEAD + HTTP_PAGE_TITLE + HTTP_PAGE_FORM + HTTP_PAGE_END;

void handleRoot() {
  server.send(200, "text/html", "Hello from esp32!");
}

// Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); 
}

void handleColor() {
  server.send(200, "text/html", HTTP_WEBPAGE);
}

void handleAction() {  
  String selcolor = server.arg(0);
  int r, g, b;
  Serial.println();
  Serial.print("received Args : "); Serial.println(server.args());
  for (int i = 0; i < server.args(); i++) {
    Serial.print(server.argName(i)); Serial.print(" "); Serial.println(server.arg(i));
  }

  r = hex2i(selcolor.charAt(1))*16 + hex2i(selcolor.charAt(2));
  g = hex2i(selcolor.charAt(3))*16 + hex2i(selcolor.charAt(4));
  b = hex2i(selcolor.charAt(5))*16 + hex2i(selcolor.charAt(6));          
  Serial.print(r); Serial.print("\t"); Serial.print(g); Serial.print("\t"); Serial.println(b);
  setColor(r,g,b);
  
  server.send(200, "text/html", HTTP_WEBPAGE);
}

int hex2i(char c) {
  if (c>='0' && c<='9') return (c - '0');
  if (c>='a' && c<='f') return ((c - 'a')+10);
  if (c>='A' && c<='F') return ((c - 'A')+10);  
}

void setColor(uint8_t r, uint8_t g, uint8_t b) {
  ledcWrite(chRED,   r);
  ledcWrite(chGREEN, g);
  ledcWrite(chBLUE,  b);
  delay(ledTime);
}

void setup() {
  Serial.begin(115200);
    
  // setup PWM pin and its channel
  pinMode(pinRED,   OUTPUT);
  pinMode(pinGREEN, OUTPUT);
  pinMode(pinBLUE,  OUTPUT);
  ledcSetup(chRED,  PWMFREQ, PWMRESOLUTION);
  ledcSetup(chGREEN,PWMFREQ, PWMRESOLUTION); 
  ledcSetup(chBLUE, PWMFREQ, PWMRESOLUTION); 
  ledcAttachPin(pinRED,   chRED); 
  ledcAttachPin(pinGREEN, chGREEN); 
  ledcAttachPin(pinBLUE,  chBLUE);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/colorpicker", handleColor);
  server.on("/action.php", handleAction);

  server.begin();
  Serial.println("HTTP server started");   
}

void loop() {
  server.handleClient();
}
