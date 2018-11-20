#include "OTA.h"

void OTA::init(WebServer& server, const char* host) {
  //OTA is possible only with 4mb memory
  long flashSize = ESP.getFlashChipSize();
  Serial.print("Flash Size: ");
  Serial.println(flashSize);
  if(flashSize > 4000000) {
     //set web UI
     MDNS.begin(host);
     MDNS.addService("http", "tcp", 80);
     Serial.printf("\n\nHTTPUpdateServer ready! Open http://%s.local/update in your browser\n", host);
     _otaUpdater.setUpdaterUi("Title", "SBMS120 Solar Charger", "Build : 0.7.28", "Branch : master","Device info : ukn","mySerial.readString usage; Laengenpruefung sread; Reconnect on close;Integration ReconnectingWebSocket; Reparaturversuch, Debugschalter zu setzen, Validierung payload length, Isrhandler ist jetzt normale Funktion, SPIFFS deaktiviert");
     _otaUpdater.setup(&server);
  } else {
     Serial.println("Flash OTA programming only possible with 4Mb Flash size!!!");
  }
}
