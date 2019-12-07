#include "MyWifi.h"
#include "global.h"

void MyWifi::connect() {

  //This is here to force the ESP32 to reset the WiFi and initialize correctly.
  WiFi.disconnect(true);
  WiFi.setSleep(false);
  WiFi.enableSTA(true);
  Serial.print(F("WIFI status = "));
  Serial.println(WiFi.getMode());
  WiFi.mode(WIFI_STA);
  //Ende silly mode 
 
  WiFi.begin(_ssid, _password); 

  while(WiFi.status() != WL_CONNECTED) {
        wifiReconnects++;
        lastReconnectMillis = millis();
        WiFi.disconnect(true);
        WiFi.mode(WIFI_STA);
        WiFi.begin(_ssid, _password);
        Serial.print(F("."));
        delay(3000); 
  }

  if(connected()) {
        Serial.printf("\nNew Client. RSSi: %ld dBm\n",WiFi.RSSI()); 
        Serial.print(F("Ip Address: "));
      
        IPAddress ip = WiFi.localIP();
        Serial.println(ip);
        _localIP = ip;
        _ip=ip.toString();
      
        Serial.println(F("Initializing sma.init"));
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
      Serial.println(msg);
      wifiReconnects=0;
      ESP.restart();
    } else {    
      Serial.println(F("Restarting WiFi..."));
      connect();
    }
    Serial.println(F("And reinitializing sma: reset()"));
    sma.reset();
  }
}

bool MyWifi::connected() {
  return WiFi.status() == WL_CONNECTED;
}
