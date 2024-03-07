/*
Example guide:
TBD
*/

#include <Arduino.h>
#include "WiFi.h"
#include "StreamIO.h"
#include "AudioStream.h"
#include "AudioEncoder.h"
#include "MP4Recording.h"

#include "AmebaFatFS.h"
#include "Base64.h"

// Default audio preset configurations:
// 0 :  8kHz Mono Analog Mic
// 1 : 16kHz Mono Analog Mic
// 2 :  8kHz Mono Digital PDM Mic
// 3 : 16kHz Mono Digital PDM Mic

AudioSetting configA(0);
Audio audio;
AAC aac;
MP4Recording mp4;
StreamIO audioStreamer1(1, 1);    // 1 Input Audio -> 1 Output AAC
StreamIO audioStreamer2(1, 1);    // 1 Input AAC -> 1 Output MP4

#define FILENAME "TestRecordingAudioOnly.mp4"

char ssid[] = "HITRON-DF90-5G";    // your network SSID (name)
char pass[] = "0972211921";        // your network password
int status = WL_IDLE_STATUS;

char server[] = "192.168.0.166";    // your server IP running HTTP server on PC

AmebaFatFS fs;
WiFiClient wifiClient;

char buf[512];
char *p;

void setup()
{
    Serial.begin(115200);

    // Connection to internet
    while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to WPA SSID: ");
        Serial.println(ssid);
        status = WiFi.begin(ssid, pass);
        delay(2000);
    }

    // Configure audio peripheral for audio data output
    audio.configAudio(configA);
    audio.begin();
    // Configure AAC audio encoder
    aac.configAudio(configA);
    aac.begin();

    // Configure MP4 recording settings
    mp4.configAudio(configA, CODEC_AAC);
    mp4.setRecordingDuration(30);
    mp4.setRecordingFileCount(1);
    mp4.setRecordingFileName("TestRecordingAudioOnly");
    mp4.setRecordingDataType(STORAGE_AUDIO);    // Set MP4 to record audio only

    // Configure StreamIO object to stream data from audio channel to AAC encoder
    audioStreamer1.registerInput(audio);
    audioStreamer1.registerOutput(aac);
    if (audioStreamer1.begin() != 0) {
        Serial.println("StreamIO link start failed");
    }

    // Configure StreamIO object to stream data from AAC encoder to MP4
    audioStreamer2.registerInput(aac);
    audioStreamer2.registerOutput(mp4);
    if (audioStreamer2.begin() != 0) {
        Serial.println("StreamIO link start failed");
    }

    // Start recording MP4 data to SD card
    mp4.begin();

    // printInfo()
    Serial.println("- Audio Information -");
    audio.printInfo();
    Serial.println("- MP4 Recording Information -");
    mp4.printInfo();

    // list files under root directory
    fs.begin();

    memset(buf, 0, sizeof(buf));
    fs.readDir(fs.getRootPath(), buf, sizeof(buf));
    String filepath = String(fs.getRootPath()) + String(FILENAME);
    /* the filenames are separated with '\0', so we scan one by one */
    p = buf;
    while (strlen(p) > 0) {
        /* list out file name image will be saved as "TestRecordingAudioOnly.mp4"*/
        if (strstr(p, FILENAME) != NULL) {
            Serial.println("[INFO] Found 'TestRecordingAudioOnly.mp4' in the string.");
            Serial.println("[INFO] Processing file...");
        } else {
            // Serial.println("Substring 'image.jpg' not found in the
            // string.");
        }
        p += strlen(p) + 1;
    }

    uint8_t *fileinput;
    File file = fs.open(filepath);
    unsigned int fileSize = file.size();
    fileinput = (uint8_t *)malloc(fileSize + 1);
    file.read(fileinput, fileSize);
    fileinput[fileSize] = '\0';
    file.close();
    fs.end();

    // Encode the file data as Base64
    int encodedLen = base64_enc_len(fileSize);
    char *encodedData = (char *)malloc(encodedLen);
    base64_encode(encodedData, (char *)fileinput, fileSize);

    // Print or use the encoded data as needed
    // Serial.println("Base64 Encoded Data:");
    // Serial.println(encodedData);
    // delay(1000);

    if (wifiClient.connect(server, 8000)) {
        // wifiClient.println("POST /audio HTTP/1.1");
        // wifiClient.println("Host: " + String(server));
        // wifiClient.println("Content-Type: multipart/form-data; boundary=boundary");
        // wifiClient.println("Connection: keep-alive");

        // // Calculate content length
        // size_t contentLength = encodedLen + strlen("--boundary--\r\n");
        // wifiClient.print("Content-Length: ");
        // wifiClient.println(contentLength);

        // // Send multipart/form-data body
        // wifiClient.println("--boundary");
        // wifiClient.println("Content-Disposition: form-data; name=\"audio\"; filename=\"TestRecordingAudioOnly.mp4\"");
        // wifiClient.println("Content-Type: audio/mp4");
        // wifiClient.println();
        // wifiClient.print(encodedData);
        // wifiClient.println();
        // wifiClient.println("--boundary--");
        // Serial.println("Binary sent");

        wifiClient.println("POST /audio HTTP/1.1");
        wifiClient.println("Host: " + String(server));
        wifiClient.println("Content-Type: application/x-www-form-urlencoded");    // Use appropriate content type
        wifiClient.println("Content-Length: " + String(encodedLen));              // Specify the length of the content
        wifiClient.println("Connection: keep-alive");
        wifiClient.println();             // Empty line indicates the end of headers
        wifiClient.print(encodedData);    // Send the Base64 encoded audio data directly
        Serial.println("Binary sent");
    }
}

void loop()
{
    // if there are incoming bytes available
    // from the server, read them and print them:
    while (wifiClient.available()) {
        char c = wifiClient.read();
        Serial.write(c);
    }

    // if the server's disconnected, stop the client:
    if (!wifiClient.connected()) {
        Serial.println();
        Serial.println("disconnecting from server.");
        wifiClient.stop();

        // do nothing forevermore:
        while (true)
            ;
    }
}
