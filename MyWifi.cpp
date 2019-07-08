#include "MyWifi.h"
#include "global.h"

void MyWifi::connect() {

  //This is here to force the ESP32 to reset the WiFi and initialize correctly.
  WiFi.disconnect(true);
  WiFi.setSleep(false);
  WiFi.enableSTA(true);
  Serial.print("WIFI status = ");
  Serial.println(WiFi.getMode());
  WiFi.mode(WIFI_STA);
  //Ende silly mode 
 
  WiFi.begin(_ssid, _password); 

  int ct = 0;
  while(WiFi.status() != WL_CONNECTED) {
      ct++;
      if(ct>10) {
        Serial.println("Second attempt");
        ct=0;
        WiFi.disconnect(true);
        WiFi.mode(WIFI_STA);
        WiFi.begin(_ssid, _password);
      }
      Serial.print(".");
      delay(500); 
  }
  Serial.println("\n");
  Serial.printf("New Client. RSSi: %ld dBm\n",WiFi.RSSI()); 
  Serial.print(F("Ip Address: "));

  IPAddress ip = WiFi.localIP();
  Serial.println(ip);
  _localIP = ip;
  _ip=ip.toString();

  Serial.println(F("Initializing sma.init)"));
  sma.init();
}

String MyWifi::getIpAddress() {
  return _ip;
}

IPAddress MyWifi::localIP() {
  return _localIP;
}

void MyWifi::reconnect() {
  Serial.println(F("Restarting WiFi..."));
  WiFi.disconnect();
  connect();
}
