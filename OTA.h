#ifndef OTA_H
#define OTA_H

#include <Esp.h>
#include <WString.h>
#include <ESP32OTA.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <WebServer.h>

class OTA {
   private:
      ESP32OTA _otaUpdater;
   public:
      void init(WebServer& server, const char* host);
};

#endif
