#include "global.h"
#include <ArduinoJson.h>

/**
   Sende Daten zu allen über Websockets verbundenen
   Clients. 
*/
void WebCom::sendClients(const char* msg) { 
  ws.textAll(msg);
}

void WebCom::sendJson(const char* key, const char* value) {
  int len = strlen(key) + strlen(value) + 8;
  String msg = String((char *)0);
  msg.reserve(len);
  msg+=F("{\"");
  msg+=key;
  msg+=F("\":\"");
  msg+=value;
  msg+=F("\"}");
  ws.textAll(msg.c_str());
}

void WebCom::updateUi(AsyncWebSocketClient *client, bool all) {
        //mit JSON
        StaticJsonDocument<512> doc; 
        
        if(debug) {
          bitset.setCharAt(0,49);
        } else {
          bitset.setCharAt(0,48);
        }
        if(debug2) {
          bitset.setCharAt(1,49);
        } else {
          bitset.setCharAt(1,48);
        }     
        if(debugJson) {
          bitset.setCharAt(2,49);
        } else {
          bitset.setCharAt(2,48);
        }  
        doc["dbg"]=bitset;
        
        doc["s1"]=charger.isChargerOn(1);
        doc["s2"]=charger.isChargerOn(2);
        doc["b"]=battery.isOn();
        doc["l"]=lieferung;
        doc["z"]=bezug;
        if(sbmsData != NULL && sbmsData.length() > 10) {
           doc["d"]=sbmsData;
        }
        if(perry.hasUpdate()) {
          doc["rts"] = perry.status();
          doc["cs"] = perry.isCharging();
        }
        doc["dt"]=datetime;
        doc["t"]=temp;
        doc["ta"]=teslaCtrlActive;
        char jsonChar[512];
        serializeJson(doc, jsonChar);
        String str(jsonChar);
        if(debugJson) {
          Serial.println(str);
        }
        if(debug2) {
          doc["fh"]=ESP.getFreeHeap();
        }
        if (all) {
          ws.textAll(str.c_str());
        } else {
          ws.text(client->id(), str.c_str());
        }
}

void WebCom::updateUi() {
        updateUi(NULL, true);
}

void WebCom::onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len){

  switch (type) {
    case WS_EVT_CONNECT: {       
       
          client->text(F("@ Connected"));
         
          //Aktualisieren von debug/debug1/s1/s2/netz bzw batt
          wc.updateUi(client, false);             
          break;
      }    
      case WS_EVT_DATA: 
      
      if(debug) {
        Serial.println(String((char *)data));
      }
      if(data[0] == '{') {
        if(debugJson) {
          Serial.println(String((char *)data));
        }
        //Umstellung auf JSon
        StaticJsonDocument<300> doc;
        deserializeJson(doc, data);

        String msg = String((char*)0);
        msg.reserve(63);
        bool update = false;
        bool saveConfig = false;
        
        bool updateTeslaCtrlActive = doc["ta"];
        if(updateTeslaCtrlActive != teslaCtrlActive) {  
          update = true;
          saveConfig = true;      
          teslaCtrlActive = updateTeslaCtrlActive;
          buildMessage(&msg, "teslaCtrlActive", String(teslaCtrlActive).c_str());
          yield();
        } 
        
        if(doc.containsKey("s1")) { //Charger S1
          bool s1 = doc["s1"];
          if(s1 != charger.isChargerOn(S1)) {    
            update = true;    
            charger.toggleCharger(S1, s1, true, false);
            buildMessage(&msg, "S1", String(s1).c_str());
          }
          yield();
        }  
        
        if(doc.containsKey("s2")) { //Charger S2
          bool s2 = doc["s2"];
          if(s2 != charger.isChargerOn(S2)) {    
            update = true;    
            charger.toggleCharger(S2, s2, true, false);
            buildMessage(&msg, "S2", String(s2).c_str());
          }
          yield();
        } 

        if(doc.containsKey("dbg")) { //0.9.9.54 Bitset statt Debugflags
          String bs = doc["dbg"]; 
          for(int i=0; i<bs.length(); i++) {
            char c = bs.charAt(i);
            switch(i) {
              case 0:
                if(c != bitset.charAt(i)) {
                  bitset.setCharAt(i, c);
                  debug = (c == 49);   
                  update = true;      
                  buildMessage(&msg, "debug", String(debug).c_str());      
                }
                break;
              case 1:
                if(c != bitset.charAt(i)) {
                  bitset.setCharAt(i, c);
                  debug2 = (c == 49);   
                  update = true;      
                  buildMessage(&msg, "debug2", String(debug2).c_str());      
                }
                break;      
              case 2:
                if(c != bitset.charAt(i)) {
                  bitset.setCharAt(i, c);
                  debugJson = (c == 49);   
                  update = true;      
                  buildMessage(&msg, "debugJson", String(debugJson).c_str());      
                }
                break;                 
              default:   
                if(c != bitset.charAt(i)) {
                  bitset.setCharAt(i, c);  
                  update = true;         
                }              
            }
          }
          if(update) {
            Serial.print(F("Aktualisierung von Bitset: "));
            Serial.println(bitset);
          }
          yield();
        }     
        
        if(doc.containsKey("vr")) {
          String netzOderBatt = doc["vr"];
          if(netzOderBatt == "netz") {       
            update = true;
            inverter.starteNetzvorrang(F("Websockets"));
            buildMessage(&msg, "Netzvorrang", "true");
          } else if(netzOderBatt == "batt") {
            update = true;
            inverter.starteBatterie(F("Websockets"));    
            buildMessage(&msg, "Battery", "true");      
          }
          yield();
        }  
        
        if(doc.containsKey("rts")) { //rts, requst Tesla status
          update = true;
          int rc = perry.readChargeState();     
          msg+=F("Updated tesla status information; status code: ");    
          msg+=rc;
          yield();
        }  

        if(doc.containsKey("rts_reset")) { //rts update reset, es werden keine weiteren Daten an den Client gesendet
          perry.reset(); //keine Updates mehr
          msg+=F("Reset Tesla Request Status ( perry.reset() )");   
          yield(); 
        }  

        if(doc.containsKey("wt")) { //wt, request Tesla wakeup
          update = true;
          int rc = perry.wakeup();     
          msg+=F("Requested Tesla wakeup; Statuscode: ");    
          msg+=rc;
          wc.sendJson("wt", String(rc).c_str());  //GUI-Aktualisierung     
          yield();
        }

        if(doc.containsKey("ch")) { //ch, request Tesla charge start/stop
          bool chargeRequest = doc["ch"];
          int rc;
          if(chargeRequest) {
              update = true;
              rc = perry.startCharge();
              if(rc == 200) {
                 wc.sendJson("cs","true");     
              }
              msg+=F("Requested Tesla charge start; Statuscode: ");    
              msg+=rc;            
          } else {
              update = true;
              int rc = perry.stopCharge();  
              if(rc == 200) {
                  wc.sendJson("cs","false");    
              }   
              msg+=F("Requested Tesla charge stop; Statuscode: ");     
              msg+=rc;            
          }     
          yield();    
        }
        
        if(saveConfig) {
          config.save();
          yield();
        }
        if(update) {
          wc.sendClients(msg.c_str());
          //nun die Info an alle zum Umpdate der UI versenden
          updateUi();         
          yield();     
        }   
      } else {
        String msg = String((char*)0);
        msg.reserve(32);
        msg+=F("Cannot process data: ");
        msg+=String((char *)data);
        wc.sendClients(msg.c_str());
      }
      break;
    }
}

void WebCom::buildMessage(String* msg, const char* name, const char* value) {
    msg->concat(F("Switch "));
    msg->concat(name);
    msg->concat(F(" to "));
    msg->concat(value);
}
