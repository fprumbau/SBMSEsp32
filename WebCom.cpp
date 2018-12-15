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
        StaticJsonBuffer<300> jsonBuffer; //letzte Zaehlung: 114
        JsonObject& root = jsonBuffer.createObject();
        root["d1"]=debug;
        root["d2"]=debug2;
        root["s1"]=sma.isChargerOn(1);
        root["s2"]=sma.isChargerOn(2);
        root["b"]=inverter.isBatteryOn();
        root["l"]=lieferung;
        root["z"]=bezug;
        root["d"]=sbmsData;
        char jsonChar[512];
        root.printTo(jsonChar);
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
      msg = "Switched debug1 to true";
    } else {
      debug = false;
      msg = "Switched debug1 to false";
    }
  } else {
    if (payload[4] == 't') {
      debug2 = true;
      msg = "Switched debug2 to true";
    } else {
      debug2 = false;
      msg = "Switched debug2 to false";
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
                sma.toggleCharger(1, true, true);
                wc.sendClients("s1 an");
              } else {
                //s1 abschalten
                sma.toggleCharger(1, false, true);
                wc.sendClients("s1 aus");
              }
            } else {
              if (data[3] == '+') {
                //s2 anschalten
                sma.toggleCharger(2, true, true);
                wc.sendClients("s2 an");
              } else {
                //s2 abschalten
                sma.toggleCharger(2, false, true);
                wc.sendClients("s2 aus");
              }
            }
          }
          if (data[1] == 'd' && len > 4) { 
            toggleDebug(data);
          }
      }
      break;
    }
}
