#include "MyWifi.h"

void MyWifi::connect() {
  WiFi.begin(_ssid, _password);
  WiFi.mode(WIFI_STA); //para que no inicie el SoftAP en el modo normal
  
  while(WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500); 
  }
  Serial.println("\n");
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
