#include "global.h"

#include "OTA.h"
#include "Vars.h"
#include "MyWifi.h"
#include "WebCom.h"
#include "Battery.h"
#include "ESP32OTA.h"
#include <WebServer.h>
#include "ESPAsyncWebServer.h"

Vars vars; 
MyWifi myWifi;
WebCom wc;
Battery battery;
//WebServer server(80);
AsyncWebServer server(80);
ESP32OTA updater;
OTA ota;;  //Over-the-air updater
SBMS sbms;
SMA sma;
