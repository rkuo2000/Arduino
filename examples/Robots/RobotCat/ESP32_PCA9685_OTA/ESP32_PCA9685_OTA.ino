
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
#define SERVO_LF  0  // Left-Front Leg Servo Motor 0 on connector 0,1,2
#define SERVO_RF  4  // Right-Front Leg Servo Motor 0 on connector 4,5,6
#define SERVO_LB  8  // Left-Front Leg Servo Motor 0 on connector 8,9,10
#define SERVO_RB  12 // Right-Front Leg Servo Motor 0 on connector 12,13,14

// WiFi-AP's SSID & Password
const char* ssid = "CBN-42738-2.4G";
const char* password = "0972211921";

bool ledState = 0;
const int ledPin = 2; // ESP32 built-in LED

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

const char index_html[] PROGMEM = R"rawpage(
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
  .topdiv {
    overflow: hidden;
    background-color: #143642;
  }
  .content {
    padding: 30px;
    max-width: 720px;
    margin: 0 auto;
  }
  .button {
    padding: 15px 25px;
    font-size: 16px;
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
  </style>
<title>ESP Web Server</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" href="data:,">
</head>
<body>
  <div class="topdiv">
    <h1>ESP32 - PCA9685 to SG90 x12</h1>
  </div>
  <div class="content">
    <div>
      <table>
      <tr>
      <td></td>
      <td>Upper</td>
      <td></td>
      <td>Middle</td>
      <td></td>
      <td>Lower</td>
      <td></td>
      <td></td>
      </tr>      
      <tr>
      <td><h2>Left Front</h2></td>
      <td><input type="range" id="servoLFU" name="posLFU" min="0" max="180" value="0"  ></td>
      <td><p id="valLFU">0~180</p></td>      
      <td><input type="range" id="servoLFM" name="posLFM" min="0" max="180" value="0"  ></td>
      <td><p id="valLFM">0~180</p></td>
      <td><input type="range" id="servoLFL" name="posLFL" min="0" max="180" value="180"></td>
      <td><p id="valLFL">0~180</p></td>      
      <td><p><button id="btnLF" class="button">Set</button></p></td>
      </tr>
      <tr>
      <td><h2>Right Front</h2></td>
      <td><input type="range" id="servoRFU" name="posRFU" min="0" max="180" value="0"  ></td>
      <td><p id="valRFU">0~180</p></td>      
      <td><input type="range" id="servoRFM" name="posRFM" min="0" max="180" value="180"></td>
      <td><p id="valRFM">0~180</p></td>
      <td><input type="range" id="servoRFL" name="posRFL" min="0" max="180" value="0"  ></td>
      <td><p id="valRFL">0~180</p></td>      
      <td><p><button id="btnRF" class="button">Set</button></p></td>
      </tr>
      <tr>
      <td><h2>Left Back</h2></td>
      <td><input type="range" id="servoLBU" name="posLBU" min="0" max="180" value="0"  ></td>
      <td><p id="valLBU">0~180</p></td>      
      <td><input type="range" id="servoLBM" name="posLBM" min="0" max="180" value="0"  ></td>
      <td><p id="valLBM">0~180</p></td>
      <td><input type="range" id="servoLBL" name="posLBL" min="0" max="180" value="180"></td>
      <td><p id="valLBL">0~180</p></td>      
      <td><p><button id="btnLB" class="button">Set</button></p></td>
      </tr>
      <tr>
      <td><h2>Right Back</h2></td>
      <td><input type="range" id="servoRBU" name="posRBU" min="0" max="180" value="0"  ></td>
      <td><p id="valRBU">0~180</p></td>      
      <td><input type="range" id="servoRBM" name="posRBM" min="0" max="180" value="180"></td>
      <td><p id="valRBM">0~180</p></td>
      <td><input type="range" id="servoRBL" name="posRBL" min="0" max="180" value="0"  ></td>
      <td><p id="valRBL">0~180</p></td>      
      <td><p><button id="btnRB" class="button">Set</button></p></td>
      </tr> 
      </table>
    </div>
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
    document.getElementById('btnLF').addEventListener('click', setPosLF);
    document.getElementById('btnRF').addEventListener('click', setPosRF);
    document.getElementById('btnLB').addEventListener('click', setPosLB);
    document.getElementById('btnRB').addEventListener('click', setPosRB);            
  }
  function setPosLF(){
    var lfu = document.getElementById('servoLFU').value;
    var lfm = document.getElementById('servoLFM').value;
    var lfl = document.getElementById('servoLFL').value;
    document.getElementById("valLFU").innerHTML = lfu; 
    document.getElementById("valLFM").innerHTML = lfm;
    document.getElementById("valLFL").innerHTML = lfl;            
    websocket.send('LF'+','+lfu.toString()+','+lfm.toString()+','+lfl.toString());
  }
  function setPosRF(){
    var rfu = document.getElementById('servoRFU').value;
    var rfm = document.getElementById('servoRFM').value;
    var rfl = document.getElementById('servoRFL').value;
    document.getElementById("valRFU").innerHTML = rfu; 
    document.getElementById("valRFM").innerHTML = rfm;
    document.getElementById("valRFL").innerHTML = rfl;            
    websocket.send('RF'+','+rfu.toString()+','+rfm.toString()+','+rfl.toString());
  } 
  function setPosLB(){
    var lbu = document.getElementById('servoLBU').value;
    var lbm = document.getElementById('servoLBM').value;
    var lbl = document.getElementById('servoLBL').value;
    document.getElementById("valLBU").innerHTML = lbu; 
    document.getElementById("valLBM").innerHTML = lbm;
    document.getElementById("valLBL").innerHTML = lbl;            
    websocket.send('LB'+','+lbu.toString()+','+lbm.toString()+','+lbl.toString());
  }
  function setPosRB(){
    var rbu = document.getElementById('servoRBU').value;
    var rbm = document.getElementById('servoRBM').value;
    var rbl = document.getElementById('servoRBL').value;
    document.getElementById("valRBU").innerHTML = rbu; 
    document.getElementById("valRBM").innerHTML = rbm;
    document.getElementById("valRBL").innerHTML = rbl;            
    websocket.send('RB'+','+rbu.toString()+','+rbm.toString()+','+rbl.toString());
  }   
</script>
</body>
</html>)rawpage";

void notifyClients() {
  ws.textAll(String(ledState));
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  char *pch;
  int servoPin;

  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    Serial.printf("%s\n", (char*)data);

    //if (strcmp((char*)data, "toggle") == 0) {
    //  ledState = !ledState;
    //  notifyClients(); 
    //}

    // set first servoPin = 0,4,8,12
    pch = strtok((char*)data," ,.-");
    //Serial.printf ("%s\n",pch);    
    if (strcmp(pch, "LF") == 0) { servoPin = SERVO_LF; }
    if (strcmp(pch, "RF") == 0) { servoPin = SERVO_RF; }
    if (strcmp(pch, "LB") == 0) { servoPin = SERVO_LB; }
    if (strcmp(pch, "RB") == 0) { servoPin = SERVO_RB; }

    pch = strtok (NULL, " ,.-");
    setposSERVO(servoPin, atoi(pch));
    
    pch = strtok (NULL, " ,.-");
    setposSERVO(servoPin+1, atoi(pch));
    
    pch = strtok (NULL, " ,.-");
    setposSERVO(servoPin+2, atoi(pch)); 
       
    Serial.printf("Servo setting finished !!!\n");   
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
  int pulse = map(pos, 0, 180, SERVOMIN, SERVOMAX);
  pca9685.setPWM(pinSERVO, 0, pulse); // Write to PCA9685
  Serial.print("Servo "); Serial.print(pinSERVO); 
  Serial.print(" = ");    Serial.print(pos);
  Serial.print(", pulse ="); Serial.println(pulse);  
  delay(30);
  pca9685.setPWM(pinSERVO, 0, 0); // set PWM output Low 
}

void setup(){  
  Serial.begin(115200); // Serial port for debugging purposes
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
