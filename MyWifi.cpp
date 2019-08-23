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

  while(WiFi.status() != WL_CONNECTED && wifiReconnects < 10) {
        wifiReconnects++;
        WiFi.disconnect(true);
        WiFi.mode(WIFI_STA);
        WiFi.begin(_ssid, _password);
        Serial.print(F("."));
        delay(100); 
  }
  Serial.printf("\nNew Client. RSSi: %ld dBm\n",WiFi.RSSI()); 
  Serial.print(F("Ip Address: "));

  IPAddress ip = WiFi.localIP();
  Serial.println(ip);
  _localIP = ip;
  _ip=ip.toString();

  Serial.println(F("Initializing sma.init"));
  sma.init();
}

String MyWifi::getIpAddress() {
  return _ip;
}

IPAddress MyWifi::localIP() {
  return _localIP;
}

void MyWifi::reconnect() {
  if(wifiReconnects>=10) {
    String msg = F("Nach 10 Wifi Reconnects: Esp.restart()");
    wc.sendClients(msg.c_str());
    Serial.println(msg);
    ESP.restart();
  } else {    
    Serial.println(F("Restarting WiFi..."));
    connect();
  }
}
