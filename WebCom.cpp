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
      if (data[0] == '@') {
          if (data[1] == '+') {
            inverter.starteBatterie("Websockets");
          } else if (data[1] == '-') {
            inverter.starteNetzvorrang("Websockets");
          } else if (data[1] == 's' && len > 3) {
            //Solar charger s1 an / ausschalten
            if (data[2] == '1') {
              if (data[3] == '+') {
                //s1 anschalten
                charger.toggleCharger(1, true, true);
                wc.sendClients("s1 an");
              } else {
                //s1 abschalten
                charger.toggleCharger(1, false, true);
                wc.sendClients("s1 aus");
              }
            } else {
              if (data[3] == '+') {
                //s2 anschalten
                charger.toggleCharger(2, true, true);
                wc.sendClients("s2 an");
              } else {
                //s2 abschalten
                charger.toggleCharger(2, false, true);
                wc.sendClients("s2 aus");
              }
            }
          }
      } else if(data[0] == '{') {
        //Umstellung auf JSon
        StaticJsonDocument<300> doc;
        deserializeJson(doc, data);

        String msg = "";
        bool saveConfig = false;
        
        bool updateTeslaCtrlActive = doc["ta"];
        if(updateTeslaCtrlActive != teslaCtrlActive) {  
          saveConfig = true;      
          teslaCtrlActive = updateTeslaCtrlActive;
          buildMessage(&msg, "teslaCtrlActive", String(teslaCtrlActive).c_str());
        }

        bool updateDbg1 = doc["d1"];
        if(updateDbg1 != debug) {        
          debug = updateDbg1;
          buildMessage(&msg, "debug", String(debug).c_str());
        }
        bool updateDbg2 = doc["d2"];
        if(updateDbg2 != debug2) {        
          debug2 = updateDbg2;
          buildMessage(&msg, "debug2", String(debug2).c_str());
        }
        
        if(saveConfig) {
          config.save();
          wc.sendClients(msg);
        }
        //nun die Info an alle zum Umpdate der UI versenden
        updateUi();       
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
