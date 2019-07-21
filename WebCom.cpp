#include "global.h"
#include <ArduinoJson.h>

/**
   Sende Daten zu allen über Websockets verbundenen
   Clients. 
*/
void WebCom::sendClients(String msg) { 
  ws.textAll(msg.c_str());
}

void WebCom::updateUi(AsyncWebSocketClient *client, bool all) {
        //mit JSON
        StaticJsonDocument<300> doc; //letzte Zaehlung: 114
        doc["d1"]=debug;
        doc["d2"]=debug2;
        doc["s1"]=charger.isChargerOn(1);
        doc["s2"]=charger.isChargerOn(2);
        doc["b"]=battery.isOn();
        doc["l"]=lieferung;
        doc["z"]=bezug;
        if(sbmsData != NULL && sbmsData.length() > 10) {
           doc["d"]=sbmsData;
        }
        doc["dt"]=datetime;
        doc["t"]=temp;
        doc["ta"]=teslaCtrlActive;
        char jsonChar[512];
        serializeJson(doc, jsonChar);
        String str(jsonChar);
        if(debug2) {
          Serial.println(str);
        }
        if (all) {
          ws.textAll(str.c_str());
        } else {
          ws.text(client->id(), str.c_str());
        }
}

void WebCom::updateUi() {
        //mit JSON
        StaticJsonDocument<300> doc; //letzte Zaehlung: 114
        doc["d1"]=debug;
        doc["d2"]=debug2;
        doc["s1"]=charger.isChargerOn(1);
        doc["s2"]=charger.isChargerOn(2);
        doc["b"]=battery.isOn();
        doc["l"]=lieferung;
        doc["z"]=bezug;
        if(sbmsData != NULL && sbmsData.length() > 10) {
           doc["d"]=sbmsData;
        }
        doc["dt"]=datetime;
        doc["t"]=temp;
        doc["ta"]=teslaCtrlActive;
        char jsonChar[512];
        serializeJson(doc, jsonChar);
        String str(jsonChar);
        if(debug2) {
          Serial.println(str);
        }
        ws.textAll(str.c_str());
}

void WebCom::onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len){

  switch (type) {
    case WS_EVT_CONNECT: {       
       
          client->text("@ Connected");
         
          //Aktualisieren von debug/debug1/s1/s2/netz bzw batt
          wc.updateUi(client, false);             
          break;
      }    
      case WS_EVT_DATA: 
      
      if(debug) {
        Serial.println(String((char *)data));
      }
      if(data[0] == '{') {
        //Umstellung auf JSon
        StaticJsonDocument<300> doc;
        deserializeJson(doc, data);

        String msg = "";
        bool update = false;
        bool saveConfig = false;
        
        bool updateTeslaCtrlActive = doc["ta"];
        if(updateTeslaCtrlActive != teslaCtrlActive) {  
          update = true;
          saveConfig = true;      
          teslaCtrlActive = updateTeslaCtrlActive;
          buildMessage(&msg, "teslaCtrlActive", String(teslaCtrlActive).c_str());
        }
        //Charger S1
        if(doc.containsKey("s1")) {
          bool s1 = doc["s1"];
          if(s1 != charger.isChargerOn(S1)) {    
            update = true;    
            charger.toggleCharger(S1, s1, true, false);
            buildMessage(&msg, "S1", String(s1).c_str());
          }
        }
        //Charger S2
        if(doc.containsKey("s2")) {
          bool s2 = doc["s2"];
          if(s2 != charger.isChargerOn(S2)) {    
            update = true;    
            charger.toggleCharger(S2, s2, true, false);
            buildMessage(&msg, "S2", String(s2).c_str());
          }
        }
        //Debug Flag 1
        if(doc.containsKey("d1")) {
          bool updateDbg1 = doc["d1"];
          if(updateDbg1 != debug) {    
            update = true;    
            debug = updateDbg1;
            buildMessage(&msg, "debug", String(debug).c_str());
          }
        }
        //Debug Flag 2
        if(doc.containsKey("d2")) {
          bool updateDbg2 = doc["d2"];
          if(updateDbg2 != debug2) {       
            update = true;          
            debug2 = updateDbg2;
            buildMessage(&msg, "debug2", String(debug2).c_str());
          }
        }
        if(doc.containsKey("vr")) {
          String netzOderBatt = doc["vr"];
          if(netzOderBatt == "netz") {       
            update = true;
            inverter.starteNetzvorrang("Websockets");
            buildMessage(&msg, "Netzvorrang", "true");
          } else if(netzOderBatt == "batt") {
            update = true;
            inverter.starteBatterie("Websockets");    
            buildMessage(&msg, "Battery", "true");      
          }
        }
        if(saveConfig) {
          config.save();
        }
        if(update) {
          wc.sendClients(msg);
          //nun die Info an alle zum Umpdate der UI versenden
          updateUi();              
        }   
      } else {
        String msg = "Cannot process data: ";
        msg+=String((char *)data);
        wc.sendClients(msg);
      }
      break;
    }
}

void WebCom::buildMessage(String* msg, const char* name, const char* value) {
    msg->concat("Switch ");
    msg->concat(name);
    msg->concat(" to ");
    msg->concat(value);
}
