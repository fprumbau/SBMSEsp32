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
        doc["d"]=sbmsData;
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
        doc["d"]=sbmsData;
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

/**
   Toggle:
   - @d1-true
   - @d1-false
   - @d2-true
   - @d2-false
*/
void WebCom::toggleDebug(unsigned char* payload) {
  String msg;
  if (payload[2] == '1') {
    if (payload[4] == 't') {
      debug = true;
      msg = F("Switched debug1 to true");
    } else {
      debug = false;
      msg = F("Switched debug1 to false");
    }
  } else {
    if (payload[4] == 't') {
      debug2 = true;
      msg = F("Switched debug2 to true");
    } else {
      debug2 = false;
      msg = F("Switched debug2 to false");
    }
  }
  if (debug2) {
    Serial.println(msg);
  }
  wc.sendClients(msg);
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
          if (data[1] == 'd' && len > 4) { 
            toggleDebug(data);
          }
      } else if(data[0] == '{') {
        //Umstellung auf JSon
        StaticJsonDocument<300> doc;
        deserializeJson(doc, data);
        teslaCtrlActive = doc["ta"];
        //nun die Info an alle zum Umpdate der UI versenden
        updateUi();       
      }
      break;
    }
}
