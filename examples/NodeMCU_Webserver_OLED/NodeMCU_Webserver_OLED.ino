//
// Webserver to receive data from Webclients and display on 0.9" OLED (SSD1306)
// To use a web browser to open IP address of this webserver 
//
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

#define OLED_RESET LED_BUILTIN
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET); // 0.9" OLED 128x64: Width = 128, Height = 64

const char* ssid     = "Kuo";
const char* password = "0972211921";

ESP8266WebServer server(80);

const String HTTP_PAGE_HEAD  = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>{v}</title>";
const String HTTP_PAGE_STYLE = "<style>.c{text-align: center;} div,input{padding:5px;font-size:1em;}  input{width:90%;}  body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.6rem;background-color:#1fb3ec;color:#fdd;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;} .button2 {background-color: #008CBA;} .button3 {background-color: #f44336;} .button4 {background-color: #e7e7e7; color: black;} .button5 {background-color: #555555;} .button6 {background-color: #4CAF50;} </style>";
const String HTTP_PAGE_SCRIPT= "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>";
const String HTTP_PAGE_BODY  = "</head><body><div style='text-align:left;display:inline-block;min-width:260px;'>";
const String HTTP_WEBPAGE = HTTP_PAGE_HEAD + HTTP_PAGE_STYLE + HTTP_PAGE_SCRIPT + HTTP_PAGE_BODY;

const String HTTP_PAGE_END = "</div></body></html>";

// DHT11
String dht11_name0 = "Temperature";
String dht11_name1 = "Humidity   ";
String dht11_value0= "0 ";
String dht11_value1= "0 ";
// HTU21DF
String htu21_name0 = "Temperature";
String htu21_name1 = "Humidity   ";
String htu21_value0= "0 ";
String htu21_value1= "0 ";

void handleRoot() {
  // Display Sensor Status
  String s  = HTTP_WEBPAGE;
         s += "<table border=\"1\"";
         s += "<tr><th align='center'>DHT11 Sensor</th><th align='cener'>value</th></tr>";
         s += "<tr><td align='center'>"+dht11_name0+"</td><td align='center'>"+dht11_value0+"</td></tr>";
         s += "<tr><td align='center'>"+dht11_name1+"</td><td align='center'>"+dht11_value1+"</td></tr>";
         s += "<tr><th align='center'>HTU21 Sensor</th><th align='cener'>value</th></tr>";
         s += "<tr><td align='center'>"+htu21_name0+"</td><td align='center'>"+htu21_value0+"</td></tr>";
         s += "<tr><td align='center'>"+htu21_name1+"</td><td align='center'>"+htu21_value1+"</td></tr>";
         s += "</tr></table>";
         s += HTTP_PAGE_END;
         
  server.send(200, "text/html", s);
}

void OLED_display() {
  // OLED display print
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("DHT11");
  display.println("Temp = "+dht11_value0);
  display.println("Humid= "+dht11_value1);
  display.println("");
  display.println("HTU21");
  display.println("Temp = "+htu21_value0);
  display.println("Humid= "+htu21_value1);
  display.display();
}

// http://192.168.xx.xx/dht11?T=28&H=50 from Webclient_DHT11 
//(you can open a browser to test it, too)
void dht11() {
  String message = "Number of args received:";
  message += server.args();                   //Get number of parameters
  message += "\n";                            //Add a new line

  for (int i = 0; i < server.args(); i++) {
    message += "Arg "+(String)i + " –> "; //Include the current iteration value
    message += server.argName(i) + ": ";      //Get the name of the parameter
    message += server.arg(i) + "\n";          //Get the value of the parameter
  }
  Serial.print(message);

  dht11_value0=server.arg(0);
  dht11_value1=server.arg(1);
  
  String s  = HTTP_WEBPAGE;
         s += "<table border=\"1\"";
         s += "<tr><th align='center'>DHT11 Sensor</th><th align='cener'>value</th></tr>";
         s += "<tr><td align='center'>"+dht11_name0+"</td><td align='center'>"+dht11_value0+"</td></tr>";
         s += "<tr><td align='center'>"+dht11_name1+"</td><td align='center'>"+dht11_value1+"</td></tr>";
         s += "<tr><th align='center'>HTU21 Sensor</th><th align='cener'>value</th></tr>";
         s += "<tr><td align='center'>"+htu21_name0+"</td><td align='center'>"+htu21_value0+"</td></tr>";
         s += "<tr><td align='center'>"+htu21_name1+"</td><td align='center'>"+htu21_value1+"</td></tr>";
         s += "</tr></table>";
         s += HTTP_PAGE_END; 
	 
  server.send(200, "text/html", s);
  OLED_display();
}

// http://192.168.yy.yy/htu21?T=28&H=50 from Webclient_HTU21
//(you can open a browser to test it, too)
void htu21() {
  String message = "Number of args received:";
  message += server.args();                   //Get number of parameters
  message += "\n";                            //Add a new line

  for (int i = 0; i < server.args(); i++) {
    message += "Arg "+(String)i + " –> "; //Include the current iteration value
    message += server.argName(i) + ": ";      //Get the name of the parameter
    message += server.arg(i) + "\n";          //Get the value of the parameter
  }
  Serial.print(message);

  htu21_value0=server.arg(0);
  htu21_value1=server.arg(1);
  
  String s  = HTTP_WEBPAGE;
         s += "<table border=\"1\"";
         s += "<tr><th align='center'>DHT11 Sensor</th><th align='cener'>value</th></tr>";
         s += "<tr><td align='center'>"+dht11_name0+"</td><td align='center'>"+dht11_value0+"</td></tr>";
         s += "<tr><td align='center'>"+dht11_name1+"</td><td align='center'>"+dht11_value1+"</td></tr>";
         s += "<tr><th align='center'>HTU21 Sensor</th><th align='cener'>value</th></tr>";
         s += "<tr><td align='center'>"+htu21_name0+"</td><td align='center'>"+htu21_value0+"</td></tr>";
         s += "<tr><td align='center'>"+htu21_name1+"</td><td align='center'>"+htu21_value1+"</td></tr>";
         s += "</tr></table>";
         s += HTTP_PAGE_END; 
   
  server.send(200, "text/html", s);
  OLED_display();
}

void setup() {
  Serial.begin(115200);

  // OLED display Init
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // initialize with the I2C addr 0x3C
  display.clearDisplay(); // Clear the buffer.
  display.setTextSize(1); // set Text Size
  
  // OLED display print
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Webserver ON");
  display.display();
  delay(2000); // Pause for 2 seconds
  display.clearDisplay();
      
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");  
  Serial.println(ssid);
  // OLED display print
  display.setCursor(0,0);          
  display.print("Connecting to ");                                 
  display.println(ssid);
  display.display(); 
  delay(2000); 
  display.clearDisplay();
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");               
  Serial.println("WiFi connected"); 
  Serial.println("IP address: ");   
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/dht11", dht11);
  server.on("/htu21", htu21);
  
  Serial.println("HTTP server started"); 
  server.begin(); 
  
  // OLED display print
  display.setCursor(0,0);
  display.println("WiFi connected");
  display.print("IP addr: ");
  display.println(WiFi.localIP());
  display.println(""); 
  display.println("HTTP server started"); 
  display.display();                    
}

void loop() {
  server.handleClient();
}
