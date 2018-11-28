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
     String changes = "Device info : ukn";
     changes += "<li> Integrierte Schaltung der beiden Solarlader, WebSocketServer als WebCom gekapselt";
     changes += "<li> Errormodus wird bei fehlenden SBMS-Paketen nun wieder aktiviert.";    
     changes += "<li> Interruptbehandlung (Taster) wieder akiviert.";  
     changes += "<li> Wenn debug sread nur ausgeben, wenn Inhalt vorhanden.";  
     changes += "<li> Fix input Pullup fuer Taster auf GPIO19.";
     changes += "<li> Acebutton und ButtonConfig zur Entprellung einsetzen.";
     changes += "<li> ToggleCharger-Methoden.";
     changes += "<li> S1 / S2 nun manuell zu schalten.";     
     changes += "<li> Initmeldungen, wenn neuer Client connected sollte nicht an alle gehen."; 
     changes += "<li> Initmeldungen fuer debug/debug2 an neue Clients."; 
     changes += "<li> Einbinden von JSON zwischen Server -> Client.";
     _otaUpdater.setUpdaterUi("Title", "Build : 0.8.10", "SBMS120 Solar Charger", "Branch : master", changes);
     _otaUpdater.setup(&server);
  } else {
     Serial.println("Flash OTA programming only possible with 4Mb Flash size!!!");
  }
}
