//
// IRremote Http Server for Kbro TV box
//
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
 
// Set your network SSID and Password
const char* ssid = "Kuo";
const char* password = "0972211921";
 
ESP8266WebServer server(80);   //instantiate server at port 80 (http port)

const String HTTP_HEAD_BEGIN = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>Kbro Remote</title>]";
const String HTTP_STYLE  = "<style>.c{text-align: center;} div,input{padding:5px;font-size:1em;}  input{width:90%;}  body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.6rem;background-color:#3b5998;color:#dfe3ee;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;} .buttonboxon {background-color: #dfe3ee; color:#3b5998} .buttonsmod {background-color: #8b9dc3; color:#DB4437} .buttonokup {background-color: #8b9dc3; color:#dfe3ee} .buttonokdn {background-color: #8b9dc3; color:#dfe3ee} .buttonoklt {background-color: #8b9dc3; color:#dfe3ee} .buttonokrt {background-color: #8b9dc3; color:#dfe3ee} .buttonok {background-color: #8b9dc3; color:#dfe3ee}  .buttonexit {background-color: #8b9dc3; color:#3b5998} .buttonreturn {background-color: #8b9dc3; color:#3b5998} } </style>";
const String HTTP_HEAD_END= "</head>";
const String BODY_TABLE = "<body><div style=\"text-align:center; display:inline-block;min-width:240px;\">";
const String TABLE_ROW0 = "<table><tr><td><form action=\"/cmdboxon\" method=\"get\"><button style=\"width: 248px; height: 30px; font-size: 25px\" class=\"buttonboxon\">Kbro Box</button></form></td></tr></table><table>";
const String TABLE_ROW1 = "<tr><td><form action=\"/cmd1\" method=\"get\"><button style=\"width: 80px; height: 60px; font-size: 60px\" class=\"button1\">1</button></form></td><td><form action=\"/cmd2\" method=\"get\"><button style=\"width: 80px; height: 60px; font-size: 60px\" class=\"button2\">2</button></form></td><td><form action=\"/cmd3\" method=\"get\"><button style=\"width: 80px; height: 60px; font-size: 60px\" class=\"button3\">3</button></form></td></tr>";
const String TABLE_ROW2 = "<tr><td><form action=\"/cmd4\" method=\"get\"><button style=\"width: 80px; height: 60px; font-size: 60px\" class=\"button4\">4</button></form></td><td><form action=\"/cmd5\" method=\"get\"><button style=\"width: 80px; height: 60px; font-size: 60px\" class=\"button5\">5</button></form></td><td><form action=\"/cmd6\" method=\"get\"><button style=\"width: 80px; height: 60px; font-size: 60px\" class=\"button6\">6</button></form></td></tr>";
const String TABLE_ROW3 = "<tr><td><form action=\"/cmd7\" method=\"get\"><button style=\"width: 80px; height: 60px; font-size: 60px\" class=\"button7\">7</button></form></td><td><form action=\"/cmd8\" method=\"get\"><button style=\"width: 80px; height: 60px; font-size: 60px\" class=\"button8\">8</button></form></td><td><form action=\"/cmd9\" method=\"get\"><button style=\"width: 80px; height: 60px; font-size: 60px\" class=\"button9\">9</button></form></td></tr>";
const String TABLE_ROW4 = "<tr><td><form action=\"/cmdchdown\" method=\"get\"><button style=\"width: 80px; height: 60px; font-size: 60px\" class=\"buttonchdn\">-</button></form></td><td><form action=\"/cmd0\" method=\"get\"><button style=\"width: 80px; height: 60px; font-size: 60px\" class=\"button0\">0</button></form></td><td><form action=\"/cmdchup\" method=\"get\"><button style=\"width: 80px; height: 60px; font-size: 60px\" class=\"buttonchup\">+</button></form></td></tr>";
const String TABLE_ROW5 = "<tr><td><form action=\"/cmdvoldown\" method=\"get\"><button style=\"width: 80px; height: 60px; font-size: 25px\" class=\"buttonvoldn\">(((</button></form></td><td><form action=\"/cmdmute\" method=\"get\"><button style=\"width: 80px; height: 60px; font-size: 15px\" class=\"buttonmute\">Mute</button></form></td><td><form action=\"/cmdvolup\" method=\"get\"><button style=\"width: 80px; height: 60px; font-size: 25px\" class=\"buttonvolup\">)))</button></form></td></tr></table><table>";
const String TABLE_ROW6 = "<tr><td><form action=\"/cmdsmod\" method=\"get\"><button style=\"width: 80px; height: 40px; font-size: 15px\" class=\"buttonsmod\">SMOD</button></form></td><td><form action=\"/cmdokup\" method=\"get\"><button style=\"width: 80px; height: 40px; font-size: 15px\" class=\"buttonokup\"> up </button></form></td><td></td><td></td></tr>";
const String TABLE_ROW7 = "<tr><td><form action=\"/cmdoklt\" method=\"get\"><button style=\"width: 80px; height: 40px; font-size: 15px\" class=\"buttonoklt\">left</button></form></td><td><form action=\"/cmdok\" method=\"get\"><button style=\"width: 80px; height: 40px; font-size: 15px\" class=\"buttonok\"> OK </button></form></td><td><form action=\"/cmdokrt\" method=\"get\"><button style=\"width: 80px; height: 40px; font-size: 15px\" class=\"buttonokrt\">right</button></form></td></tr>";
const String TABLE_ROW8 = "<tr><td><form action=\"/cmdexit\" method=\"get\"><button style=\"width: 80px; height: 40px; font-size: 15px\" class=\"buttonexit\">exit</button></form></td><td><form action=\"/cmdokdn\" method=\"get\"><button style=\"width: 80px; height: 40px; font-size: 15px\" class=\"buttonokdn\">down</button></form></td><td><form action=\"/cmdreturn\" method=\"get\"><button style=\"width: 80px; height: 40px; font-size: 15px\" class=\"buttonreturn\">retrun</button></form></td></tr>";
const String BODY_END   = "</table></div></body></html>";
const String HOMEPAGE = HTTP_HEAD_BEGIN + HTTP_STYLE + HTTP_HEAD_END + BODY_TABLE + TABLE_ROW0 + TABLE_ROW1 + TABLE_ROW2 + TABLE_ROW3 + TABLE_ROW4 + TABLE_ROW5 + TABLE_ROW6 + TABLE_ROW7 + TABLE_ROW8 + BODY_END;

const uint16_t IRLed = D2;  // ESP8266 GPIO pin to use.

IRsend irsend(IRLed);  

// Code data captured by IRrecvDumpV2.ino
uint16_t BOXON_a[35] = {226, 912,  204, 2264,  204, 770,  204, 2806,  204, 1314,  202, 1314,  204, 1450,  204, 1856,  204, 14236,  224, 906,  202, 772,  204, 770,  204, 770,  204, 772,  204, 2806,  204, 772,  204, 770,  204};  // UNKNOWN CF7964EB
uint16_t BOXON_b[35] = {226, 912,  204, 2262,  204, 770,  204, 2806,  204, 1316,  204, 1314,  204, 1450,  202, 1858,  204, 14234,  250, 880,  204, 1856,  214, 1846,  206, 768,  204, 770,  204, 2804,  204, 772,  230, 744,  204};  // UNKNOWN F09C641D

uint16_t NO_0_a[35] = {240, 880,  234, 2232,  236, 740,  234, 2774,  236, 1284,  234, 1282,  234, 1418,  236, 1824,  234, 14204,  256, 876,  236, 2774,  234, 742,  232, 742,  234, 742,  234, 740,  234, 740,  234, 740,  234};  // UNKNOWN 41CB412B
uint16_t NO_0_b[35] = {238, 882,  234, 2230,  238, 738,  234, 2774,  234, 1286,  234, 1282,  234, 1420,  234, 1826,  236, 14222,  234, 878,  234, 1688,  234, 1826,  234, 742,  234, 742,  234, 740,  234, 740,  234, 740,  234};  // UNKNOWN 98B576B7

uint16_t NO_1[35] = {226, 910,  230, 2236,  204, 770,  232, 2778,  204, 1314,  232, 1286,  204, 1450,  232, 1830,  230, 14206,  254, 880,  232, 2642,  232, 744,  230, 744,  232, 742,  232, 880,  230, 744,  204, 770,  230};  // UNKNOWN 41CB412B

uint16_t NO_2_a[35] = {256, 882,  234, 2232,  236, 740,  234, 2776,  234, 1282,  232, 1286,  234, 1420,  234, 1824,  236, 14202,  256, 876,  234, 2504,  234, 740,  236, 740,  234, 740,  236, 1012,  234, 740,  234, 740,  234};  // UNKNOWN 99B22E91
uint16_t NO_2_b[35] = {256, 878,  234, 2232,  234, 740,  234, 2776,  234, 1284,  234, 1284,  236, 1418,  234, 1826,  234, 14202,  256, 878,  232, 1420,  234, 1826,  236, 738,  236, 738,  234, 1012,  234, 740,  232, 742,  234};  // UNKNOWN AE898A1C

uint16_t NO_3_a[35] = {238, 880,  234, 2230,  236, 740,  234, 2774,  236, 1282,  234, 1282,  236, 1418,  236, 1824,  236, 14222,  238, 876,  236, 2366,  234, 742,  232, 742,  234, 742,  234, 1146,  234, 740,  234, 740,  234};  // UNKNOWN 99B22E91
uint16_t NO_3_b[35] = {258, 880,  234, 2232,  236, 740,  234, 2772,  236, 1282,  236, 1282,  234, 1418,  236, 1824,  236, 14202,  258, 876,  234, 1282,  236, 1824,  234, 742,  234, 742,  234, 1146,  236, 740,  234, 742,  234};  // UNKNOWN AE898A1C

uint16_t NO_4_a[35] = {226, 910,  204, 2264,  202, 772,  202, 2808,  204, 1314,  202, 1314,  202, 1452,  204, 1856,  202, 14234,  226, 906,  204, 2264,  202, 772,  204, 772,  202, 772,  202, 1314,  204, 772,  202, 772,  204}; // UNKNOWN 99B22E91
uint16_t NO_4_b[35] = {226, 912,  202, 2264,  204, 772,  202, 2806,  204, 1314,  202, 1314,  204, 1450,  202, 1858,  204, 14232,  226, 906,  204, 1178,  204, 1858,  202, 770,  204, 772,  202, 1314,  204, 772,  202, 772,  202}; // UNKNOWN AE898A1C

uint16_t NO_5_a[35] = {254, 882,  230, 2236,  234, 742,  230, 2778,  232, 1286,  230, 1286,  232, 1422,  232, 1828,  232, 14206,  256, 878,  232, 2100,  230, 744,  232, 742,  232, 742,  230, 1422,  232, 742,  232, 744,  232};  // UNKNOWN 99B22E91
uint16_t NO_5_b[35] = {254, 882,  232, 2236,  232, 744,  232, 2776,  232, 1284,  234, 1286,  230, 1422,  232, 1828,  232, 14204,  256, 878,  230, 1014,  232, 1828,  232, 742,  230, 744,  230, 1426,  226, 744,  230, 744,  230};  // UNKNOWN 3E649815

uint16_t NO_6_a[35] = {226, 912,  204, 2262,  206, 770,  202, 2806,  204, 1314,  204, 1314,  204, 1450,  204, 1856,  204, 14234,  226, 908,  204, 1992,  204, 772,  204, 770,  202, 772,  204, 1584,  204, 772,  202, 772,  202};  // UNKNOWN 99B22E91
uint16_t NO_6_b[35] = {228, 912,  202, 2264,  204, 772,  202, 2808,  202, 1314,  204, 1314,  204, 1450,  204, 1856,  204, 14232,  228, 906,  202, 906,  204, 1856,  204, 770,  204, 772,  202, 1586,  202, 772,  204, 772,  202};  // UNKNOWN 3E649815

uint16_t NO_7_a[35] = {238, 880,  234, 2232,  236, 740,  236, 2772,  236, 1282,  236, 1284,  234, 1418,  234, 1826,  234, 14222,  236, 876,  236, 1824,  234, 742,  234, 738,  236, 740,  234, 1690,  234, 740,  236, 740,  234};  // UNKNOWN 99B22E91
uint16_t NO_7_b[35] = {256, 874,  234, 2232,  236, 744,  232, 2774,  234, 1282,  236, 1282,  234, 1420,  234, 1826,  236, 14202,  256, 876,  236, 740,  234, 1824,  236, 740,  234, 740,  236, 1688,  236, 740,  234, 740,  236};  // UNKNOWN 3E649815

uint16_t NO_8_a[35] = {228, 910,  204, 2264,  202, 772,  202, 2806,  202, 1316,  202, 1314,  204, 1448,  204, 1856,  204, 14232,  228, 908,  204, 1720,  202, 772,  202, 770,  206, 770,  204, 1856,  206, 770,  202, 772,  202}; // UNKNOWN 99B22E91
uint16_t NO_8_b[35] = {226, 912,  202, 2264,  204, 774,  200, 2806,  202, 1314,  204, 1314,  204, 1450,  202, 1858,  204, 14232,  226, 906,  204, 2806,  204, 1856,  204, 770,  204, 772,  204, 1856,  202, 772,  204, 772,  204}; // UNKNOWN C68A87BA

uint16_t NO_9_a[35] = {252, 884,  232, 2234,  234, 742,  234, 2778,  232, 1286,  230, 1286,  232, 1422,  232, 1828,  232, 14204,  256, 878,  232, 1558,  232, 742,  230, 744,  232, 742,  232, 1964,  234, 742,  232, 742,  232};  // UNKNOWN 99B22E91
uint16_t NO_9_b[35] = {256, 880,  232, 2234,  232, 742,  232, 2776,  232, 1286,  232, 1286,  232, 1420,  230, 1830,  230, 14206,  256, 878,  234, 2640,  232, 1828,  234, 740,  232, 742,  232, 1964,  234, 740,  234, 742,  232};  // UNKNOWN C68A87BA

uint16_t CHup_a[35] = {226, 912,  204, 2262,  204, 772,  202, 2806,  204, 1314,  202, 1314,  204, 1448,  204, 1858,  204, 14232,  226, 906,  204, 1042,  204, 770,  204, 772,  202, 772,  204, 2536,  204, 772,  202, 772,  204};  // UNKNOWN 533BF3A0
uint16_t CHup_b[35] = {228, 912,  204, 2262,  204, 770,  204, 2806,  204, 1314,  204, 1314,  204, 1450,  202, 1856,  204, 14232,  228, 908,  204, 2128,  204, 1856,  202, 772,  204, 770,  202, 2534,  204, 772,  204, 772,  204};  // UNKNOWN F09C641D

uint16_t CHdown_a[35] = {228, 908,  206, 2262,  206, 770,  204, 2806,  204, 1314,  204, 1314,  204, 1450,  204, 1856,  204, 14232,  226, 906,  204, 906,  204, 770,  204, 772,  204, 770,  204, 2670,  204, 770,  204, 772,  204};  // UNKNOWN CF7964EB
uint16_t CHdown_b[35] = {228, 910,  204, 2262,  208, 768,  204, 2806,  204, 1314,  204, 1314,  204, 1450,  202, 1856,  204, 14230,  228, 906,  204, 1992,  204, 1856,  204, 772,  204, 772,  202, 2670,  204, 772,  204, 770,  204};  // UNKNOWN F09C641D

uint16_t VOLup_a[35] = {254, 882,  232, 2236,  204, 770,  204, 2804,  226, 1292,  232, 1286,  232, 1422,  232, 1828,  230, 14208,  254, 878,  230, 1422,  232, 744,  232, 744,  204, 770,  230, 2100,  232, 744,  230, 744,  204};  // UNKNOWN 99B22E91
uint16_t VOLup_b[35] = {228, 908,  208, 2258,  204, 770,  204, 2806,  206, 1312,  204, 1312,  204, 1450,  204, 1858,  202, 14234,  226, 906,  204, 2400,  204, 1858,  202, 770,  204, 770,  204, 2262,  204, 772,  204, 770,  204};  // UNKNOWN C68A87BA

uint16_t VOLdown_a[35] = {208, 948,  168, 2262,  206, 772,  202, 2804,  204, 1314,  206, 1314,  204, 1448,  206, 1852,  206, 14252,  208, 906,  204, 1314,  204, 772,  204, 770,  204, 770,  230, 2272,  168, 772,  204, 770,  204};  // UNKNOWN 9FD97BF4
uint16_t VOLdown_b[35] = {214, 904,  204, 2264,  204, 770,  206, 2804,  204, 1312,  206, 1312,  204, 1486,  168, 1856,  204, 14250,  236, 878,  204, 2398,  204, 1856,  206, 808,  166, 808,  166, 2298,  170, 770,  204, 772,  202};  // UNKNOWN 20F67DBF

uint16_t Mute_a[35] = {238, 880,  234, 2232,  234, 740,  234, 2774,  234, 1284,  234, 1282,  234, 1418,  236, 1824,  234, 14204,  256, 876,  234, 1148,  234, 742,  234, 742,  234, 740,  234, 2366,  236, 740,  234, 740,  234};  // UNKNOWN 99B22E91
uint16_t Mute_b[35] = {258, 880,  234, 2238,  228, 742,  232, 2774,  236, 1282,  236, 1282,  234, 1418,  236, 1824,  236, 14202,  254, 876,  236, 2230,  236, 1826,  234, 740,  234, 740,  232, 2368,  236, 740,  234, 740,  234};  // UNKNOWN F09C641D

uint16_t SMOD_a[35] = {256, 880,  234, 2230,  236, 740,  234, 2772,  236, 1282,  236, 1284,  234, 1418,  236, 1824,  236, 14222,  236, 876,  232, 1556,  232, 744,  232, 742,  232, 1422,  236, 1282,  232, 742,  234, 740,  234};  // UNKNOWN 11FC2CD9
uint16_t SMOD_b[35] = {236, 882,  234, 2234,  234, 740,  236, 2774,  234, 1284,  234, 1284,  236, 1416,  236, 1824,  236, 14222,  238, 876,  236, 2638,  234, 1828,  236, 738,  234, 1418,  234, 1284,  236, 740,  234, 740,  232};  // UNKNOWN 3A331C6A

uint16_t Return_a[35] = {228, 912,  202, 2264,  204, 772,  202, 2806,  204, 1314,  204, 1314,  202, 1450,  204, 1856,  204, 14232,  230, 906,  202, 1586,  204, 772,  204, 770,  204, 1042,  204, 1722,  202, 772,  204, 770,  202};  // UNKNOWN AEF70110
uint16_t Return_b[35] = {228, 912,  204, 2266,  202, 770,  204, 2806,  204, 1314,  202, 1314,  202, 1450,  204, 1856,  204, 14232,  226, 908,  204, 2670,  202, 1858,  202, 772,  202, 1044,  204, 1720,  206, 768,  204, 772,  204};  // UNKNOWN B7D5673F

uint16_t Exit_a[35] = {228, 910,  204, 2264,  204, 772,  202, 2806,  206, 1314,  204, 1312,  202, 1452,  254, 1806,  202, 14230,  230, 906,  204, 1722,  204, 772,  204, 772,  202, 1044,  232, 1556,  202, 772,  204, 770,  204};  // UNKNOWN 2BD563E3
uint16_t Exit_b[35] = {228, 910,  204, 2264,  204, 772,  204, 2806,  204, 1312,  202, 1314,  204, 1450,  204, 1856,  202, 14232,  228, 908,  202, 2806,  204, 1858,  202, 772,  202, 1044,  204, 1586,  204, 772,  202, 770,  204};  // UNKNOWN B7D5673F

uint16_t OK_a[35] = {234, 884,  234, 2232,  234, 742,  232, 2776,  236, 1282,  236, 1282,  234, 1418,  234, 1826,  234, 14222,  234, 880,  234, 1826,  234, 740,  234, 740,  234, 1014,  234, 1416,  236, 740,  232, 742,  234};  // UNKNOWN AEF70110
uint16_t OK_b[35] = {238, 880,  236, 2232,  234, 742,  234, 2776,  236, 1282,  234, 1284,  236, 1418,  234, 1824,  232, 14224,  238, 878,  232, 740,  234, 1826,  236, 740,  234, 1012,  234, 1418,  234, 740,  234, 740,  234};  // UNKNOWN 53A96A94

uint16_t OK_UP_a[35] = {226, 912,  202, 2266,  202, 772,  202, 2806,  204, 1314,  202, 1314,  202, 1450,  204, 1856,  204, 14232,  228, 908,  202, 2400,  202, 774,  202, 772,  202, 1044,  204, 908,  202, 772,  204, 772,  202};  // UNKNOWN 6DDEE76
uint16_t OK_UP_b[35] = {230, 912,  202, 2264,  202, 772,  202, 2808,  204, 1314,  202, 1314,  204, 1450,  204, 1856,  202, 14232,  228, 908,  202, 1314,  204, 1858,  202, 772,  200, 1044,  204, 908,  202, 774,  202, 772,  204};  // UNKNOWN 415DCA37

uint16_t OK_RT_a[35] = {226, 910,  202, 2264,  204, 772,  204, 2806,  204, 1314,  204, 1314,  202, 1450,  204, 1856,  204, 14232,  228, 906,  204, 1994,  204, 772,  202, 770,  204, 1042,  204, 1314,  204, 770,  204, 772,  204};  // UNKNOWN AEF70110
uint16_t OK_RT_b[35] = {224, 912,  202, 2266,  202, 772,  202, 2806,  204, 1314,  204, 1314,  204, 1450,  204, 1856,  204, 14232,  226, 908,  204, 908,  204, 1856,  202, 772,  204, 1042,  204, 1314,  204, 772,  202, 770,  206};  // UNKNOWN 53A96A94

uint16_t OK_DN_a[35] = {230, 910,  204, 2262,  204, 772,  202, 2806,  202, 1316,  202, 1314,  204, 1450,  204, 1856,  204, 14234,  224, 908,  202, 2264,  202, 772,  202, 772,  202, 1044,  204, 1042,  204, 772,  202, 772,  202};  // UNKNOWN 11FC2CD9
uint16_t OK_DN_b[35] = {228, 910,  204, 2264,  204, 774,  200, 2806,  202, 1314,  204, 1312,  204, 1452,  204, 1856,  204, 14232,  226, 906,  202, 1178,  204, 1856,  202, 772,  202, 1044,  204, 1042,  204, 770,  204, 770,  204};  // UNKNOWN 363F8BD4

uint16_t OK_LT_a[35] = {226, 910,  204, 2262,  204, 772,  204, 2806,  204, 1316,  202, 1314,  204, 1448,  206, 1854,  204, 14232,  226, 908,  204, 2128,  202, 772,  202, 772,  204, 1042,  204, 1178,  202, 772,  204, 770,  204};  // UNKNOWN 11FC2CD9
uint16_t OK_LT_b[35] = {226, 910,  204, 2264,  204, 772,  204, 2806,  204, 1314,  202, 1314,  202, 1450,  204, 1856,  204, 14232,  226, 908,  202, 1044,  202, 1856,  204, 770,  204, 1042,  204, 1180,  204, 770,  202, 770,  204};  // UNKNOWN B6AE965D


void handleRoot() {
  server.send(200, "text/html", HOMEPAGE);
}

void cmdboxon() {
  server.send(200, "text/html", HOMEPAGE);
  irsend.sendRaw(BOXON_a, 35, 38);  // Send a raw data capture at 38kHz.
  irsend.sendRaw(BOXON_b, 35, 38);  // Send a raw data capture at 38kHz.
}

void cmd0() {
  server.send(200, "text/html", HOMEPAGE);
  irsend.sendRaw(NO_0_a, 35, 38);  // Send a raw data capture at 38kHz.
  irsend.sendRaw(NO_0_b, 35, 38);  // Send a raw data capture at 38kHz.
}

void cmd1() {
  server.send(200, "text/html", HOMEPAGE);
  irsend.sendRaw(NO_1, 35, 38);  // Send a raw data capture at 38kHz.
}

void cmd2() {
  server.send(200, "text/html", HOMEPAGE);
  irsend.sendRaw(NO_2_a, 35, 38);  // Send a raw data capture at 38kHz.
  irsend.sendRaw(NO_2_b, 35, 38);  // Send a raw data capture at 38kHz.
}

void cmd3() {
  server.send(200, "text/html", HOMEPAGE);
  irsend.sendRaw(NO_3_a, 35, 38);  // Send a raw data capture at 38kHz..
  irsend.sendRaw(NO_3_b, 35, 38);  // Send a raw data capture at 38kHz.
}

void cmd4() {
  server.send(200, "text/html", HOMEPAGE);
  irsend.sendRaw(NO_4_a, 35, 38);  // Send a raw data capture at 38kHz.
  irsend.sendRaw(NO_4_b, 35, 38);  // Send a raw data capture at 38kHz.
}

void cmd5() {
  server.send(200, "text/html", HOMEPAGE);
  irsend.sendRaw(NO_5_a, 35, 38);  // Send a raw data capture at 38kHz.
  irsend.sendRaw(NO_5_b, 35, 38);  // Send a raw data capture at 38kHz.
}

void cmd6() {
  server.send(200, "text/html", HOMEPAGE);
  irsend.sendRaw(NO_6_a, 35, 38);  // Send a raw data capture at 38kHz.
  irsend.sendRaw(NO_6_b, 35, 38);  // Send a raw data capture at 38kHz.
}

void cmd7() {
  server.send(200, "text/html", HOMEPAGE);
  irsend.sendRaw(NO_7_a, 35, 38);  // Send a raw data capture at 38kHz.
  irsend.sendRaw(NO_7_b, 35, 38);  // Send a raw data capture at 38kHz.
}

void cmd8() {
  server.send(200, "text/html", HOMEPAGE);
  irsend.sendRaw(NO_8_a, 35, 38);  // Send a raw data capture at 38kHz.
  irsend.sendRaw(NO_8_b, 35, 38);  // Send a raw data capture at 38kHz.
}

void cmd9() {
  server.send(200, "text/html", HOMEPAGE);
  irsend.sendRaw(NO_9_a, 35, 38);  // Send a raw data capture at 38kHz.
  irsend.sendRaw(NO_9_b, 35, 38);  // Send a raw data capture at 38kHz.
}

void cmdchup() {
  server.send(200, "text/html", HOMEPAGE);
  irsend.sendRaw(CHup_a, 35, 38);  // Send a raw data capture at 38kHz.  
  irsend.sendRaw(CHup_b, 35, 38);  // Send a raw data capture at 38kHz.
}

void cmdchdown() {
  server.send(200, "text/html", HOMEPAGE);
  irsend.sendRaw(CHdown_a, 35, 38);  // Send a raw data capture at 38kHz.  
  irsend.sendRaw(CHdown_b, 35, 38);  // Send a raw data capture at 38kHz.
}

void cmdvolup() {
  server.send(200, "text/html", HOMEPAGE);
  irsend.sendRaw(VOLup_a, 35, 38);  // Send a raw data capture at 38kHz.  
  irsend.sendRaw(VOLup_b, 35, 38);  // Send a raw data capture at 38kHz.
}

void cmdvoldown() {
  server.send(200, "text/html", HOMEPAGE);
  irsend.sendRaw(VOLdown_a, 35, 38);  // Send a raw data capture at 38kHz.  
  irsend.sendRaw(VOLdown_b, 35, 38);  // Send a raw data capture at 38kHz.
}

void cmdmute() {
  server.send(200, "text/html", HOMEPAGE);
  irsend.sendRaw(Mute_a, 35, 38);  // Send a raw data capture at 38kHz.
  irsend.sendRaw(Mute_b, 35, 38);  // Send a raw data capture at 38kHz.  
}

void cmdsmod() {
  server.send(200, "text/html", HOMEPAGE);
  irsend.sendRaw(SMOD_a, 35, 38);  // Send a raw data capture at 38kHz.  
  irsend.sendRaw(SMOD_b, 35, 38);  // Send a raw data capture at 38kHz.
}

void cmdreturn() {
  server.send(200, "text/html", HOMEPAGE);
  irsend.sendRaw(Return_a, 35, 38);  // Send a raw data capture at 38kHz.  
  irsend.sendRaw(Return_b, 35, 38);  // Send a raw data capture at 38kHz.
}

void cmdexit() {
  server.send(200, "text/html", HOMEPAGE);
  irsend.sendRaw(Exit_a, 35, 38);  // Send a raw data capture at 38kHz.  
  irsend.sendRaw(Exit_b, 35, 38);  // Send a raw data capture at 38kHz.
}

void cmdok() {
  server.send(200, "text/html", HOMEPAGE);
  irsend.sendRaw(OK_a, 35, 38);  // Send a raw data capture at 38kHz.  
  irsend.sendRaw(OK_b, 35, 38);  // Send a raw data capture at 38kHz.
}

void cmdokup() {
  server.send(200, "text/html", HOMEPAGE);
  irsend.sendRaw(OK_UP_a, 35, 38);  // Send a raw data capture at 38kHz.  
  irsend.sendRaw(OK_UP_b, 35, 38);  // Send a raw data capture at 38kHz.
}

void cmdokdn() {
  server.send(200, "text/html", HOMEPAGE);
  irsend.sendRaw(OK_DN_a, 35, 38);  // Send a raw data capture at 38kHz.  
  irsend.sendRaw(OK_DN_b, 35, 38);  // Send a raw data capture at 38kHz.
}


void cmdoklt() {
  server.send(200, "text/html", HOMEPAGE);
  irsend.sendRaw(OK_LT_a, 35, 38);  // Send a raw data capture at 38kHz.  
  irsend.sendRaw(OK_LT_b, 35, 38);  // Send a raw data capture at 38kHz.
}

void cmdokrt() {
  server.send(200, "text/html", HOMEPAGE);
  irsend.sendRaw(OK_RT_a, 35, 38);  // Send a raw data capture at 38kHz.  
  irsend.sendRaw(OK_RT_b, 35, 38);  // Send a raw data capture at 38kHz.
}

void setup() {
  // IR remote transmitter
  irsend.begin();
  
  //Serial.begin(115200);
  WiFi.begin(ssid, password); //begin WiFi connection
  //Serial.println("");
 
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }
  //Serial.println("");
  //Serial.print("Connected to ");
  //Serial.println(ssid);
  //Serial.print("IP address: ");
  //Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/cmdboxon", cmdboxon); 
  server.on("/cmd0", cmd0);  
  server.on("/cmd1", cmd1);
  server.on("/cmd2", cmd2);
  server.on("/cmd3", cmd3);
  server.on("/cmd4", cmd4);
  server.on("/cmd5", cmd5); 
  server.on("/cmd6", cmd6);
  server.on("/cmd7", cmd7);
  server.on("/cmd8", cmd8);
  server.on("/cmd9", cmd9);
  server.on("/cmdchup", cmdchup);
  server.on("/cmdchdown", cmdchdown);
  server.on("/cmdvolup", cmdvolup); 
  server.on("/cmdvoldown", cmdvoldown);
  server.on("/cmdmute", cmdmute);   
  server.on("/cmdsmod", cmdsmod); 
  server.on("/cmdreturn", cmdreturn);
  server.on("/cmdexit", cmdexit);
  server.on("/cmdok", cmdok);
  server.on("/cmdokup", cmdokup);
  server.on("/cmdokdn", cmdokdn);
  server.on("/cmdoklt", cmdoklt);
  server.on("/cmdokrt", cmdokrt); 
  //Serial.println("Web server started...");    
  server.begin();
}
 

void loop() {
  server.handleClient();
}
