#include "MyWifi.h"
#include "global.h"

void MyWifi::connect() {

  //This is here to force the ESP32 to reset the WiFi and initialize correctly.
  WiFi.disconnect(true);
  WiFi.setSleep(false);
  WiFi.enableSTA(true);
  Log.warning(F("WIFI status = %s"CR), WiFi.getMode());
  WiFi.mode(WIFI_STA);
  //Ende silly mode 
 
  WiFi.begin(_ssid, _password); 

  while(WiFi.status() != WL_CONNECTED) {
        wifiReconnects++;
        lastReconnectMillis = millis();
        WiFi.disconnect(true);
        WiFi.mode(WIFI_STA);
        WiFi.begin(_ssid, _password);
        Log.warning(F("."));
        delay(3000); 
  }

  if(connected()) {
      
        IPAddress ip = WiFi.localIP();

        _localIP = ip;
        _ip=ip.toString();

        Log.warningln("\nNew Client. RSSi: %ld dBm\nIp Address: %s; Initializing sma.init",WiFi.RSSI(), ip); 
      
        sma.init();
      
        //Running since
        //ab v.0.9.9.28 NTPClient mit Zeit; 0.9.9.98 nicht EWIG warten (10 Versuche)
        int ct = 0;
        bool timeUpdate = false;
        while(!(timeUpdate = timeClient.update())) {
          yield();
          if(ct++>10) { 
            lastStatusMsg = F("Loese timeUpdate-Loop (break)");
            break;
          }
          timeClient.forceUpdate();
        }
        if(isBooting) {
          runningSince = timeClient.getFormattedDate();
        }
  }
}

String MyWifi::getIpAddress() {
  return _ip;
}

IPAddress MyWifi::localIP() {
  return _localIP;
}

void MyWifi::reconnect() {
  long now = millis();
  if((now - lastReconnectMillis) > 60000) { //Ein Reconnect max. jede Minute
    if(wifiReconnects >= myWifiRestartLimit) {
      String msg = F("Nach {myWifiRestartLimit} Wifi Reconnects: Esp.restart()");
      wc.sendClients(msg.c_str());
      Log.warningln(msg.c_str());
      wifiReconnects=0;
      ESP.restart();
    } else {    
      Log.warningln(F("Restarting WiFi..."));
      connect();
    }
    Log.warningln(F("And reinitializing sma: reset()"));
    sma.reset();
  }
}

bool MyWifi::connected() {
  return WiFi.status() == WL_CONNECTED;
}

void MyWifi::print() {
  Log.warningln(F("--------------------------------"));
  Log.warningln(F("Wifi reconnects: %s"), wifiReconnects);
  Log.warningln(F("Wifi IP: %s"), _localIP);
  bool conn = connected();
  Log.warningln(F("MyWifi.connected() :: %T"), conn); 
}
