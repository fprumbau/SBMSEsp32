#ifndef OTA_H
#define OTA_H

#include <Esp.h>
#include <WiFi.h>
#include <WString.h>
#include <ESPmDNS.h>
#include <WebServer.h>
#include "ESP32OTA.h"
#include "Vars.h"

class OTA {
   private:
      ESP32OTA updater;
   public:
      OTA(ESP32OTA &updater);
      void init(WebServer &server, const char* host);
};

#endif
