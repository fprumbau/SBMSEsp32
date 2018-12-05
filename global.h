#ifndef GLOBAL_H
#define GLOBAL_H

#include <WebServer.h>
#include "OTA.h"
#include "Vars.h"
#include "MyWifi.h"
#include "WebCom.h"
#include "Battery.h"
#include "ESP32OTA.h"
#include "SBMS.h"
#include "SMA.h"
#include "ESPAsyncWebServer.h"

extern Vars vars; //Global definierte Variablen
extern MyWifi myWifi;
extern WebCom wc;
extern Battery battery;
//extern WebServer server;
extern AsyncWebServer server;   
extern ESP32OTA updater;
extern OTA ota;  //Over-the-air updater
extern SBMS sbms;//SBMS solar battery charger functions, uncompress etc.
extern SMA sma;  //read SMA energy meter broadcast messages

#endif
