#include "MyWifi.h"

void MyWifi::connect() {

  //This is here to force the ESP32 to reset the WiFi and initialize correctly.
  WiFi.enableSTA(true);
  Serial.print("WIFI status = ");
  Serial.println(WiFi.getMode());
  WiFi.disconnect(true);
  delay(1000);
  WiFi.mode(WIFI_STA);
  delay(1000);
  Serial.print("WIFI status = ");
  Serial.println(WiFi.getMode());
  //Ende silly mode 
 
  WiFi.begin(_ssid, _password);
  WiFi.setSleep(false);
  
  while(WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500); 
  }
  Serial.println("\n");
  Serial.printf("New Client. RSSi: %ld dBm\n",WiFi.RSSI()); 
  Serial.print("Ip Address: ");

  IPAddress ip = WiFi.localIP();
  Serial.println(ip);
  _localIP = ip;
  _ip=ip.toString();
}

String MyWifi::getIpAddress() {
  return _ip;
}

IPAddress MyWifi::localIP() {
  return _localIP;
}

void MyWifi::reconnect() {
  Serial.println("Restarting WiFi...");
  WiFi.disconnect();
  connect();
}
