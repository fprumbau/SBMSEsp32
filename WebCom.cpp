#include "global.h"
#include <ArduinoJson.h>

/**
   Sende Daten zu allen über Websockets verbundenen
   Clients. Alles, was NICHT SBMS-Daten sind, also
   Fehler- bzw. Statusmeldungen MUSS mit einem '@'
   eingeleitet werden, sonst wird es von der Webseite
   falsch interpretiert und führt zu wilden Werten
   z.B. beim Batteriestatus.
*/
void WebCom::sendClients(String msg, bool data) { 
  if(!data) {
    msg = "@ " + msg;
  }
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
        root["battery"]=battery.isBatteryOn();
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
