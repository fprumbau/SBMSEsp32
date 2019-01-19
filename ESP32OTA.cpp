#include <Update.h>

#include "html.h"
#include "global.h"

//https://github.com/bbx10/WebServer_tng/issues/4 Anpassung ESP32

void ESP32OTA::init(const char* host) {
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
     changes += "<li> Umstellung auf AsyncWebserver inkl. Async OTA";
     changes += "<li>OTA.h/OTA.cpp mit ESP32OTA.h/ESP32OTA.cpp verschmolzen";
     changes += "<li>Vars.h in global.h ueberfuehrt, updaterui.h und webpage.h in html.h konsolidiert";
     changes += "<li>Eigene IP in JavaScript mit location.host auslesen und verwenden";
     changes += "<li>Battery.h/.cpp in wechselrichter.h/.cpp ueberfuehrt";
     changes += "<li>Methoden starteBatterie und starteWechselrichter in inverter-Klasse uebernommen";
     changes += "<li>Websocket-Eventhandler ueber Wrappermethode in WebCom verschoben";
     changes += "<li>SOC-Variable und cv (cell voltages) in global.h/.cpp uebernommen";
     changes += "<li>OTA mit Reporting und Abschlusseite (OK/Failed) geht jetzt";
     changes += "<li>Funktion readSBMS nun in SBMS.h/.cpp";
     changes += "<li>Authentifizierung fuer ESPOTA und AsyncWebserver (auskommentiert), Zugriff ueber / statt /sbms";
     changes += "<li>Ein Schaltvorgang auf s1 und s2 sollte maximal alle 60s erlaubt werden.";
     changes += "<li>checkValues mit led-Methoden in Inverterklasse verlegt.";
     changes += "<li>Code zum behandeln des Inverter-Toggle-Tasters in Inverterklasse verlegt";
     changes += "<li>Nun 60s, bis WiFi wieder aufgebaut wird (UDP miss)";
     changes += "<li>Feld fuer Lieferung / Bezug anzeigen";
     changes += "<li>SBMS-Daten werden nun auch per JSon uebermittelt";
     changes += "<li>Webseite aufraeumen. Doppelklick auf Dbg1/Dbg2 leitet auf Updateseite, erfolgr. Update nach 3s zur SBMS-Seite";
     changes += "<li>Low-Meter von 20% auf 30% geaendert, statt 110AH nun 100AH, Lieferung Schriftformatierung";
     changes += "<li>NTPClient zugefuegt, um im inverter.check() ein automatische Schaltung des Verbrauchs zu ermoeglichen";
     changes += "<li>Zwischen 20 Uhr und 5 Uhr morgens Batteriebetrieb versuchen (nur wenn state of charge > limit)";
     changes += "<li>Staendige Datumsanzeige in der Webseite";
     changes += "<li>Fehler in 0.9.9.30 saugte Batterie leer, solange SOC_LIMIT OK, ignorierte aber Zelleinzelspannungen";
     changes += "<li>Aut. WiFi reconnect ging nicht, darum erstmal herausgenommen (SMA.cpp); Update-Site aus Location beruecksichtigt ";
     changes += "<li>Schaltschwelle CV low von 2850mV auf 3000mV erhoeht (spaeter weiter auf 3100 mit 0.9.9.38)";
     changes += "<li>Hysterese fuer SOC_LIMIT (SOC_HYST mit 5%) hinzugefuegt";
     changes += "<li>0.9.9.36: 100mV Hysterese bei Zellspannungen verhindert automatisches Anschalten bei Leerlauf";
     changes += "<li>0.9.9.37: Faellt die Spannung einer Zelle auf <2,7V, dann wird der Lader S2 für 5Min aktiviert";     
     changes += "<li>0.9.9.38: Weitere Modularisierung mit Charger und Batteryklasse, bei niedriger CV nun start S2 fuer jeweils 5Min.";
     changes += "<li>0.9.9.39: Nun statt Wifi-Reset sma.reset (udp reinit); Fixed Fehler, es wurden nur 7 Zellen ueberwacht";
     changes += "<li>0.9.9.40: Relais 3+4 verdrahtet, Umstrukturierung, mit Relais 4 werden nun die Luefter geschaltet";
     changes += "<li>0.9.9.41: Batterie erst wieder moeglich, wenn Zellspannungen LOW_VOLTAGE_MILLIS + CV_HYST (3150mV) betragen";
     changes += "<li>0.9.9.42: SBMS-Temperatur wird ausgelesen und verhindert ab 40 Grad Celsius ein Abschalten der Luefter";
     changes += "<li>0.9.9.43: Immer, wenn seit dem letzten Restart mehr als 5h vergangen sind (Ausnahme: 0Uhr-Uebergang), wird ESP.resta  rt() ausgeloest";
     changes += "<li>0.9.9.44: Da das Relais zum Charger S1 (HLG600, 600W) ausgewechselt werden musste, wird S1 nun seltener geschaltet (10min statt 60s)";
     changes += "<li>0.9.9.45: Zellspannungsdifferenzen Min-Max werden wieder richtig angezeigt";
     changes += "<li>0.9.9.46: Auswertung Webseite ging nur bis Zelle 7, Notladungsautomatik in battery.cpp schaltete S2 nach 5 Minuten immer wieder ab";
     changes += "<li>0.9.9.47: Min-/Maxzellspannung um einen Index versetzt; Seit 0.9.9.45 Fehler bei PV1 und PV2 Poweranzeige (weil eigentlich Zellspannungen 2/3)";
     changes += "<li>0.9.9.48: Ueber das freie Relais RELAY_3 zum Remote ON/OFF von S1";
     updater.setUpdaterUi("Title", "Build : 0.9.9.48", "SBMS120 Solar Charger", "Branch : master", changes);
     //Optional: Authentifizieren
     //updater.setup("/update", "admin", "Go8319!");
     updater.setup("/update", "", "");
  } else {
     Serial.println("Flash OTA programming only possible with 4Mb Flash size!!!");
  }
}

double calcSpeed(unsigned long ms, size_t len){
    return (double)(len * 125) / (double)(ms * 16);
}

void ESP32OTA::setup(const char *path, String username, String password) {

    _username = username;
    _password = password;
    uint32_t t_start,t_stop;
    size_t fileSize;
    uint16_t ct = 0;
    

    // handler for the /update form page
    server.on(path, HTTP_GET, [&](AsyncWebServerRequest *request){
               
        String pageIndex = String(update);
        pageIndex.replace("{title}",_title);
        pageIndex.replace("{banner}",_banner);
        pageIndex.replace("{build}",_build);
        pageIndex.replace("{branch}",_branch);
        pageIndex.replace("{deviceInfo}",_deviceInfo);
        pageIndex.replace("{footer}",_footer);
      
        AsyncWebServerResponse *response = request->beginResponse(200, "text/html", pageIndex);
        response->addHeader("Connection", "close");
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
    });

    // handler for the /update form POST (once file upload finishes)
   server.on(path, HTTP_POST, [&](AsyncWebServerRequest *request){
        // the request handler is triggered after the upload has finished... 
        // create the response, add header, and send response
        String pageIndex = String(update);
        pageIndex.replace("{title}",_title);
        if(Update.hasError()){
          pageIndex.replace("{banner}","<b><font color=red>Update gescheitert</font></b>");
        } else {
          pageIndex.replace("{banner}","<b><font color=green>Update erfolgreich</font></b>");
          pageIndex.replace("{redirect}", "redirect=true;");
        }
        pageIndex.replace("{build}",_build);
        pageIndex.replace("{branch}",_branch);
        pageIndex.replace("{deviceInfo}",_deviceInfo);
        pageIndex.replace("{footer}",_footer);
      
      AsyncWebServerResponse *response = request->beginResponse(200, "text/html", pageIndex);
      response->addHeader("Connection", "close");
      response->addHeader("Access-Control-Allow-Origin", "*");
      request->send(response);
    },[&](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
      //Upload handler chunks in data     
      if(!index){ // if index == 0 then this is the first frame of data
        stopForOTA = true; //stoppt alle Aktionen im loop()

        delay(3000); //geht das hier?? unsicher!!!
        udp.stop(); //koennte helfen
        
        Serial.printf("UploadStart: %s\n", filename.c_str());
        Serial.setDebugOutput(true);
        t_start = millis();
        fileSize = len;
        
        // calculate sketch space required for the update
        uint32_t maxSketchSpace = (1048576 - 0x1000) & 0xFFFFF000;
        if(!Update.begin(maxSketchSpace)){//start with max available size
          Update.printError(Serial);
        }
        //Update.runAsync(true); // tell the updaterClass to run in async mode (nicht da fuer ESP32)
      } else {
        ct++;
        if(ct%70==0) Serial.println("");
        Serial.print(".");
        fileSize += len;
      }
      //Write chunked data to the free sketch space
      if(Update.write(data, len) != len){
          Update.printError(Serial);
      }
      
      if(final){ // if the final flag is set then this is the last frame of data
        if(Update.end(true)){ //true to set the size to the current progress
            t_stop = millis();
            Serial.print("\nTime UPLOAD: "); Serial.print((t_stop - t_start) / 1000.0); Serial.println(" sec.");
            Serial.print("Speed UPLOAD: "); Serial.print(calcSpeed(t_stop - t_start, fileSize)); Serial.println(" Kbit/s");
            Serial.printf("Upload Success, Rebooting: %u bytes\n", fileSize);
            restartRequired = true;  // Tell the main loop to restart the ESP
        } else {
            Update.printError(Serial);
        }
        Serial.setDebugOutput(false);
      }
  });

  //Hmmh not found, gehoert eigentlich nicht hier hin
  server.onNotFound([](AsyncWebServerRequest *request){
      Serial.printf("NOT_FOUND: ");
      if(request->method() == HTTP_GET)
        Serial.printf("GET");
      else if(request->method() == HTTP_POST)
        Serial.printf("POST");
      else if(request->method() == HTTP_DELETE)
        Serial.printf("DELETE");
      else if(request->method() == HTTP_PUT)
        Serial.printf("PUT");
      else if(request->method() == HTTP_PATCH)
        Serial.printf("PATCH");
      else if(request->method() == HTTP_HEAD)
        Serial.printf("HEAD");
      else if(request->method() == HTTP_OPTIONS)
        Serial.printf("OPTIONS");
      else
        Serial.printf("UNKNOWN");
      Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());
  
      if(request->contentLength()){
        Serial.printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
        Serial.printf("_CONTENT_LENGTH: %u\n", request->contentLength());
      }
  
      int headers = request->headers();
      int i;
      for(i=0;i<headers;i++){
        AsyncWebHeader* h = request->getHeader(i);
        Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
      }
  
      int params = request->params();
      for(i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if(p->isFile()){
          Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
        } else if(p->isPost()){
          Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        } else {
          Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
      }

    request->send(404);
  });  

}

void ESP32OTA::setUpdaterUi(String title,String banner,String build,String branch,String footer) {
    _title = title;
    _banner = banner;
    _build = build;
    _branch = branch;
    _deviceInfo = "ChipId : " + String(ESP.getChipRevision());
    _footer = footer;
}
