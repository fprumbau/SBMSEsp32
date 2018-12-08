#ifndef GLOBAL_H
#define GLOBAL_H

#include <WebServer.h>
#include <WiFiUdp.h>
#include "OTA.h"
#include "Vars.h"
#include "MyWifi.h"
#include "WebCom.h"
#include "Battery.h"
#include "ESP32OTA.h"
#include "SBMS.h"
#include "SMA.h"

extern Vars vars; //Global definierte Variablen
extern SMA sma;  //read SMA energy meter broadcast messages
extern MyWifi myWifi;
extern WebCom wc;
extern Battery battery;
extern WebServer server;
extern ESP32OTA updater;
extern OTA ota;  //Over-the-air updater
extern SBMS sbms;//SBMS solar battery charger functions, uncompress etc.
extern WiFiUDP udp;
extern long lastUdpRead; //letztes, erfolgreiches lesen eines UDP-Paketes (WiFi isAlive)

#endif
