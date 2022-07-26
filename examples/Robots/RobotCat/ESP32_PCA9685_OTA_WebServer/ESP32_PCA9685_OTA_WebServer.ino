
// Import required libraries
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pca9685 = Adafruit_PWMServoDriver(0x40);
#define SERVOMIN  80  // Minimum value
#define SERVOMAX  600 // Maximum value

// Define servo motor connections (expand as required)
#define SERVO_LFu  0 // Left-Front Leg Servo Motor 0 on connector 0
#define SERVO_LFm  1 // Left-Front Leg Servo Motor 1 on connector 1
#define SERVO_LFl  2 // Left-Front Leg Servo Motor 2 on connector 2

#define SERVO_RFu  4 // Right-Front Leg Servo Motor 0 on connector 4
#define SERVO_RFm  5 // Right-Front Leg Servo Motor 1 on connector 5
#define SERVO_RFl  6 // Right-Front Leg Servo Motor 2 on connector 6

#define SERVO_LBu  8 // Left-Front Leg Servo Motor 0 on connector 8
#define SERVO_LBm  9 // Left-Front Leg Servo Motor 1 on connector 9
#define SERVO_LBl  10 // Left-Front Leg Servo Motor 2 on connector 10

#define SERVO_RBu  12 // Right-Front Leg Servo Motor 0 on connector 12
#define SERVO_RBm  13 // Right-Front Leg Servo Motor 1 on connector 13
#define SERVO_RBl  14 // Right-Front Leg Servo Motor 2 on connector 14

// Variables for Servo Motor positions (expand as required)
int posPulse;

// Replace with your network credentials
const char* ssid = "CBN-42738-2.4G";
const char* password = "0972211921";

bool ledState = 0;
const int ledPin = 2;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
  html {
    font-family: Arial, Helvetica, sans-serif;
    text-align: center;
  }
  h1 {
    font-size: 1.8rem;
    color: white;
  }
  h2{
    font-size: 1.5rem;
    font-weight: bold;
    color: #143642;
  }
  .topnav {
    overflow: hidden;
    background-color: #143642;
  }
  body {
    margin: 0;
  }
  .content {
    padding: 30px;
    max-width: 600px;
    margin: 0 auto;
  }
  .card {
    background-color: #F8F7F9;;
    box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
    padding-top:10px;
    padding-bottom:20px;
  }
  .button {
    padding: 15px 50px;
    font-size: 24px;
    text-align: center;
    outline: none;
    color: #fff;
    background-color: #0f8b8d;
    border: none;
    border-radius: 5px;
    -webkit-touch-callout: none;
    -webkit-user-select: none;
    -khtml-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
    user-select: none;
    -webkit-tap-highlight-color: rgba(0,0,0,0);
   }
   /*.button:hover {background-color: #0f8b8d}*/
   .button:active {
     background-color: #0f8b8d;
     box-shadow: 2 2px #CDCDCD;
     transform: translateY(2px);
   }
   .state {
     font-size: 1.5rem;
     color:#8c8c8c;
     font-weight: bold;
   }
  </style>
<title>ESP Web Server</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" href="data:,">
</head>
<body>
  <div class="topnav">
    <h1>ESP32 - PCA9685 driving SG90 x12</h1>
  </div>
  <div class="content">
    <div>
      <img src="https://github.com/rkuo2000/Robotics/blob/gh-pages/images/RobotCat.jpg?raw=true" class="center">
    </div>
    <table>
    <tr>
      <td><p>Left-Front Leg</p></td>
      <td><input type="range" id="servoLFU" name="posLFU" min="0" max="180"><label for="posLFU">upper</label></td>
      <td><input type="range" id="servoLFM" name="posLFM" min="0" max="180"><label for="posLFM">middle</label></td>
      <td><input type="range" id="servoLFL" name="posLFL" min="0" max="180"><label for="posLFL">lower</label></td>
    </tr>
    <tr></tr>
    <tr>
      <td><p>Right-Front Leg</p></td>    
      <td><input type="range" id="servoRFU" name="posRFU" min="0" max="180"><label for="posRFU">upper</label></td>
      <td><input type="range" id="servoRFM" name="posRFM" min="0" max="180"><label for="posRFM">middle</label></td>
      <td><input type="range" id="servoRFL" name="posRFL" min="0" max="180"><label for="posRFL">lower</label></td>
    </tr>
    <tr></tr>
    <tr>
      <td><p>Left-Back Leg</p></td>
      <td><input type="range" id="servoLBU" name="posLBU" min="0" max="180"><label for="posLBU">upper</label></td>
      <td><input type="range" id="servoLBM" name="posLBM" min="0" max="180"><label for="posLBM">middle</label></td>
      <td><input type="range" id="servoLBL" name="posLBL" min="0" max="180"><label for="posLBL">lower</label></td>
    </tr>
    <tr></tr>
    <tr>
      <td><p>Right-Back Leg</p></td>    
      <td><input type="range" id="servoRBU" name="posRBU" min="0" max="180"><label for="posRBU">upper</label></td>
      <td><input type="range" id="servoRBM" name="posRBM" min="0" max="180"><label for="posRBM">middle</label></td>
      <td><input type="range" id="servoRBL" name="posRBL" min="0" max="180"><label for="posRBL">lower</label></td>
    </tr>    
    </table>
  </div>
<script>
  var gateway = `ws://${window.location.hostname}/ws`;
  var websocket;
  window.addEventListener('load', onLoad);
  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage; // <-- add this line
  }
  function onOpen(event) {
    console.log('Connection opened');
  }
  function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
  }
  function onMessage(event) {
    var state;
    if (event.data == "1"){
      state = "ON";
    }
    else{
      state = "OFF";
    }
    document.getElementById('state').innerHTML = state;
  }
  function onLoad(event) {
    initWebSocket();
    initButton();
  }
  function initButton() {
    document.getElementById('button').addEventListener('click', toggle);
  }
  function toggle(){
    websocket.send('toggle');
  }
</script>
</body>
</html>)rawliteral";

void notifyClients() {
  ws.textAll(String(ledState));
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "toggle") == 0) {
      ledState = !ledState;
      notifyClients();
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

String processor(const String& var){
  Serial.println(var);
  if(var == "STATE"){
    if (ledState){
      return "ON";
    }
    else{
      return "OFF";
    }
  }
  return String();
}

void setposSERVO(int pinSERVO, int pos) {
  posPulse = map(pos, 0, 180, SERVOMIN, SERVOMAX);
  pca9685.setPWM(pinSERVO, 0, posPulse); // Write to PCA9685
  delay(100);
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  pca9685.begin(); // Initialize PCA9685  
  pca9685.setPWMFreq(50); // Set PWM Frequency to 50Hz
  
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Start ElegantOTA
  AsyncElegantOTA.begin(&server);
  // Start server
  server.begin();
}

void loop() {
  ws.cleanupClients();
  digitalWrite(ledPin, ledState);
}
