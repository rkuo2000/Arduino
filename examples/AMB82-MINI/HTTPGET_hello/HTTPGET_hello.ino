/*

 Example guide:
 
 1. To run HTTP Server
 https://github.com/rkuo2000/GenAI/blob/main/Audio-To-Text/whisper_server.py
 python whisper_server.py
 
 2. To do HTTP GET, use browser to open this URL 
 http://192.168.0.166:8000/hello
  
 */

#include <WiFi.h>

char ssid[] = "Your_SSID";    // your network SSID (name)
char pass[] = "Your_Password";        // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                // your network key Index number (needed only for WEP)
int status = WL_IDLE_STATUS;     // Indicator of Wifi status

// IPAddress server(64,233,189,94);  // numeric IP for Google (no DNS)
//char server[] = "www.google.com";    // name address for Google (using DNS)
char server[]= "192.168.0.166"; // your server IP runing HTTP server on PC

WiFiClient client;
void setup()
{
    // Initialize serial and wait for port to open:
    Serial.begin(115200);
    while (!Serial) {
        ;
    }

    // attempt to connect to Wifi network:
    while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
        // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
        status = WiFi.begin(ssid, pass);

        // wait 10 seconds for connection:
        delay(10000);
    }
    Serial.println("Connected to wifi");
    printWifiStatus();

    Serial.println("\nStarting connection to server...");
    // if you get a connection, report back via serial:
    if (client.connect(server, 8000)) {
        Serial.println("connected to server");
        // Make a HTTP request:
        client.println("GET /hello HTTP/1.1");
        client.println();
    }
    delay(100);
}

void loop()
{
    // if there are incoming bytes available
    // from the server, read them and print them:
    while (client.available()) {
        char c = client.read();
        Serial.write(c);
    }

    // if the server's disconnected, stop the client:
    if (!client.connected()) {
        Serial.println();
        Serial.println("disconnecting from server.");
        client.stop();

        // do nothing forevermore:
        while (true)
            ;
    }
}

void printWifiStatus()
{
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // print your WiFi IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);

    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.print(rssi);
    Serial.println(" dBm");
}
