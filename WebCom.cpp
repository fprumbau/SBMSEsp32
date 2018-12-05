#include "global.h"
#include <ArduinoJson.h>

WebCom::WebCom() { }

IPAddress WebCom::remoteIP(uint8_t num) {
  return _wsServer.remoteIP(num);
}

bool WebCom::sendTXT(uint8_t num, String& payload) {
  return _wsServer.sendTXT(num, payload);
}

/**
   Sende Daten zu allen über Websockets verbundenen
   Clients. Alles, was NICHT SBMS-Daten sind, also
   Fehler- bzw. Statusmeldungen MUSS mit einem '@'
   eingeleitet werden, sonst wird es von der Webseite
   falsch interpretiert und führt zu wilden Werten
   z.B. beim Batteriestatus.
*/
void WebCom::sendClients(String msg, bool data) {
  if (clientCount <= 0) {
    return;
  }
  _wsServerLastSend = millis();
  for (int m = 0; m < clientCount; m++) {
    uint8_t client = clients[m];
    if (vars.debug) {
      Serial.printf("Sending client %u ( %u ) from %u clients\n", (m + 1), client, clientCount);
    }
    if (!data) {
      msg = "@ " + msg;
    }
    _wsServer.sendTXT(client, msg);
  }
}

void WebCom::loop() {
  _wsServer.loop();
}

void WebCom::begin(WebSocketServerEvent cbEvent) {
  _wsServer.onEvent(cbEvent);
  _wsServer.begin();
}

void WebCom::updateUi(uint8_t num) {
        //mit JSON
        StaticJsonBuffer<300> jsonBuffer; //letzte Zaehlung: 114
        JsonObject& root = jsonBuffer.createObject();
        root["d1"]=vars.debug;
        root["d2"]=vars.debug2;
        root["s1"]=sma.isChargerOn(1);
        root["s2"]=sma.isChargerOn(2);
        root["battery"]=battery.isBatteryOn();
        char jsonChar[512];
        root.printTo(jsonChar);
        String str(jsonChar);
        if(vars.debug2) {
          Serial.println(str);
        }
        if (num > 0) {
          wc.sendTXT(num, str);
        } else {
          sendClients(str, false); 
        }
}
