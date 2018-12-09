#ifndef GLOBAL_H
#define GLOBAL_H

#include "ESPAsyncWebServer.h"
#include <WiFiUdp.h>
#include "MyWifi.h"
#include "WebCom.h"
#include "Battery.h"
#include "ESP32OTA.h"
#include "SBMS.h"
#include "SMA.h"

extern bool debug;
extern bool debug2;
extern int RELAY_S1;
extern int RELAY_S2;    
extern int RELAY_PIN;
extern int LED_S1;
extern int LED_S2;
extern int TASTER; //manuelle Inverterumschaltung

extern Battery battery;
extern MyWifi myWifi;
extern SMA sma;  //read SMA energy meter broadcast messages
extern WebCom wc;
extern AsyncWebServer server;   
extern AsyncWebSocket ws;
extern ESP32OTA updater; //Over-the-air updater
extern WiFiUDP udp;
extern SBMS sbms;//SBMS solar battery charger functions, uncompress etc.
extern long lastUdpRead; //letztes, erfolgreiches lesen eines UDP-Paketes (WiFi isAlive)

#endif
