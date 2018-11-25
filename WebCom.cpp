#include "WebCom.h"

WebCom::WebCom(Vars& vars) {
  _vars = vars;
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
  /*if(wsServerLastSend>0 && (millis()-wsServerLastSend) < 100) {
    if(debug) {
      Serial.print("Could not send data multiple times in 100ms; disgarding ");
      Serial.println(data);
    }
    return;
    }*/
  wsServerLastSend = millis();
  for (int m = 0; m < clientCount; m++) {
    uint8_t client = clients[m];
    if (_vars.debug) {
      Serial.printf("Sending client %u ( %u ) from %u clients\n", (m + 1), client, clientCount);
    }
    if (!data) {
      msg = "@ " + msg;
    }
    wsServer.sendTXT(client, msg);
  }
}

void WebCom::loop() {
  wsServer.loop();
}

void WebCom::begin(WebSocketServerEvent cbEvent) {
  wsServer.onEvent(cbEvent);
  wsServer.begin();
}
