#ifndef MYWIFI_H
#define MYWIFI_H

#include <WiFi.h>

class MyWifi {
    private:
      const char* _ssid = "Perry";
      const char* _password = "5785340536886787";
      String _ip;
      IPAddress _localIP;
      long lastReconnectMillis;
      const int myWifiRestartLimit = 25;
    public:
      void connect();
      String getIpAddress();
      IPAddress localIP();
      void reconnect();
      bool connected();
};

#endif
