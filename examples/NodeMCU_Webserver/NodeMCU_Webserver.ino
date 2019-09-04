//
// Webserver to receive data from Webclients
// To use a web browser to open IP address of this webserver 
//
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

const char* ssid     = "Your_SSID";
const char* password = "Your_Password";

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
}

void setup() {
  Serial.begin(115200);
  
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

  server.on("/", handleRoot);
  server.on("/dht11", dht11);
  server.on("/htu21", htu21);
  
  Serial.println("HTTP server started");
  server.begin();  
}

void loop() {
  server.handleClient();
}
