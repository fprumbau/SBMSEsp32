#include "global.h"
#include <ESPmDNS.h>

void OTA::init(const char* host) {
  //OTA is possible only with 4mb memory
  long flashSize = ESP.getFlashChipSize();
  Serial.print("Flash Size: ");
  Serial.println(flashSize);
  if(flashSize > 4000000) {
     //set web UI
     MDNS.begin(host);
     MDNS.addService("http", "tcp", 80);
     Serial.printf("\n\nHTTPUpdateServer ready! Open http://%s.local/update in your browser\n", host);
     String changes = "<b>Device info</b>: Frank's Solar Charger Monitor";
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
     changes += "<li> Automatisches Ein-/Ausschalten der Charger.";
     changes += "<li> OTA-Updaterklasse uebernommen, Hauptklasse aufgeraeumt.";
     changes += "<li> Debugausgabe Wirkleistung/Lieferung in Webbrowser (d2).";
     changes += "<li> OTAUpdater mit Debugsettings ueber d2.";
     changes += "<li> ESP32OTA kann nun auf vars (debug2 und udp) zugreifen, update geht (vorher Reset empfehlenswert!).";
     changes += "<li> Werden S1 oder S2 ueber SMA.cpp geschaltet, muss s1 an, s2 aus etc. uebermittelt werden.";
     changes += "<li> Globale Objekte mit global.h.";
     changes += "<li> Variablenupdats nun nur noch mittels JSon";
     changes += "<li> Moeglichkeit, WiFi mittels 'restart wifi' ueber serielle Schnittstelle zu restarten";     
     changes += "<li> Wifi restartet, wenn 10s kein UDP-Paket des SMA-Energymeters empfangen wurde";
     updater.setUpdaterUi("Title", "Build : 0.9.5", "SBMS120 Solar Charger", "Branch : master", changes);
     updater.setup("/update", "", "");
  } else {
     Serial.println("Flash OTA programming only possible with 4Mb Flash size!!!");
  }
}
