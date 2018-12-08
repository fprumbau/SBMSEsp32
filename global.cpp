#include "global.h"

#include "OTA.h"
#include "Vars.h"
#include "MyWifi.h"
#include "WebCom.h"
#include "Battery.h"
#include "ESP32OTA.h"

#include <WiFiUdp.h>
#include <WebServer.h>

Vars vars; 
SMA sma;
MyWifi myWifi;
WebCom wc;
Battery battery;
WebServer server(80);
ESP32OTA updater;
OTA ota;  //Over-the-air updater
SBMS sbms;
WiFiUDP udp;
long lastUdpRead = -1;
