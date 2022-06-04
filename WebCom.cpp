#include "global.h"
#include <ArduinoJson.h>

/**
   Sende Daten zu allen über Websockets verbundenen
   Clients. 
*/
void WebCom::sendClients(const char* msg) { 
  ws.textAll(msg);
}

void WebCom::sendClients(const char* msg, bool save) { 
  //Fixme save msg (wenn client sich connected, bekommt er alle im Buffer liegenden Nachrichten)
  String str = String((char*)0);
  str = timeClient.getFormattedDate();
  str += ": ";
  str += msg;
  ws.textAll(str);
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
        if(debugWeb) {
          bitset.setCharAt(1,49);
        } else {
          bitset.setCharAt(1,48);
        }     
        if(debugJson) {
          bitset.setCharAt(2,49);
        } else {
          bitset.setCharAt(2,48);
        } 
        if(debugRelais) {
          bitset.setCharAt(3,49);
        } else {
          bitset.setCharAt(3,48);
        } 
        if(debugSma) {
          bitset.setCharAt(4,49);
        } else {
          bitset.setCharAt(4,48);
        }        
        if(debugSbms) {
          bitset.setCharAt(5,49);
        } else {
          bitset.setCharAt(5,48);
        }    
        if(debugCharger) {
          bitset.setCharAt(6,49);
        } else {
          bitset.setCharAt(6,48);
        }   
        if(debugBattery) {
          bitset.setCharAt(7,49);
        } else {
          bitset.setCharAt(7,48);
        }  
        if(debugInverter) {
          bitset.setCharAt(8,49);
        } else {
          bitset.setCharAt(8,48);
        }            
        if(debugConfig) {
          bitset.setCharAt(9,49);
        } else {
          bitset.setCharAt(9,48);
        }      
        if(debugTesla) {
          bitset.setCharAt(10,49);
        } else {
          bitset.setCharAt(10,48);
        } 
        if(battery.enabled) {
          bitset.setCharAt(11,49);
        } else {
          bitset.setCharAt(11,48);
        } 
        if(fastResponse) {
          bitset.setCharAt(12,49);
        } else {
          bitset.setCharAt(12,48);
        } 
        if(charger.automatedCharging) {
          bitset.setCharAt(13,49);
        } else {
          bitset.setCharAt(13,48);
        } 
        if(charger.s1Activated) {
          bitset.setCharAt(14,49);
        } else {
          bitset.setCharAt(14,48);
        }  
        if(inverter.dauerbetrieb) {
          bitset.setCharAt(15,49);
        } else {
          bitset.setCharAt(15,48);
        }  
        doc["dbg"]=bitset;
        doc["s1"]=charger.isChargerOn(1);
        doc["s2"]=charger.isChargerOn(2);
        doc["b"]=battery.isOn();
        doc["n"]=netto;
        if(sbms.data != NULL && sbms.data.length() == 60 && sbms.data.charAt(0) != 'P') {
           doc["d"]=sbms.data;
        }
        if(perry.hasUpdate()) {
          doc["rts"] = perry.status();
        }
        doc["cs"] = perry.isCharging();
        doc["sc"] = perry.getSoC();
        doc["bl"] = perry.getSoC();
        doc["dt"]=datetime;
        doc["t"]=temp;
        doc["ta"]=teslaCtrlActive;
        doc["str1"] = controller.string1;
        doc["str2"] = controller.string2;
        doc["pgl"] = controller.pegel;
        doc["temp"] = controller.temp;
        doc["vl"] = controller.vorlauf;
        doc["rl"] = controller.ruecklauf;
        char jsonChar[512];
        serializeJson(doc, jsonChar);
        String str(jsonChar);
        if(debugJson) {
          Serial.println(str);
        }
        if(debugWeb) {
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
       
          //0.9.9.95 Verzicht auf: client->text(F("@ Connected"));
         
          //Aktualisieren von debug/debug1/s1/s2/netz bzw batt
          wc.updateUi(client, false);  
          logs.print(true); //die letzten Ereignisse zum client schreiben    
                 
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
            charger.toggleCharger(S1, s1, false);
            buildMessage(&msg, "S1", String(s1).c_str());
          }
          yield();
        }  
        
        if(doc.containsKey("s2")) { //Charger S2
          bool s2 = doc["s2"];
          if(s2 != charger.isChargerOn(S2)) {    
            update = true;    
            charger.toggleCharger(S2, s2, false);
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
                  debugWeb = (c == 49);   
                  update = true;      
                  buildMessage(&msg, "debugWeb", String(debugWeb).c_str());      
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
              case 3:
                if(c != bitset.charAt(i)) {
                  bitset.setCharAt(i, c);
                  debugRelais = (c == 49);   
                  update = true;      
                  buildMessage(&msg, "debugRelais", String(debugRelais).c_str());      
                }
                break; 
              case 4:
                if(c != bitset.charAt(i)) {
                  bitset.setCharAt(i, c);
                  debugSma = (c == 49);   
                  update = true;      
                  buildMessage(&msg, "debugSma", String(debugSma).c_str());      
                }
                break;  
              case 5:
                if(c != bitset.charAt(i)) {
                  bitset.setCharAt(i, c);
                  debugSbms = (c == 49);   
                  update = true;      
                  buildMessage(&msg, "debugSbms", String(debugSbms).c_str());      
                }
                break;
              case 6:
                if(c != bitset.charAt(i)) {
                  bitset.setCharAt(i, c);
                  debugCharger = (c == 49);   
                  update = true;      
                  buildMessage(&msg, "debugCharger", String(debugCharger).c_str());      
                }
                break;         
              case 7:
                if(c != bitset.charAt(i)) {
                  bitset.setCharAt(i, c);
                  debugBattery = (c == 49);   
                  update = true;      
                  buildMessage(&msg, "debugBattery", String(debugBattery).c_str());      
                }
                break; 
              case 8:
                if(c != bitset.charAt(i)) {
                  bitset.setCharAt(i, c);
                  debugInverter = (c == 49);   
                  update = true;      
                  buildMessage(&msg, "debugInverter", String(debugInverter).c_str());      
                }
                break;   
              case 9:
                if(c != bitset.charAt(i)) {
                  bitset.setCharAt(i, c);
                  debugConfig = (c == 49);   
                  update = true;      
                  buildMessage(&msg, "debugConfig", String(debugConfig).c_str());      
                }
                break;    
              case 10:
                if(c != bitset.charAt(i)) {
                  bitset.setCharAt(i, c);
                  debugTesla = (c == 49);   
                  update = true;      
                  buildMessage(&msg, "debugTesla", String(debugTesla).c_str());      
                }
                break;    
              case 11:
                if(c != bitset.charAt(i)) {
                  bitset.setCharAt(i, c);
                  battery.enabled = (c == 49);   
                  update = true;      
                  buildMessage(&msg, "battery.enabled", String(battery.enabled).c_str());      
                  config.save();
                }
                break;        
              case 12:
                if(c != bitset.charAt(i)) {
                  bitset.setCharAt(i, c);
                  fastResponse = (c == 49);   
                  update = true;      
                  buildMessage(&msg, "fastResponse", String(fastResponse).c_str());      
                  config.save();
                }
                break;      
             case 13:
                if(c != bitset.charAt(i)) {
                  bitset.setCharAt(i, c);
                  charger.automatedCharging = (c == 49);   
                  update = true;      
                  buildMessage(&msg, "charger.automatedCharging", String(charger.automatedCharging).c_str());      
                  config.save();
                }
                break;
             case 14:
                if(c != bitset.charAt(i)) {
                  bitset.setCharAt(i, c);
                  charger.s1Activated = (c == 49);   
                  update = true;      
                  buildMessage(&msg, "charger.s1Activated", String(charger.s1Activated).c_str());      
                  config.save();
                }
                break;    
             case 15:
                if(c != bitset.charAt(i)) {
                  bitset.setCharAt(i, c);
                  inverter.dauerbetrieb = (c == 49);   
                  update = true;      
                  buildMessage(&msg, "inverter.dauerbetrieb", String(inverter.dauerbetrieb).c_str());      
                  config.save();
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
            Serial.println(msg);
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
          msg+=F("; ");
          yield();
        }  

        if(doc.containsKey("rts_reset")) { //rts update reset, es werden keine weiteren Daten an den Client gesendet
          perry.reset(); //keine Updates mehr
          msg+=F("Reset Tesla Request Status ( perry.reset() ); ");   
          yield(); 
        }  

        if(doc.containsKey("wt")) { //wt, request Tesla wakeup
          update = true;
          int rc = perry.wakeup();     
          msg+=F("Requested Tesla wakeup; Statuscode: ");    
          msg+=rc;
          msg+=F("; ");
          wc.sendJson("wt", String(rc).c_str());  //GUI-Aktualisierung     
          yield();
        }

        if(doc.containsKey("auth")) { //wt, request Tesla wakeup
          update = true;
          const char* pw = config.load("TESLAAUTH"); 
          int rc = perry.authorize(pw);     
          msg+=F("Requested Tesla reauthentication; Statuscode: ");    
          msg+=rc;
          msg+=F("; ");
          yield();
        }

        if(doc.containsKey("lm")) { //lm set charge limit
          update = true;
          int limit = doc["lm"];
          int rc = perry.setChargeLimit(limit);     
          msg+=F("Server: Requested charge limit ");
          msg+=limit;
          msg+=F("%; Statuscode: ");    
          msg+=rc;
          msg+=F("; ");
          wc.sendJson("lm", String(rc).c_str());  //GUI-Aktualisierung     
          yield();
        }

        if(doc.containsKey("ch")) { //ch, request Tesla charge start/stop
          bool chargeRequest = doc["ch"];
          int rc;
          if(chargeRequest) {
              update = true;
              rc = perry.startCharge();
              if(rc == 200) {    
                 perry.setCharging(true);
                 updateUi();
              }
              msg+=F("Server: Requested Tesla charge start; Statuscode: ");    
              msg+=rc;            
              msg+=F("; ");
          } else {
              update = true;
              int rc = perry.stopCharge();  
              if(rc == 200) { 
                  perry.setCharging(false); 
                  updateUi();
              }   
              msg+=F("Server: Requested Tesla charge stop; Statuscode: ");     
              msg+=rc;    
              msg+=F("; ");        
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
        msg+=F("; ");
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
    msg->concat(F("; "));

    String s = name;
    float sens;
    sens = voltageSensor.getSensitivity();
    if(s.equals("debugWeb")) {    
        sens-=0.00001;
    } else {
        sens+=0.00001;       
    }
    voltageSensor.setSensitivity(sens);
    Serial.println(sens, 6);
}

void WebCom::print() {
    Serial.println(F("--------------------------------"));
    Serial.print(F("WebCom clients: "));
    Serial.println(ws.count());  
}
