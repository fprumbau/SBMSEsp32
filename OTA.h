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
      void init(WebServer &server, Vars &vars, const char* host);
};

#endif
