#include <Arduino.h>
#include <Preferences.h>
#include <SPI.h>
#include <WiFi.h>
#include "Audio.h" //see my repository at github "https://github.com/schreibfaul1/ESP32-audioI2S"

#define I2S_DOUT      25
#define I2S_BCLK      27
#define I2S_LRC       26

Preferences pref;
Audio audio;

String ssid =     "Kuo";
String password = "0972211921";

String stations[] ={
        "0n-80s.radionetz.de:8000/0n-70s.mp3",
        "mediaserv30.live-streams.nl:8000/stream",
        "www.surfmusic.de/m3u/100-5-das-hitradio,4529.m3u",
        "stream.1a-webradio.de/deutsch/mp3-128/vtuner-1a",
        "mp3.ffh.de/radioffh/hqlivestream.aac", //  128k aac
        "www.antenne.de/webradio/antenne.m3u",
        "listen.rusongs.ru/ru-mp3-128",
        "edge.audio.3qsdn.com/senderkw-mp3",
        "macslons-irish-pub-radio.com/media.asx",
};

//some global variables
uint8_t max_volume   = 21;
uint8_t max_stations = 0;   //will be set later
uint8_t cur_station  = 0;   //current station(nr), will be set later
uint8_t cur_volume   = 0;   //will be set from stored preferences
int8_t  cur_btn      =-1;   //current button (, -1 means idle)

enum action{VOLUME_UP=0, VOLUME_DOWN=1, STATION_UP=2, STATION_DOWN=3};

struct _btns{
    uint16_t x; //PosX
    uint16_t y; //PosY
    uint16_t w; //Width
    uint16_t h; //Hight
    uint8_t  a; //Action
    uint8_t  s; //Status
};
typedef _btns btns;

btns btn[4];

// User Interface : 
//      Keys: Volume-Up, Volume-Down,
//            Station-Up, Sation-Down
//      Message: stationName, streamTitle

//**************************************************************************************************
//                                           S E T U P                                             *
//**************************************************************************************************
void setup() {
    max_stations= sizeof(stations)/sizeof(stations[0]); log_i("max stations %i", max_stations);
    Serial.begin(115200);
    pref.begin("WebRadio", false);  // instance of preferences for defaults (station, volume ...)
    if(pref.getShort("volume", 1000) == 1000){ // if that: pref was never been initialized
        pref.putShort("volume", 10);
        pref.putShort("station", 0);
    }
    else{ // get the stored values
        cur_station = pref.getShort("station");
        cur_volume = pref.getShort("volume");
    }
    
    WiFi.mode(WIFI_STA);
    while (WiFi.status() != WL_CONNECTED){
        WiFi.begin(ssid.c_str(), password.c_str());
        delay(1500);
    }
    log_i("Connect to %s", WiFi.SSID().c_str());

    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(cur_volume); // 0...21
    audio.connecttohost(stations[cur_station]);

    Serial.println(cur_volume);
    Serial.println(cur_station);
}
//**************************************************************************************************
//                                            L O O P                                              *
//**************************************************************************************************
void loop()
{
    audio.loop();
}

//**************************************************************************************************
//                                           E V E N T S                                           *
//**************************************************************************************************
void audio_info(const char *info){
    Serial.print("audio_info: "); Serial.println(info);
}
void audio_showstation(const char *info){
    Serial.println(String(info));
}
void audio_showstreamtitle(const char *info){
    String sinfo=String(info);
    sinfo.replace("|", "\n");
    Serial.println(sinfo);
}
