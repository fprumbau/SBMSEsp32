#include "global.h"

#include "MyWifi.h"
#include "WebCom.h"
#include "wechselrichter.h"
#include "ESP32OTA.h"

#include <WiFiUdp.h>

bool debug = false;
bool debug2 = false;
int RELAY_S1 = 33;
int RELAY_S2 = 32;    
int RELAY_PIN = 21;    
int LED_S1 = 25;
int LED_S2 = 26;
int TASTER = 19; //manuelle Inverterumschaltung

MyWifi myWifi;
SMA sma;
WebCom wc;
Inverter inverter;
AsyncWebServer server(80);
ESP32OTA updater;
SBMS sbms;
WiFiUDP udp;
long lastUdpRead = -1;
AsyncWebSocket ws("/ws");

long soc = -1; //aktueller Wert State Of Charge
int cv[8]; //aktuelle Zellspannungen

//nicht auf Serial1 warten, Feste Werte annehmen
bool testFixed = false;
const char* hostName = "esp32a";
unsigned long lastReceivedMillis = -1;
long timeout = 10000;
